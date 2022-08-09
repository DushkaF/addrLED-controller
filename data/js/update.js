window.onload = function () {
    uploadUpdateFile();
}



function uploadUpdateFile() { // Без защиты от потери соединения! #TODO
    document.getElementById("update-btn").addEventListener("click", sendUpdateFiles);

    // добавляем вывод названия файла
    var elements = document.querySelectorAll('.software-update');
    for (var i = 0; i < elements.length; i++) {
        // console.log(elements[i].querySelector('.upload-file-name'));   
        elements[i].querySelector('input').onchange = function () {
            this.parentElement.querySelector('.upload-file-name').hidden = false;
            this.parentElement.querySelector('.upload-file-name').innerText = this.files[0].name.split(/(\\|\/)/g).pop();
            var fileExt = this.files[0].name.split('.').pop();
            this.parentElement.querySelector('.upload-file-name').classList.remove((fileExt != "bin") ? "success" : "fail");
            this.parentElement.querySelector('.upload-file-name').classList.add((fileExt == "bin") ? "success" : "fail");
            
        }
    }
}


function sendUpdateFiles() {
    var code_file = document.getElementById("binary-upload-code"),
        spiffs_file = document.getElementById("binary-upload-spiffs"),
        xhr = CreateRequest(),
        form = new FormData();
    if (!((code_file.files[0] != undefined && code_file.files[0].name.split('.').pop() == "bin") && (spiffs_file.files[0] != undefined && spiffs_file.files[0].name.split('.').pop() == "bin"))){
        showUploadState(false);
        return;
    }

    console.log("update file loaded", code_file.files[0]);
    
    form = new FormData();
    form.append("code", code_file.files[0]);
    form.append("spiffs", spiffs_file.files[0]);

    xhr.upload.onprogress = function (event) {
        console.log("onload");
        document.getElementById("update-progressbar").hidden = false;
        document.getElementById("update-progressbar").style.width = "" + ((event.loaded / event.total) * 100) + "%";
    }
    console.log("loading");
    xhr.open("POST", document.getElementById("update-btn").getAttribute("path"), true);
    // xhr.setRequestHeader('Access-Control-Allow-Origin', '*');
    changeTextBox(document.querySelector('#update-btn [show]'), document.getElementById("update-btn-updating"));
    // xhr.setRequestHeader('Access-Control-Allow-Methods', '*');
    xhr.send(form);
    xhr.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                console.log("Success ", this.status);
                showUploadState(true);
            } else {
                console.log("Not 200, ", this.status);
                showUploadState(false);
            }
        } else {
            console.log("send not loaded");
            // showUploadState(false);
        }
    }
}


function showUploadState(success) {
    var elements = document.querySelectorAll('.software-update');
    document.getElementById("update-progressbar").classList.add("fade-out");
    document.getElementById("update-progressbar").classList.add("fade");
    // for (var i = 0; i < elements.length; i++) {
    //     // console.log("Elem", i);
    //     elements[i].querySelector('.upload-file-name').classList.add(success ? "success" : "fail");
    // }

    var labels = document.querySelectorAll('#update-btn p');
    changeTextBox(document.querySelector('#update-btn [show]'), document.getElementById("update-btn-" + (success ? "success" : "fail")));
    if (success) {
        document.getElementById("update-btn").removeEventListener("click", sendUpdateFiles);
    }
    setTimeout(function () {
        document.getElementById("update-progressbar").hidden = true;
        document.getElementById("update-progressbar").classList.remove("fade-out");
        document.getElementById("update-progressbar").classList.remove("fade");
    }, 1000);
}

var changingText;
function changeTextBox(from, to) {
    from.removeAttribute("show");
    to.setAttribute("show", "");
    from.classList.add("fade-out-text");
    from.classList.add("fade");
    clearTimeout(changingText);
    setTimeout(function () {
        from.hidden = true;
        from.classList.remove("fade-out-text");
        from.classList.remove("fade");
        to.hidden = false;
        to.classList.add("fade-in-text");
        setTimeout(function () {
            from.classList.remove("fade-in-text");
        }, 200);
    }, 200);
}

function CreateRequest() {
    var Request = false;

    if (window.XMLHttpRequest) {
        //Gecko-совместимые браузеры, Safari, Konqueror
        Request = new XMLHttpRequest();
    }
    else if (window.ActiveXObject) {
        //Internet explorer
        try {
            Request = new ActiveXObject("Microsoft.XMLHTTP");
        }
        catch (CatchException) {
            Request = new ActiveXObject("Msxml2.XMLHTTP");
        }
    }

    if (!Request) {
        alert("Невозможно создать XMLHttpRequest");
    }

    return Request;
}