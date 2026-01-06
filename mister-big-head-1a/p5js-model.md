```` javascript
let stlModel;

function preload() {
  // Load the STL file from the GitHub link.
  stlModel = loadModel('https://raw.githubusercontent.com/jimf99/jimf99.github.io/main/mister-big-head-1a/low_polygon_mask_V2.stl', true);
}

function setup() {
  createCanvas(400, 400, WEBGL);
}

function draw() {
  background(200);
  lights(); // Enable lighting

  // Rotate the model for better visibility
  rotateX(frameCount * 0.005);
  rotateY(frameCount * 0.005);
  
  // Render the loaded STL model
  scale(1.0); // Adjust the scale if necessary
  model(stlModel);
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}

```

