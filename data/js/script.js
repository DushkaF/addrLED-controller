window.onload = function () {
   getSelectList();
}

var repeatRequest;
var existRequest;

import hueColor from './wheel.js';

function getSelectList() {
    makeGETRequest('../effects_set.json', decodeSetJSON, function (request, path, func) {
        waitGET(request, path, func);
    });
}

function decodeSetJSON(handler) {
    // showForm();
    console.log("content-type: ", handler.getResponseHeader('Content-Type'));
    console.log(handler.responseText);
    var resp = JSON.parse(handler.responseText);
    console.log(resp["effects"]);
    var effects = resp["effects"];
    pasteInListOnPage(effects);
}

function pasteInListOnPage(list) {
    if (list.length > 0) {
    }
    var selectList = document.getElementById("led-mode-select");
    for (var i = 0; i < list.length; i++) {
        var option = document.createElement('option');
        option.innerHTML = list[i];
        option.value = i+1;
        selectList.appendChild(option);
    }
}

document.getElementById("led-mode-select").onchange = function(){
    sendEffectsState(hueColor());
}

document.getElementById("speed-range").onchange = function(){
    sendEffectsState(hueColor());
}

var effectsSending = false;

export default function sendEffectsState(hueColor) {
    if (!effectsSending){
        effectsSending = true;
        var selectList = document.getElementById("led-mode-select");
        var speedSlider = document.getElementById("speed-range");
        // var hueColor = [50,50,50];
        var sendJson = { "effect": selectList.selectedIndex, "hue":hueColor, "speed": parseInt(speedSlider.value)};
        console.log(sendJson);
        makePOSTRequest("/selectedEffects", sendJson, function () {
            effectsSending = false;
        }, function(){
            alert("Connection lost");
            effectsSending = false;
    });
    //alert(sendAPjson["name"] + " " + sendAPjson["password"]);
    }
}


document.getElementById("LED-count").onchange = function(){
    sendSettings();
}

function sendSettings() {
    var countInput = document.getElementById("LED-count");
    var sendJson = { "led-count": parseInt(countInput.value)};
    console.log(sendJson);
    makePOSTRequest("/settings", sendJson, function () {}, function(){
        alert("Connection lost");
    });
}

function makeGETRequest(path, func, badFunc = function () { ; }, loadFunc = function () { }) {
    var xmlHttp = CreateRequest();
    xmlHttp.open('GET', path, true);
    xmlHttp.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                func(this);
            } else {
                console.log("Not 200, ", this.status);
                badFunc(this, path, func);
            }
        } else {
            console.log("not loaded");
            loadFunc(this, path, func);
        }
    };
    xmlHttp.send(null);
}

function makePOSTRequest(path, json, func = function () { }, badFunc = function () { }, loadFunc = function () { }) {
    var xmlHttp = CreateRequest();
    xmlHttp.open('POST', path, true); // Открываем асинхронное соединение
    xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded;charset=UTF-8'); // Отправляем кодировку
    xmlHttp.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                console.log("POST sended!");
                func(this);
            } else {
                console.log("Not 200, ", this.status);
                badFunc(this);
            }
        } else {
            console.log("send not loaded");
            loadFunc(this);
        }
    };
    xmlHttp.send("data=" + JSON.stringify(json)); // Отправляем POST-запрос
}

function waitGET(request, path, func) {
    if (request.status == 202 && request.readyState == 4) {
        repeatRequest = setInterval(function () {
            if (!existRequest) {
                existRequest = true;
                console.log("try to get");
                makeGETRequest(path, function (secondary_request) {
                    repeatRequest = clearInterval(repeatRequest);
                    func(secondary_request);
                }, function () { existRequest = false });
            }
        }, 1500);
    }
}

function copyToClipboard(str) {
    var area = document.createElement('textarea');

    document.body.appendChild(area);
    area.value = str;
    area.select();
    document.execCommand("copy");
    document.body.removeChild(area);
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