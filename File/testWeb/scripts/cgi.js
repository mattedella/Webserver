document.addEventListener('DOMContentLoaded', function() {
    initializeFormElements();
    setupFormEventListeners();
    setupModalFunctionality();
});

function initializeFormElements() {
    const inputs = document.querySelectorAll('input');
    inputs.forEach(input => {
        input.addEventListener('focus', function() {
            const label = this.closest('.form-group').querySelector('label');
            if (label) {
                label.style.color = '#ffcc00';
            }
        });
        
        input.addEventListener('blur', function() {
            const label = this.closest('.form-group').querySelector('label');
            if (label) {
                label.style.color = '';
            }
        });
    });
}

function setupFormEventListeners() {

    document.querySelector('form[action="/cgi-bin/test.py"][method="get"]').addEventListener('submit', function(event) {
        event.preventDefault();
        handleFormSubmission(this, 'Python GET Response');
    });

    document.querySelector('form[action="/cgi-bin/test.py"][method="post"]:not([enctype="multipart/form-data"])').addEventListener('submit', function(event) {
        event.preventDefault();
        handleFormSubmission(this, 'Python POST Response');
    });

    document.querySelector('form[action="/cgi-bin/test.php"][method="get"]').addEventListener('submit', function(event) {
        event.preventDefault();
        handleFormSubmission(this, 'PHP GET Response');
    });

    document.querySelector('form[action="/cgi-bin/test.php"][method="post"]:not([enctype="multipart/form-data"])').addEventListener('submit', function(event) {
        event.preventDefault();
        handleFormSubmission(this, 'PHP POST Response');
    });
    
    document.querySelectorAll('form[enctype="multipart/form-data"]').forEach(form => {
        form.addEventListener('submit', function(event) {
            event.preventDefault();
            const scriptType = form.action.includes('test.php') ? 'PHP' : 'Python';
            handleFormSubmission(this, `${scriptType} File Upload Response`, true);
        });
    });
}

function setupModalFunctionality() {
    document.getElementById('close-modal').addEventListener('click', function() {
        hideModal();
    });
    
    window.addEventListener('click', function(event) {
        if (event.target === document.getElementById('response-modal')) {
            hideModal();
        }
    });
    
    document.addEventListener('keydown', function(event) {
        if (event.key === 'Escape' && document.getElementById('response-modal').style.display === 'flex') {
            hideModal();
        }
    });
}

function showModal(title, content) {
    const modal = document.getElementById('response-modal');
    document.getElementById('modal-title').textContent = title;
    document.getElementById('modal-body').innerHTML = content;
    
    modal.style.display = 'flex';
    
    void modal.offsetWidth;
    
    modal.classList.add('show');
}

function hideModal() {
    const modal = document.getElementById('response-modal');
    
    modal.classList.remove('show');
    
    setTimeout(() => {
        modal.style.display = 'none';
    }, 300);
}

function handleFormSubmission(form, title, isFile = false) {
    const originalAction = form.getAttribute('action');
    const originalMethod = form.getAttribute('method');
    
    const submitButton = form.querySelector('button[type="submit"]');
    const originalButtonText = submitButton.textContent;
    submitButton.innerHTML = `${originalButtonText} <span class="loading"></span>`;
    submitButton.disabled = true;
    
    const iframe = document.createElement('iframe');
    iframe.style.display = 'none';
    iframe.name = 'submit_frame_' + Math.random().toString(36).substring(2, 15);
    document.body.appendChild(iframe);
    
    const iframeLoadPromise = new Promise((resolve) => {
        iframe.onload = function() {
            try {
                const iframeContent = iframe.contentDocument || iframe.contentWindow.document;
                resolve(iframeContent.body.innerHTML);
            } catch(e) {
                resolve("<div class='error-message'>Response received but cannot be displayed in modal due to security restrictions</div>");
            }
            
            setTimeout(() => {
                document.body.removeChild(iframe);
            }, 1000);
        };
    });
    
    form.setAttribute('target', iframe.name);
    form.submit();
    
    if (!isFile) {
        if (originalMethod.toLowerCase() === 'get') {
            const formData = new FormData(form);
            const queryString = new URLSearchParams(formData).toString();
            
            fetch(`${originalAction}?${queryString}`)
                .then(response => response.text())
                .then(data => {
                    showModal(title, formatResponse(data));
                })
                .catch(() => {
                    iframeLoadPromise.then(content => {
                        showModal(title, formatResponse(content));
                    });
                })
                .finally(() => {
                    submitButton.innerHTML = originalButtonText;
                    submitButton.disabled = false;
                });
        } else {
            const formData = new FormData(form);
            
            fetch(originalAction, {
                method: 'POST',
                body: formData
            })
            .then(response => response.text())
            .then(data => {
                showModal(title, formatResponse(data));
            })
            .catch(() => {
                iframeLoadPromise.then(content => {
                    showModal(title, formatResponse(content));
                });
            })
            .finally(() => {
                submitButton.innerHTML = originalButtonText;
                submitButton.disabled = false;
            });
        }
    } else {
        iframeLoadPromise.then(content => {
            showModal(title, formatResponse(content));
            submitButton.innerHTML = originalButtonText;
            submitButton.disabled = false;
        });
    }
}

function formatResponse(content) {
    if (!content)
        return "<div class='error-message'>No response received</div>";
    try {
        if (content.trim().startsWith('{') || content.trim().startsWith('[')) {
            const jsonObj = JSON.parse(content);
            return `<pre>${JSON.stringify(jsonObj, null, 2)}</pre>`;
        }
    } catch (e) {
    }
    
    if (content.includes('<!DOCTYPE html>') || content.includes('<html>')) {
        return `<div class="html-response">${content}</div>`;
    }
    return `<div class="response-content">${content}</div>`;
}

const style = document.createElement('style');
style.textContent = `
    .html-response {
        background-color: rgba(40, 40, 40, 0.5);
        padding: 10px;
        border-radius: 5px;
        font-family: monospace;
        white-space: pre-wrap;
        word-break: break-word;
    }
    
    pre {
        background-color: rgba(40, 40, 40, 0.5);
        padding: 15px;
        border-radius: 5px;
        overflow-x: auto;
        color: #f8f8f2;
        font-family: monospace;
    }
    
    .response-content {
        line-height: 1.6;
        word-break: break-word;
    }
`;
document.head.appendChild(style);