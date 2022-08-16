window.onload = function () {
  getSelectList();
  loadPresets();
};

window.onunload = function () {
  // saving before leave
  // alert("Do you really want to close?");
  sendPresets();
  console.log("close");
  return "Are you shure?";
};

let timerId = setInterval(function() { sendPresets(); }, 1000);

document.getElementById("led-mode-select").onchange = function () {
  sendEffectsState(hueColor());
};

document.getElementById("speed-range").oninput = function () {
  sendEffectsState(hueColor());
};

document.getElementById("turn-switch").onchange = function () {
  sendEffectsState(hueColor());
};

var repeatRequest;
var existRequest;
var effectsSending = false;

import { hueColor, setNowPreset, nowPreset, colorsPresets } from "./wheel.js";

function getSelectList() {
  makeGETRequest(
    "../effects_set.json",
    decodeSetJSON,
    function (request, path, func) {
      waitGET(request, path, func);
    }
  );
}

function decodeSetJSON(handler) {
  // showForm();
  console.log("content-type: ", handler.getResponseHeader("Content-Type"));
  //   console.log(handler.responseText);
  var resp = JSON.parse(handler.responseText);
  console.log(resp["effects"]);
  var effects = resp["effects"];
  pasteInListOnPage(effects);
}

function pasteInListOnPage(list) {
  if (Object.keys(list).length > 0) {
    var selectList = document.getElementById("led-mode-select");
    // selectList.options[0].style.display = 'none';
    for (var key in list) {
      var option = document.createElement("option");
      console.log(list[key]);
      option.innerHTML = list[key][1];
      option.value = list[key][0];
      selectList.appendChild(option);
    }
    selectList.options[0].selected = true;
  }
}

function loadPresets() {
  makeGETRequest(
    "../user_set.json",
    decodePresetJSON,
    function (request, path, func) {
      waitGET(request, path, func);
    }
  );
}

function decodePresetJSON(handler) {
  console.log("content-type: ", handler.getResponseHeader("Content-Type"));
  var resp = JSON.parse(handler.responseText);

  var selectList = document.getElementById("led-mode-select");
  var speedSlider = document.getElementById("speed-range");
  var turnSw = document.getElementById("turn-switch");

  selectList.value = resp["effect"];
  speedSlider.value = resp["speed"];
  turnSw.checked = resp["turn"];

  var newColorPresets = [];

  for (let i = 1; i <= resp["number of presets"]; i++) {
    newColorPresets.push(resp["preset " + i]);
  }
  setNowPreset(resp["now preset"], newColorPresets);
}

function sendPresets() {
  var selectList = document.getElementById("led-mode-select");
  var speedSlider = document.getElementById("speed-range");
  var turnSw = document.getElementById("turn-switch");
  var effectID = turnSw.checked
    ? parseInt(selectList.options[selectList.selectedIndex].value)
    : 0;

  var sendJson = {
    "number of presets": 3,
    "preset 1": colorsPresets[0],
    "preset 2": colorsPresets[1],
    "preset 3": colorsPresets[2],
    "now preset": nowPreset,
    effect: parseInt(selectList.value),
    speed: speedSlider.value,
    turn: turnSw.checked,
  };
  //   console.log(sendJson);
  makePOSTRequest(
    "/update_user_set",
    sendJson,
    function () {
      effectsSending = false;
    },
    function () {
      alert("Connection lost");
      effectsSending = false;
    }
  );
}

export default function sendEffectsState(hueColor) {
  if (!effectsSending) {
    effectsSending = true;
    var selectList = document.getElementById("led-mode-select");
    var speedSlider = document.getElementById("speed-range");
    var turnSw = document.getElementById("turn-switch");
    var effectID = turnSw.checked
      ? parseInt(selectList.value)
      : 0;
    var sendJson = {
      effect: effectID,
      hsv: hueColor,
      speed: parseFloat(speedSlider.value),
    };
    console.log(sendJson);
    makePOSTRequest(
      "/selectedEffects",
      sendJson,
      function () {
        effectsSending = false;
      },
      function () {
        alert("Connection lost");
        effectsSending = false;
      }
    );
    //alert(sendAPjson["name"] + " " + sendAPjson["password"]);
  }
}

document.getElementById("LED-count").onchange = function () {
  sendSettings();
};

function sendSettings() {
  var countInput = document.getElementById("LED-count");
  var sendJson = { "led-count": parseInt(countInput.value) };
  console.log(sendJson);
  makePOSTRequest(
    "/settings",
    sendJson,
    function () {},
    function () {
      alert("Connection lost");
    }
  );
}

function makeGETRequest(
  path,
  func,
  badFunc = function () {},
  loadFunc = function () {}
) {
  var xmlHttp = CreateRequest();
  xmlHttp.open("GET", path, true);
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

function makePOSTRequest(
  path,
  json,
  func = function () {},
  badFunc = function () {},
  loadFunc = function () {}
) {
  var xmlHttp = CreateRequest();
  xmlHttp.open("POST", path, true); // Открываем асинхронное соединение
  xmlHttp.setRequestHeader(
    "Content-Type",
    "application/x-www-form-urlencoded;charset=UTF-8"
  ); // Отправляем кодировку
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
        makeGETRequest(
          path,
          function (secondary_request) {
            repeatRequest = clearInterval(repeatRequest);
            func(secondary_request);
          },
          function () {
            existRequest = false;
          }
        );
      }
    }, 1500);
  }
}

function copyToClipboard(str) {
  var area = document.createElement("textarea");

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
  } else if (window.ActiveXObject) {
    //Internet explorer
    try {
      Request = new ActiveXObject("Microsoft.XMLHTTP");
    } catch (CatchException) {
      Request = new ActiveXObject("Msxml2.XMLHTTP");
    }
  }

  if (!Request) {
    alert("Невозможно создать XMLHttpRequest");
  }

  return Request;
}
