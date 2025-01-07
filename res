PRINT ALL
Server
time out 60
root Root
max body size 10
listing 0
index index.html
listen:
127.0.0.1:9191
pippo:1234
255:8080
server name:
localhost
dreamteam
MAP
LOCATIONS
4
Key -> /
-----LOCATION------
root: /tmp/www
methods: 
index: index.html
bodySize: 30
Key -> /kapouet
-----LOCATION------
root: /www
methods: 
index: index.html
bodySize: 30
Key -> /kapouet/upload
-----LOCATION------
root: /www
methods: GET POST DELETE
index: 
bodySize: 50
Key -> \.php$
-----LOCATION------
root: 
methods: 
index: 
bodySize: 30
listen:
127.0.0.1:9191
pippo:1234
255:8080
server name:
localhost
dreamteam
listing: 0
PRINT ALL
Server
time out 60
root Root/example
max body size 20
listing 1
index index.html
listen:
9090
8082
server name:
localhost
example.html
MAP
LOCATIONS
3
Key -> /
-----LOCATION------
root: /www/example
methods: 
index: 
bodySize: 30
Key -> /images
-----LOCATION------
root: /www
methods: 
index: index.html
bodySize: 30
Key -> \.cgi$
-----LOCATION------
root: 
methods: 
index: 
bodySize: 30
listen:
9090
8082
server name:
localhost
example.html
listing: 1
Key == 1
---LOCATIONS---
4
Key -> /
-----LOCATION------
root: /tmp/www
methods: 
index: index.html
bodySize: 30
Key -> /kapouet
-----LOCATION------
root: /www
methods: 
index: index.html
bodySize: 30
Key -> /kapouet/upload
-----LOCATION------
root: /www
methods: GET POST DELETE
index: 
bodySize: 50
Key -> \.php$
-----LOCATION------
root: 
methods: 
index: 
bodySize: 30
listen:
127.0.0.1:9191
pippo:1234
255:8080
server name:
localhost
dreamteam
listing: 0

Key == 2
---LOCATIONS---
3
Key -> /
-----LOCATION------
root: /www/example
methods: 
index: 
bodySize: 30
Key -> /images
-----LOCATION------
root: /www
methods: 
index: index.html
bodySize: 30
Key -> \.cgi$
-----LOCATION------
root: 
methods: 
index: 
bodySize: 30
listen:
9090
8082
server name:
localhost
example.html
listing: 1

----HTTP----
body size: 10
include: /etc/nginx/fastcgi_params
Errors :
num: 400 = /40x.html
num: 500 = /50x.html
