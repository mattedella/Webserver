#include <algorithm>
#include <cctype>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "../includes/webserv.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>
#include <signal.h>
#include <sys/time.h>

CGIHandler::CGIHandler() : _status(0), _timeout(30) {}

CGIHandler::CGIHandler(const std::string& cgiPath, const std::string& scriptPath)
    : _cgiPath(cgiPath), _scriptPath(scriptPath), _status(0), _timeout(30) {
    size_t lastSlash = scriptPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        _scriptName = scriptPath.substr(lastSlash + 1);
        _workingDir = scriptPath.substr(0, lastSlash);
    } else {
        _scriptName = scriptPath;
        _workingDir = ".";
    }
}

CGIHandler::~CGIHandler() {
    clear();
}

void CGIHandler::clear() {
    _env.clear();
    _content.clear();
    _status = 0;
}

void CGIHandler::setTimeout(int seconds) {
    _timeout = seconds;
}

void CGIHandler::setupEnvironment(Request* req) {
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["SERVER_SOFTWARE"] = "WebServ/1.0";
    _env["SERVER_NAME"] = req->getHeader("Host");
    _env["REQUEST_METHOD"] = req->getMethod();
    _env["SCRIPT_FILENAME"] = _scriptPath;
    _env["SCRIPT_NAME"] = "/" + _scriptName;
    _env["PATH_INFO"] = _scriptPath;
    _env["PATH_TRANSLATED"] = _scriptPath;
    std::string url = req->getURL();
    size_t queryPos = url.find('?');
    if (queryPos != std::string::npos)
        _env["QUERY_STRING"] = url.substr(queryPos + 1);
    else {
        std::string queryString = _env["QUERY_STRING"];
        if (queryString.empty())
            _env["QUERY_STRING"] = "";
    }
    _env["CONTENT_TYPE"] = req->getHeader("Content-Type");
    _env["CONTENT_LENGTH"] = req->getHeader("Content-Length");
    _env["REMOTE_ADDR"] = "127.0.0.1";
    _env["REMOTE_HOST"] = "localhost";
    std::string host = req->getHeader("Host");
    size_t colonPos = host.find(':');
    if (colonPos != std::string::npos)
        _env["SERVER_PORT"] = host.substr(colonPos + 1);
    else
        _env["SERVER_PORT"] = "80";
    _env["REDIRECT_STATUS"] = "200";
    for (std::map<std::string, std::string>::const_iterator it = req->getHeaders().begin(); 
         it != req->getHeaders().end(); ++it) {
        std::string headerName = it->first;
        std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::toupper);
        std::replace(headerName.begin(), headerName.end(), '-', '_');
        _env["HTTP_" + headerName] = it->second;
    }
}

std::string CGIHandler::processChunkedBody(const std::string& body) {
    if (body.empty())
        return "";
    std::stringstream result;
    size_t pos = 0;
    
    while (pos < body.length()) {
        size_t endOfSize = body.find("\r\n", pos);
        if (endOfSize == std::string::npos)
            break;
        std::string sizeHex = body.substr(pos, endOfSize - pos); // chunk size
        size_t chunkSize = 0;
        std::stringstream ss;
        ss << std::hex << sizeHex;
        ss >> chunkSize;
        
        if (chunkSize == 0)
            break;
        pos = endOfSize + 2;
        // estrae il chunk
        if (pos + chunkSize <= body.length()) {
            result << body.substr(pos, chunkSize);
            pos += chunkSize;
            // Skip the CRLF after the chunk data
            if (pos + 2 <= body.length() && body.substr(pos, 2) == "\r\n")
                pos += 2;
            else
                break;
        } else
            break;
    }
    return result.str();
}

bool CGIHandler::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
        return false;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
        return false;
    return true;
}

std::string CGIHandler::processCGIOutput(int fd) {
    std::string output;
    char buffer[4096];
    ssize_t bytes_read;
    setNonBlocking(fd);
    struct timeval start, current;
    gettimeofday(&start, NULL);
    bool is_done = false;
    
    while (!is_done) {
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            output += buffer;
            gettimeofday(&start, NULL);
        }
        else if (bytes_read == 0)
            is_done = true;
        else {
            usleep(10000);
            gettimeofday(&current, NULL);
            double elapsed = (current.tv_sec - start.tv_sec) + 
                          (current.tv_usec - start.tv_usec) / 1000000.0;
            
            if (elapsed >= _timeout) {
                std::cerr << "timeout CGI" << std::endl;
                is_done = true;
            }
        }
    }
    return output;
}

bool CGIHandler::execute(Request* req) {
    setupEnvironment(req);
    std::ifstream script_check(_scriptPath.c_str());
    if (!script_check.is_open()) {
        std::cerr << "CGI Error: Script not found or not readable: " << _scriptPath << std::endl;
        _content = "<html><body><h1>404 Not Found</h1><p>CGI script not found: " + _scriptPath + "</p></body></html>";
        _status = 404;
        return false;
    }
    script_check.close();
    std::string requestBody = "";
    std::string dataPath = _workingDir;
    if (dataPath.find("/cgi-bin") != std::string::npos)
        dataPath = _workingDir;
    else
        dataPath = _workingDir.substr(0, _workingDir.find_last_of('/')) + "/upload";
    static std::map<std::string, bool> processedGetRequests;
    std::string requestKey = req->getMethod() + ":" + req->getURL();
    if (req->getMethod() == "POST") {
        std::string transferEncoding = req->getHeader("Transfer-Encoding");
        requestBody = (transferEncoding == "chunked") ? 
                     processChunkedBody(req->getBodyContent()) : 
                     req->getBodyContent();
        if (!requestBody.empty()) {
            std::ofstream dataFile((dataPath + "/data.txt").c_str(), std::ios::out | std::ios::app);
            if (dataFile.is_open()) {

                if (req->getHeader("Content-Type").find("application/x-www-form-urlencoded") == 0) {
                    std::istringstream stream(requestBody);
                    std::string pair;
                    while (std::getline(stream, pair, '&'))
                        dataFile << pair << "\n";
                } else
                    dataFile << "raw_data=" << requestBody << "\n";
                dataFile << "------------\n";
                dataFile.close();
            }
        }
        if (req->getBodyMap().empty()) {
            for (std::map<std::string, std::string>::const_iterator it = req->getBodyMap().begin(); 
                it != req->getBodyMap().end(); ++it)
                requestBody += it->first + "=" + it->second + "&";
            if (!requestBody.empty() && requestBody[requestBody.length() - 1] == '&')
                requestBody.erase(requestBody.length() - 1);
        }
    } else if (req->getMethod() == "GET" && !_env["QUERY_STRING"].empty()) {
        requestKey += ":" + _env["QUERY_STRING"];
        if (processedGetRequests.find(requestKey) == processedGetRequests.end()) {
            processedGetRequests[requestKey] = true;
            std::ofstream dataFile((dataPath + "/data.txt").c_str(), std::ios::out | std::ios::app);
            if (dataFile.is_open()) {
                std::string queryString = _env["QUERY_STRING"];
                std::istringstream stream(queryString);
                std::string pair;
                while (std::getline(stream, pair, '&')) {
                    dataFile << pair << "\n";
                }
                dataFile << "------------\n";
                dataFile.close();
            }
        }
    }
    executeCGI(requestBody);
    return _status == 0;
}

void CGIHandler::executeCGI(const std::string& requestBody) {
    int input_pipe[2];
    int output_pipe[2];
    
    if (pipe(input_pipe) < 0 || pipe(output_pipe) < 0) {
        _status = 500;
        return;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        _status = 500;
        close(input_pipe[0]); close(input_pipe[1]);
        close(output_pipe[0]); close(output_pipe[1]);
        return;
    }

    if (pid == 0)
    {
        // Child process
        close(input_pipe[1]);
        close(output_pipe[0]);
        dup2(input_pipe[0], STDIN_FILENO);
        dup2(output_pipe[1], STDOUT_FILENO);
        dup2(output_pipe[1], STDERR_FILENO);
        close(input_pipe[0]);
        close(output_pipe[1]);
        
        if (chdir(_workingDir.c_str()) < 0)
            exit(1);
        // Env setup
        char **envp = new char*[_env.size() + 1];
        int i = 0;
        for (std::map<std::string, std::string>::const_iterator it = _env.begin(); 
             it != _env.end(); ++it, ++i) {
            std::string env_var = it->first + "=" + it->second;
            envp[i] = new char[env_var.length() + 1];
            std::strcpy(envp[i], env_var.c_str());
        }
        envp[i] = NULL;
        
        char *args[3] = { const_cast<char*>(_cgiPath.c_str()), 
                          const_cast<char*>(_scriptPath.c_str()), 
                          NULL };
        execve(args[0], args, envp);
        
        for (int j = 0; j < i; j++)
            delete[] envp[j];
        delete[] envp;
        exit(1);
    }
    else {
        close(input_pipe[0]);
        close(output_pipe[1]);
        if (!requestBody.empty()) {
            setNonBlocking(input_pipe[1]);
            size_t total_written = 0;
            size_t to_write = requestBody.length();
            struct timeval write_start, write_current;
            gettimeofday(&write_start, NULL);
            while (total_written < to_write) {
                ssize_t written = write(input_pipe[1], 
                                       requestBody.c_str() + total_written, 
                                       to_write - total_written);
                if (written > 0)
                    total_written += written;
                else {
                    usleep(1000);
                    gettimeofday(&write_current, NULL);
                    double write_elapsed = (write_current.tv_sec - write_start.tv_sec) + 
                                         (write_current.tv_usec - write_start.tv_usec) / 1000000.0;
                    if (write_elapsed >= _timeout) {
                        _status = 504;
                        break;
                    }
                }
            }
        }
        close(input_pipe[1]);
        _content = processCGIOutput(output_pipe[0]);
        close(output_pipe[0]);
        int status;
        pid_t result;
        struct timeval start, current;
        gettimeofday(&start, NULL);
        bool timeout_occurred = false;
        
        while (true) {
            result = waitpid(pid, &status, WNOHANG);
            if (result > 0)
                break;
            else if (result == 0) {
                gettimeofday(&current, NULL);
                double elapsed = (current.tv_sec - start.tv_sec) + 
                               (current.tv_usec - start.tv_usec) / 1000000.0;
                if (elapsed >= _timeout) {
                    kill(pid, SIGTERM);
                    usleep(100000);
                    if (waitpid(pid, NULL, WNOHANG) == 0) {
                        kill(pid, SIGKILL);
                        waitpid(pid, NULL, 0);
                    }
                    timeout_occurred = true;
                    _status = 504;
                    break;
                }
                usleep(10000);
            } else {
                _status = 500;
                break;
            }
        }
        
        if (!timeout_occurred)
        {
            if (WIFEXITED(status))
                _status = WEXITSTATUS(status);
            else
                _status = 500;
        }
    }
}

std::string CGIHandler::getOutput() const { return _content;}

int CGIHandler::getStatus() const { return _status;}