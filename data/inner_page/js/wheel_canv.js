var actual_color;

window.onload = function () {



color_picker = document.getElementById("color_picker");
color_id = document.getElementById("color_id");
color_picker.onmousedown = select_color;
color_picker.onmousemove = move_marker;
color_picker_add();

function color_picker_add(Mx = null, My = null) {
	color_picker_ = color_picker.getContext("2d"),
  center_x = (color_picker.width)/2,
  center_y = (color_picker.height)/2,
  sx = center_x,
  sy = center_y;
  var color = actual_color;
  if (Mx == null){
    Mx = center_x;
    My = center_y;
    color = 'black';
  }
	palette = new color_picker_element(center_x, center_y, sx, sy);
	palette.draw();
  palette.marker(Mx, My, color);
}

function select_color(e) {
  var x = e.pageX - color_picker.offsetLeft,
      y = e.pageY - color_picker.offsetTop,   
      pixel = color_picker.getContext("2d").getImageData(x, y, 1, 1).data,
	    pixelColor = "rgb(" + pixel[0] + ", " + pixel[1]+", "+ pixel[2]+ ")";
	color_id.style.backgroundColor = pixelColor;
  actual_color = pixelColor;
  color_picker_add(x, y)
}

function move_marker(e){
  var x = e.pageX - color_picker.offsetLeft,
      y = e.pageY - color_picker.offsetTop;
  color_picker_add(x, y)
}

function color_picker_element(center_x, center_y, sx, sy) {
	this.center_x = center_x;
	this.center_y = center_y;
	this.sx = sx;
	this.sy = sy;
	this.draw = function() {
		for(var i = 0; i < 360; i+=0.1)
		{
			var rad = (i-45) * (Math.PI) / 180;
			color_picker_.strokeStyle = "hsla("+i+", 100%, 50%, 1.0)";
			color_picker_.beginPath();
			color_picker_.moveTo(center_x, center_y);
			color_picker_.lineTo(center_x + sx * Math.cos(-rad), center_y + sy * Math.sin(-rad));
			color_picker_.stroke();	
		}
	}
  this.marker = function(x, y, color){
    color_picker_.beginPath();
    color_picker_.arc(x, y, 5, 0, 2 * Math.PI, false);
    color_picker_.fillStyle = color;
    color_picker_.fill();
    color_picker_.lineWidth = 1;
    color_picker_.strokeStyle = '#314268';
    color_picker_.stroke();
  }
}
}