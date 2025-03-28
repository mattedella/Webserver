<?php
header("Content-Type: text/html");

echo "<html><head><title>PHP CGI Test</title></head><body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Request Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    echo "<p>POST Data:</p>";
    echo "<pre>";
    print_r($_POST);
    echo "</pre>";
}

if ($_SERVER['REQUEST_METHOD'] == 'GET') {
    echo "<p>GET Data:</p>";
    echo "<pre>";
    print_r($_GET);
    echo "</pre>";
}

$selected_vars = array(
    "CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", 
    "PATH_INFO", "PATH_TRANSLATED", "QUERY_STRING", 
    "REDIRECT_STATUS", "REMOTE_ADDR", "REMOTE_HOST", 
    "REQUEST_METHOD", "SCRIPT_FILENAME", "SCRIPT_NAME", 
    "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", 
    "SERVER_SOFTWARE"
);

echo "<p>Environment Variables:</p>";
echo "<pre>";
foreach ($selected_vars as $var) {
    if (isset($_SERVER[$var])) {
        echo "$var: " . $_SERVER[$var] . "\n";
    }
}
while (true) {

}
echo "</pre>";

echo "</body></html>";
?>
