window.onload = function () {
  getSettings();
  setSettings();
};


function getSettings(){
    makeGETRequest(
        "/settings.json",
        decodePresetJSON,
        function (request, path, func) {
          waitGET(request, path, func);
        }
      );
}

function decodePresetJSON(handler) {
    console.log("content-type: ", handler.getResponseHeader("Content-Type"));
    var resp = JSON.parse(handler.responseText);
    console.log(resp);
    var ledСount = document.getElementById("led-count");

    ledСount.value = resp["led-count"];
  }


function sendSettings() {
  var settingsBox = document.getElementsByClassName("setiings-box");
  var sendJson = {};
  for (var i = 0; i < settingsBox.length; i++) {
    sendJson[settingsBox[i].getAttribute("param_name")] =  parseInt(settingsBox[i].value);
  }
  console.log(JSON.stringify(sendJson));
  makePOSTRequest(
    "/update_settings",
    sendJson,
    function () {showUploadState(true);},
    function () {showUploadState(false);},
    function () {
        changeTextBox(
        document.querySelector("#apply-btn [show]"),
        document.getElementById("apply-btn-process")
      );
    }
  );
}

function setSettings() {
  // Без защиты от потери соединения! #TODO
  document
    .getElementById("apply-btn")
    .addEventListener("click", sendSettings);
}

function showUploadState(success) {
  changeTextBox(
    document.querySelector("#apply-btn [show]"),
    document.getElementById("apply-btn-" + (success ? "success" : "fail"))
  );

  if (success) {    //TODO make active if change params after sending
    document
      .getElementById("apply-btn")
      // .removeEventListener("click", sendUpdateFiles);
  }
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
