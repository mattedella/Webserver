// Gestione dell'upload del file
document.getElementById('upload-form').addEventListener('submit', function(event) {
    event.preventDefault();
    
    const formData = new FormData();
    const fileInput = document.getElementById('file');
    formData.append('file', fileInput.files[0]);

    fetch('/upload', {
        method: 'POST',
        body: formData,
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('response-message').innerHTML = 'File saved correctly';
    })
    .catch(error => {
        document.getElementById('response-message').innerHTML = 'An error occurred during the upload.';
    });
});

document.getElementById('delete-button').addEventListener('click', function(event) {
    event.preventDefault();
    
    const fileName = document.getElementById('file-name-input').value;

    if (!fileName) {
        document.getElementById('delete-response-message').innerHTML = 'Please enter a file name.';
        return;
    }

    const filePath = `${fileName}`;

    fetch(filePath, {
        method: 'DELETE',
    })
    .then(response => {
        if (response.ok) {
            return response.text();
        } else {
            throw new Error('File not found or could not be deleted');
        }
    })
    .then(data => {
        document.getElementById('delete-response-message').innerHTML = 'File deleted successfully';
    })
    .catch(error => {
        document.getElementById('delete-response-message').innerHTML = 'An error occurred during the deletion.';
    });
});

document.getElementById("file").addEventListener("change", function(event) {
    const file = event.target.files[0];
    const previewContainer = document.getElementById("preview-container");

    if (file) {
        const reader = new FileReader();
        reader.onload = function(e) {
            const imgElement = new Image();
            imgElement.src = e.target.result;
            previewContainer.innerHTML = '';
            previewContainer.appendChild(imgElement);
        };
        reader.readAsDataURL(file);
    }
});
