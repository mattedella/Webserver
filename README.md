# 🌐 Webserv

> "This is when you finally understand why URLs start with HTTP."

**Webserv** is a fully functional HTTP server written in **C++98**. It’s built from scratch without relying on any external libraries, and it follows the core principles of the HTTP/1.1 protocol. This project was developed as part of the 42 curriculum to deepen understanding of networking, I/O multiplexing, and HTTP communication.

---

## 🚀 Features

- ✅ Handles **GET**, **POST**, and **DELETE** HTTP methods
- ✅ Non-blocking I/O using `poll()` (or `select()`, `kqueue()`, or `epoll()`)
- ✅ Support for multiple clients simultaneously
- ✅ Fully configurable via a custom **configuration file**
- ✅ Serves static HTML, CSS, JS, image files, etc.
- ✅ Default error pages and customizable error responses
- ✅ Directory listing support
- ✅ Support for file upload via POST
- ✅ CGI execution (e.g., `php-cgi`, `python`)
- ✅ Multiple virtual servers, ports, and hostnames
- ✅ Compatible with standard web browsers

---

## 🧠 Learning Objectives

- Deep understanding of the **HTTP protocol**
- Building a web server using **low-level socket programming**
- Managing concurrency and multiplexing with `poll()`
- Parsing and applying custom configuration files
- Implementing **CGI** execution and dynamic content handling

---

## 🧪 Requirements

- Must be written in **C++98** standard
- Must use **non-blocking** file descriptors
- Only **one** `poll()` (or equivalent) for all client/server I/O
- No `read()`/`write()` calls without prior `poll()` check
- Must gracefully handle client disconnections and invalid requests
- No external libraries or frameworks allowed
- Only allowed syscalls/functions (e.g., `socket`, `poll`, `accept`, `recv`, etc.)
- Must compile with `-Wall -Wextra -Werror -std=c++98`
- Provide a **Makefile** with standard rules: `all`, `clean`, `fclean`, `re`

---

## ⚙️ Usage

### 🧱 Build

```bash
make
```

### 🛠 Run

```bash
./webserv [config_file]
```

- Multiple servers and routes can be configured in a single config file.

---

## 🔧 Configuration File

Inspired by NGINX syntax. Example capabilities:

- Define ports and hosts
- Define multiple `server` blocks with `server_name`
- Set custom `error_pages`
- Specify `client_max_body_size`
- Configure `location` blocks with:
  - Accepted methods (GET/POST/DELETE)
  - Root directory
  - Default index file
  - Directory listing on/off
  - CGI execution by file extension
  - File upload destination
  - HTTP redirection

Example snippet:

```nginx
server {
    listen 127.0.0.1:8080;
    server_name localhost;

    error_page 404 /errors/404.html;

    location / {
        root /var/www/html;
        index index.html;
        autoindex on;
    }

    location /upload {
        methods POST;
        upload_store /var/uploads;
    }

    location ~ \.php$ {
        cgi_pass /usr/bin/php-cgi;
    }
}
```

---

## 🧪 Testing

- Test with web browsers (e.g., Chrome, Firefox)
- Use `curl`, `telnet`, or Postman to inspect headers and status codes
- Compare with NGINX behavior when in doubt
- Perform stress tests to ensure stability

---

## 📂 Project Structure

```text
.
├── CGI
│   └── Cgi.cpp
├── cgi_tester
├── Class
│   ├── ABlock.cpp
│   ├── Conf2.cpp
│   ├── Conf.cpp
│   ├── Http.cpp
│   ├── Request.cpp
│   ├── Response.cpp
│   ├── Server2.cpp
│   └── Server.cpp
├── conf
│   └── webserv.conf
├── File
│   ├── 40x.html
│   ├── 50x.html
│   ├── cgi-bin
│   │   ├── data.txt
│   │   ├── test.php
│   │   └── test.py
│   ├── cgi.html
│   ├── en.subject.pdf
│   ├── favicon.ico
│   ├── index2.html
│   ├── index.html
│   ├── secondserv
│   │   ├── 40x.html
│   │   ├── 50x.html
│   │   └── index.html
│   ├── testWeb
│   │   ├── cgi.html
│   │   ├── index.html
│   │   ├── scripts
│   │   │   ├── cgi.js
│   │   │   └── index.js
│   │   └── styles
│   │       ├── cgi.css
│   │       └── index.css
│   ├── upload
│   │   ├── 40x.html
│   │   ├── 50x.html
│   │   ├── data.txt
│   │   ├── en.subject.pdf
│   │   ├── neri-foto-darte-natura-buio-sfondo.jpg
│   │   └── png-clipart-skellig-michael-chewbacca-star-wars-computer-icons-star-wars-text-logo-thumbnail.png
│   └── www
│       └── kapouet
│           └── index.html
├── includes
│   ├── Ablock.hpp
│   ├── Cgi.hpp
│   ├── Conf.hpp
│   ├── Exc.hpp
│   ├── Http.hpp
│   ├── HttpServer.hpp
│   ├── Request.hpp
│   ├── Response.hpp
│   ├── Server.hpp
│   └── webserv.hpp
├── main.cpp
├── Makefile
├── Parsing
│   └── Parsing.cpp
├── Response
│   └── GenerateResp.cpp
├── test.conf
├── tester
├── webserv
├── webserv.dSYM
│   └── Contents
│       ├── Info.plist
│       └── Resources
│           └── DWARF
│               └── webserv
└── YoupiBanane
    ├── nop
    │   ├── other.pouic
    │   └── youpi.bad_extension
    ├── Yeah
    │   └── not_happy.bad_extension
    ├── youpi.bad_extension
    └── youpi.bla

```

---

## 📄 License

This project is developed as part of the **42 School** curriculum. Feel free to reuse, modify, and learn from it!

---

## 🙌 Credits

Built by students at [42 Firenze](https://42firenze.it), with love and sockets ❤️
