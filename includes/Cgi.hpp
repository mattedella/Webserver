#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include "Request.hpp"

class CGIHandler {
    private:
        std::map<std::string, std::string> _env;
        std::string _cgiPath;
        std::string _scriptPath;
        std::string _scriptName;
        std::string _content;
        std::string _workingDir;
        int _status;
        int _timeout;

        void setupEnvironment(Request* req);
        std::string processChunkedBody(const std::string& body);
        void executeCGI(const std::string& requestBody);
        std::string processCGIOutput(int fd);
        bool setNonBlocking(int fd);

    public: 
        CGIHandler();
        CGIHandler(const std::string& cgiPath, const std::string& scriptPath);
        ~CGIHandler();
        
        bool execute(Request* req);
        std::string getOutput() const;
        int getStatus() const;
        void setTimeout(int seconds);
        void clear();
};

#endif