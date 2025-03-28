#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb

# Enable detailed error reporting
cgitb.enable()

print("Content-Type: text/html\r\n\r\n")
print("<html><head><title>Python CGI Test</title></head><body>")
print("<h1>Python CGI Test</h1>")

# Print request method
print(f"<p>Request Method: {os.environ.get('REQUEST_METHOD', 'Unknown')}</p>")

# Handle POST data
if os.environ.get('REQUEST_METHOD') == 'POST':
    print("<p>POST Data:</p>")
    print("<pre>")
    try:
        form = cgi.FieldStorage()
        for key in form.keys():
            print(f"{key}: {form[key].value}")
    except Exception as e:
        print(f"Error processing form data: {e}")
    print("</pre>")

# Handle GET data
if os.environ.get('REQUEST_METHOD') == 'GET':
    print("<p>GET Data:</p>")
    print("<pre>")
    try:
        form = cgi.FieldStorage()
        for key in form.keys():
            print(f"{key}: {form[key].value}")
    except Exception as e:
        print(f"Error processing form data: {e}")
    print("</pre>")

# Print only selected environment variables
print("<p>Environment Variables:</p>")
print("<pre>")
selected_vars = [
    "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", 
    "PATH_INFO", "PATH_TRANSLATED", "QUERY_STRING", 
    "REDIRECT_STATUS", "REMOTE_ADDR", "REMOTE_HOST", 
    "REQUEST_METHOD", "SCRIPT_FILENAME", "SCRIPT_NAME", 
    "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", 
    "SERVER_SOFTWARE"
]

for key in sorted(selected_vars):
    if key in os.environ:
        print(f"{key}: {os.environ[key]}")
print("</pre>")

print("</body></html>")
