[34mServer 1: starting
[0mServer running on port [32m8080[0m
Available routes:
[33m/
[0m[33m/www/kapouet
[0m[33m/php
[0m[33m/upload
[0m[34mServer 2: starting
[0mServer running on port [32m9090[0m
Server running on port [32m8086[0m
Available routes:
[33m/www/example/
[0m[33m/cgi
[0m[33m/www/images
[0mHTTP/1.1 200 OK
Content-Type: text/html
Connection: keep-alive

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>POST Request Body Examples</title>
</head>
<body>
    <h1>POST Request Body Types</h1>

    <h2>1. application/x-www-form-urlencoded</h2>
    <form action="/upload" method="POST">
        <label for="name">Name: </label>
        <input type="text" id="name" name="name" value="Matteo"><br><br>
        <label for="email">Email: </label>
        <input type="text" id="email" name="email" value="matte@gmail.com"><br><br>
        <button type="submit">Submit (application/x-www-form-urlencoded)</button>
    </form>

    <h2>2. multipart/form-data (File Upload)</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
        <label for="file">Upload File: </label>
        <input type="file" id="file" name="file"><br><br>
        <button type="submit">Submit (multipart/form-data)</button>
    </form>

    <h2>3. application/json</h2>
    <form action="/upload" method="POST" id="jsonForm">
        <label for="jsonData">JSON Data:</label><br>
        <textarea id="jsonData" name="jsonData" rows="6" cols="50">
{
  "name": "Matteo",
  "email": "matte@gmail.com"
}
        </textarea><br><br>
        <button type="submit">Submit (application/json)</button>
    </form>

    <h2>4. application/xml</h2>
    <form action="/upload" method="POST">
        <label for="xmlData">XML Data:</label><br>
        <textarea id="xmlData" name="xmlData" rows="6" cols="50">
<user>
  <name>Matteo</name>
  <email>matte@gmail.com</email>
</user>
        </textarea><br><br>
        <button type="submit">Submit (application/xml)</button>
    </form>

    <h2>5. text/plain</h2>
    <form action="/upload" method="POST">
        <label for="textData">Plain Text:</label><br>
        <textarea id="textData" name="textData" rows="6" cols="50">This is a plain text body.</textarea><br><br>
        <button type="submit">Submit (text/plain)</button>
    </form>

    <h2>6. application/octet-stream (Binary Data)</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
        <label for="binaryFile">Upload Binary File:</label>
        <input type="file" id="binaryFile" name="binaryFile"><br><br>
        <button type="submit">Submit (application/octet-stream)</button>
    </form>

    <h2>7. application/ld+json (JSON-LD)</h2>
    <form action="/upload" method="POST">
        <label for="ldJsonData">JSON-LD Data:</label><br>
        <textarea id="ldJsonData" name="ldJsonData" rows="6" cols="50">
{
  "@context": "http://schema.org",
  "@type": "Person",
  "name": "Matteo",
  "email": "matte@gmail.com"
}
        </textarea><br><br>
        <button type="submit">Submit (application/ld+json)</button>
    </form>

    <h2>8. text/csv (CSV Data)</h2>
    <form action="/upload" method="POST">
        <label for="csvData">CSV Data:</label><br>
        <textarea id="csvData" name="csvData" rows="6" cols="50">
name,email
Matteo,matte@gmail.com
        </textarea><br><br>
        <button type="submit">Submit (text/csv)</button>
    </form>

    <h2>9. application/graphql (GraphQL)</h2>
    <form action="/upload" method="POST">
        <label for="graphqlQuery">GraphQL Query:</label><br>
        <textarea id="graphqlQuery" name="graphqlQuery" rows="6" cols="50">
{
  "query": "query { user { name, email } }"
}
        </textarea><br><br>
        <button type="submit">Submit (application/graphql)</button>
    </form>

    <h2>10. application/protobuf (Protocol Buffers)</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
        <label for="protobufFile">Upload Protobuf File:</label>
        <input type="file" id="protobufFile" name="protobufFile"><br><br>
        <button type="submit">Submit (application/protobuf)</button>
    </form>

    <h2>11. text/event-stream (Server-Sent Events)</h2>
    <form action="/upload" method="POST">
        <label for="eventStreamData">Event Stream Data:</label><br>
        <textarea id="eventStreamData" name="eventStreamData" rows="6" cols="50">
data: {"message": "Hello"}
        </textarea><br><br>
        <button type="submit">Submit (text/event-stream)</button>
    </form>

    <h2>12. application/zip (ZIP File Upload)</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
        <label for="zipFile">Upload ZIP File:</label>
        <input type="file" id="zipFile" name="zipFile"><br><br>
        <button type="submit">Submit (application/zip)</button>
    </form>
    
</body>
</html>

PORCACCIO I DIO
