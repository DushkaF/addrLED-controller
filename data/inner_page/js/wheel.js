window.onload = function () {
  
}


function initColorRange(){
  const brightness = document.getElementById("brightness-range");
  brightness.oninput = function () {
    changeColorsBoxes();
  }
}

function change_range_color(){
  var e = document.getElementById("brightness-range");
  var rangeVal = e.value;
  colorsPresets[nowPreset][2] = rangeVal;

  e.style.setProperty('--range-color', hsv2hex(colorsPresets[nowPreset]));
}


const colors = [
  { r: 0xe4, g: 0x3f, b: 0x00 },
  { r: 0xfa, g: 0xe4, b: 0x10 },
  { r: 0x55, g: 0xcc, b: 0x3b },
  { r: 0x09, g: 0xad, b: 0xff },
  { r: 0x6b, g: 0x0e, b: 0xfd },
  { r: 0xe7, g: 0x0d, b: 0x86 },
  { r: 0xe4, g: 0x3f, b: 0x00 }
];


document.addEventListener('DOMContentLoaded', function () {
  var localColor;

  initPresetColor();
  initColorRange();

  document.getElementById('color-wheel').addEventListener('touchmove', getColorFromWheel);
  document.getElementById('color-wheel').addEventListener('mousemove', getColorFromWheel);
  document.getElementById('color-wheel').addEventListener('click', merkerHandler);
  document.getElementById('color-wheel').addEventListener('touchmove', merkerHandler);
  // document.getElementById('color-wheel').addEventListener('pointermove', merkerHandler);

  function merkerHandler() {
    document.getElementById('color-marker').style.background = hsv2hex(localColor);
    colorsPresets[nowPreset] = localColor.slice(0);
    changeColorsBoxes();
  }

  function getColorFromWheel(e) {
    var wheel = document.getElementById('color-wheel');
    var rect = wheel.getBoundingClientRect();
    //Compute cartesian coordinates as if the circle radius was 1
    if(e.type == 'touchmove'){
      e = e.touches[0];
    }
    var x = 2 * (e.clientX - rect.left) / (rect.right - rect.left) - 1;
    var y = 1 - 2 * (e.clientY - rect.top) / (rect.bottom - rect.top);
    //Compute the angle in degrees with 0 at the top and turning clockwise as expected by css conic gradient
    var a = ((Math.PI / 2 - Math.atan2(y, x)) / Math.PI * 180);
    if (a < 0) a += 360;
    //Map the angle between 0 and number of colors in the gradient minus one
    a = a / 360 * (colors.length - 1);  //minus one because the last item is at 360° which is the same as 0°
    //Compute the colors to interpolate
    var a0 = Math.floor(a) % colors.length;
    var a1 = (a0 + 1) % colors.length;
    var c0 = colors[a0];
    var c1 = colors[a1];
    //Compute the weights and interpolate colors
    var a1w = a - Math.floor(a);
    var a0w = 1 - a1w;
    var color = {
      r: c0.r * a0w + c1.r * a1w,
      g: c0.g * a0w + c1.g * a1w,
      b: c0.b * a0w + c1.b * a1w
    };
    //Compute the radius
    var r = Math.sqrt(x * x + y * y);
    if (r > 1) r = 1;
    //Compute the white weight, interpolate, and round to integer
    var cw = r < 0.8 ? (r / 0.8) : 1;
    var ww = 1 - cw;
    color.r = Math.round(color.r * cw + 255 * ww);
    color.g = Math.round(color.g * cw + 255 * ww);
    color.b = Math.round(color.b * cw + 255 * ww);
    //Compute the hex color code and apply it
    localColor = rgb2hsv(color.r, color.g, color.b);

    // Set marker
    let marker = document.getElementById('color-marker');
    let markerX = e.clientX - rect.left;
    let markerY = e.clientY - rect.top;
    // console.log(markerX, markerY);
    if (Math.pow((markerX - wheel.offsetWidth / 2), 2) + Math.pow((markerY - wheel.offsetHeight / 2), 2) <= Math.pow((wheel.offsetHeight / 2), 2)) {
      marker.style.top = markerY - marker.offsetWidth / 2 + "px";
      marker.style.left = markerX - marker.offsetHeight / 2 + "px";
    }
    // console.log(e.clientY, e.clientX);
  }

});


var nowPreset = 0;
var colorsPresets = [];


function changeColorsBoxes() {
  change_range_color();
  changeColorPresets()
}


function changeColorPresets() {
  var presets = document.getElementsByClassName("inner-color-item");
  presets[nowPreset].style.background = hsv2hex(colorsPresets[nowPreset]);
}


function initPresetColor(){
  var presets = document.getElementsByClassName("inner-color-item");
  for(let i = 0;  i < presets.length; i++){
    colorsPresets.push([0, 0, 255]);              // todo remove when server on
    presets[i].style.background = hsv2hex(colorsPresets[i]);
    presets[i].addEventListener("click", function(){
      var number = parseInt(String(this.id).split("-")[1]);
      let lastItem = document.getElementById("ci-" + nowPreset);
      if (lastItem.classList.contains("selected-color-item")){
        lastItem.classList.remove("selected-color-item");
      }
      nowPreset = number;
      this.classList.add("selected-color-item");
      change_range_color();
    });
  }
}




function componentToHex(c) {
  var hex = c.toString(16);
  return hex.length == 1 ? "0" + hex : hex;
}

function rgb2Hex(r, g, b) {
  return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

function rgb2hsv(r, g, b) {
  let v=Math.max(r,g,b), c=v-Math.min(r,g,b);
  let h= c && ((v==r) ? (g-b)/c : ((v==g) ? 2+(b-r)/c : 4+(r-g)/c)); 
  return [60*(h<0?h+6:h), v&&c/v, v];
}

function hsv2rgb(hsv) 
{
  let h = hsv[0],
  s =  hsv[1],
  v =  hsv[2];
  let f= (n,k=(n+h/60)%6) => v - v*s*Math.max( Math.min(k,4-k,1), 0);     
  return [Math.round(f(5)),Math.round(f(3)), Math.round(f(1))];       
}   

function hsv2hex(hsv){
  let rgb = hsv2rgb(hsv);
  let r = rgb[0],
  g = rgb[1],
  b = rgb[2];
  return rgb2Hex(r, g, b);
}