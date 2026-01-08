
function preload() {
  // Load the STL file from the GitHub link.
  stlModel = loadModel('https://raw.githubusercontent.com/jimf99/jimf99.github.io/main/mister-big-head-1a/low_polygon_mask_V2.stl', true);
}

function setup() {
  createCanvas(400, 400, WEBGL);
}

let pi=3.141592650;
let pi180=180/(2*pi);
var ax=1;
var ay=3;
var az=0;

function draw() {
  background(color(0,250,16));
  lights(); // Enable lighting

  // Rotate the model for better visibility
  // rotateX(frameCount * 0.005);
  // rotateY(frameCount * 0.005);
  // Render the loaded STL model
  rotateX(3.14159650);
  rotateY(3.14159650);
  rotateZ(3.14159650);
  
  scale(1.2); // Adjust the scale if necessary
  model(stlModel);
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}

