const settings = {
  speed: 5,
  rotationSpeed: 0.05,
  liftSpeed: 2,
  arenaSize: 1200,
  forkliftSize: 60,
  wheelRadius: 15,
  lerpSpeed: 0.15,
  forkliftRadius: 42, 
  palletRadius: 40,
  buffer: 5, 
  targetZoneZ: -550,
  gridSlots: 6
};

let forklift;
let pallets = [];

function setup() {
  createCanvas(windowWidth, windowHeight, WEBGL);
  camera(600, -600, 600, 0, 0, 0, 0, 1, 0);
  
  forklift = {
    x: 0,
    z: 200,
    angle: 0,
    liftY: 0,
    w: settings.forkliftSize,
    h: settings.forkliftSize * 0.8,
    d: settings.forkliftSize * 1.5,
    carrying: null,
    isAligning: false,
    targetX: 0,
    targetZ: 0,
    targetAngle: 0,
    tempPallet: null
  };

  for (let i = 0; i < 12; i++) {
    let px, pz;
    let safe = false;
    let attempts = 0;
    while (!safe && attempts < 100) {
      attempts++;
      px = random(-500, 500);
      pz = random(-500, 500);
      let distToForklift = dist(px, pz, forklift.x, forklift.z);
      let inTargetZone = pz < settings.targetZoneZ + 150;
      if (distToForklift > 180 && !inTargetZone) {
        safe = true;
        for (let other of pallets) {
          if (dist(px, pz, other.x, other.z) < 120) {
            safe = false;
            break;
          }
        }
      }
    }
    pallets.push(new Pallet(px, pz));
  }
}

function draw() {
  background(20);
  orbitControl();
  ambientLight(100);
  pointLight(255, 255, 255, 0, -600, 0); 
  directionalLight(150, 150, 150, 0.5, 1, 0.5);

  drawArena();
  drawTargetGrid();
  handleInput();
  updateForkliftAlignment();
  
  for (let p of pallets) {
    p.update();
    p.display();
  }
  
  drawForklift();
}

function handleInput() {
  if (forklift.isAligning) return;

  let moveX = 0; 
  let moveZ = 0; 

  if (keyIsDown(87)) moveZ = settings.speed;
  if (keyIsDown(83)) moveZ = -settings.speed;
  if (keyIsDown(65)) moveX = settings.speed;
  if (keyIsDown(68)) moveX = -settings.speed;
  
  if (keyIsDown(81)) forklift.angle += settings.rotationSpeed;
  if (keyIsDown(69)) forklift.angle -= settings.rotationSpeed;
  
  if (keyIsDown(32)) forklift.liftY = min(forklift.liftY + settings.liftSpeed, 150);
  if (keyIsDown(16)) forklift.liftY = max(forklift.liftY - settings.liftSpeed, 0);

  let nextX = forklift.x + moveZ * sin(forklift.angle) + moveX * cos(forklift.angle);
  let nextZ = forklift.z + moveZ * cos(forklift.angle) - moveX * sin(forklift.angle);

  for (let p of pallets) {
    if (p.isCarried || p.y < -10) continue;
    let dNext = dist(nextX, nextZ, p.x, p.z);
    let minDist = settings.forkliftRadius + settings.palletRadius + settings.buffer;
    if (dNext < minDist) {
      let pushAngle = atan2(forklift.x - p.x, forklift.z - p.z);
      forklift.x += sin(pushAngle) * 1;
      forklift.z += cos(pushAngle) * 1;
      return;
    }
  }

  forklift.x = nextX;
  forklift.z = nextZ;
  let b = settings.arenaSize / 2 - 50;
  forklift.x = constrain(forklift.x, -b, b);
  forklift.z = constrain(forklift.z, -b, b);
}

function updateForkliftAlignment() {
  if (forklift.isAligning) {
    forklift.x = lerp(forklift.x, forklift.targetX, settings.lerpSpeed);
    forklift.z = lerp(forklift.z, forklift.targetZ, settings.lerpSpeed);
    
    let angleDiff = forklift.targetAngle - forklift.angle;
    angleDiff = ((angleDiff + PI) % TWO_PI) - PI;
    forklift.angle += angleDiff * settings.lerpSpeed;

    let d = dist(forklift.x, forklift.z, forklift.targetX, forklift.targetZ);
    if (d < 2 && abs(angleDiff) < 0.05) {
      forklift.isAligning = false;
      if (forklift.tempPallet) {
        forklift.carrying = forklift.tempPallet;
        forklift.carrying.isCarried = true;
        forklift.tempPallet = null;
      }
    }
  }
}

function keyPressed() {
  if (key === 'f' || key === 'F') {
    if (forklift.carrying) {
      let p = forklift.carrying;
      forklift.carrying = null;
      p.isCarried = false;
      
      // RESTORED STACKING LOGIC
      let stacked = false;
      for (let other of pallets) {
        // Check if we are dropping over another pallet
        if (other !== p && dist(p.x, p.z, other.x, other.z) < 60) {
          // If the dropped pallet is above the floor pallet, snap to top
          if (p.y < other.y - 30) {
            p.y = other.y - 85; 
            stacked = true;
            break;
          }
        }
      }
      if (!stacked) p.y = 0; 
      
      forklift.isAligning = false; 
    } else {
      for (let p of pallets) {
        let d = dist(forklift.x, forklift.z, p.x, p.z);
        if (d < 150 && !forklift.isAligning) {
          forklift.isAligning = true;
          forklift.tempPallet = p;
          
          let angleDiff = ((p.angle - forklift.angle + PI) % TWO_PI) - PI;
          forklift.targetAngle = p.angle;
          
          let distRequired = forklift.d / 2 + p.d / 2 + 2; 
          forklift.targetX = p.x - sin(p.angle) * distRequired;
          forklift.targetZ = p.z - cos(p.angle) * distRequired;
          break;
        }
      }
    }
  }
}

function drawArena() {
  push();
  translate(0, 0.1, 0); rotateX(HALF_PI);
  fill(40); stroke(70);
  const tSize = 100;
  for(let i = -6; i < 6; i++) {
    for(let j = -6; j < 6; j++) rect(i * tSize, j * tSize, tSize, tSize);
  }
  pop();
}

function drawTargetGrid() {
  push();
  translate(0, 0.5, settings.targetZoneZ);
  rotateX(HALF_PI);
  noFill();
  strokeWeight(4);
  let gridW = 120;
  let totalW = gridW * settings.gridSlots;
  for (let i = 0; i < settings.gridSlots; i++) {
    let xPos = -totalW/2 + i * gridW;
    stroke(0, 255, 255, 150);
    rect(xPos, 0, gridW, 120);
    push();
    noStroke(); fill(0, 255, 255, 40);
    rect(xPos + 5, 5, gridW - 10, 110);
    pop();
  }
  pop();
}

function drawForklift() {
  push();
  translate(forklift.x, -settings.wheelRadius - forklift.h / 2, forklift.z);
  rotateY(forklift.angle);
  fill(255, 180, 0); stroke(0);
  box(forklift.w, forklift.h, forklift.d);
  fill(30); noStroke();
  for (let x of [-1, 1]) {
    for (let z of [-1, 1]) {
      push(); translate(x * forklift.w * 0.5, forklift.h * 0.5, z * forklift.d * 0.35);
      rotateZ(HALF_PI); cylinder(settings.wheelRadius, 12); pop();
    }
  }
  push();
  translate(0, -30, forklift.d / 2 + 5);
  fill(40); box(forklift.w * 0.7, 180, 8);
  translate(0, 80 - forklift.liftY, 12);
  fill(100); box(forklift.w * 0.9, 10, 10);
  for(let x of [-18, 18]) {
    push(); translate(x, 5, 25); fill(80); box(8, 5, 65); pop();
  }
  pop(); 
  pop();
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}