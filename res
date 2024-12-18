Key == 1
client_body_timeout -> TUAMAMMA
client_max_body_size -> 10M
index -> index.html
root -> Root
---LOCATIONS---
4
Key -> /
root -> /tmp/www
try_files -> $uri $uri/ =404
Key -> /kapouet
index -> index.html
root -> /www
Key -> /kapouet/upload
client_max_body_size -> 50M
dav_methods -> GET POST DELETE
root -> /www
upload_pass -> /tmp/uploads
upload_store -> /tmp/uploads/
Key -> \.php$
fastcgi_index -> index.php
fastcgi_param -> SCRIPT_FILENAME /tmp/www$fastcgi_script_name
fastcgi_pass -> 127.0.0.1:9000
include -> fastcgi_params
listen:
localhost:9191
1234
server name:
localhost
dreamteam
listing: 0

Key == 2
client_body_timeout -> 60s
client_max_body_size -> 20M
index -> index.html
root -> Root/example
---LOCATIONS---
3
Key -> /
root -> /www/example
try_files -> $uri $uri/ =404
Key -> /images
index -> index.html
root -> /www
Key -> \.cgi$
fastcgi_index -> index.cgi
fastcgi_param -> SCRIPT_FILENAME /var/www/example$fastcgi_script_name
fastcgi_pass -> 127.0.0.1:9000
include -> fastcgi_params
listen:
9090
8082
server name:
localhost
example.html
listing: 1

client_body_timeout -> 60s
client_max_body_size -> 10M
include -> /etc/nginx/fastcgi_params
