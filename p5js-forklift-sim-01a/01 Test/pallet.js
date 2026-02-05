// Pallet class extracted from sketch.js - global (expects global `forklift` and `settings`)
class Pallet {
  constructor(x, z) {
    this.x = x;
    this.z = z;
    this.y = 0; 
    this.angle = 0;
    this.w = 85;
    this.h = 18;
    this.d = 80;
    this.isCarried = false;
    this.hasBox = true;
    this.isHome = false;
  }

  update() {
    if (this.isCarried) {
      let forkLength = forklift.d / 2 + this.d / 2;
      this.x = forklift.x + sin(forklift.angle) * forkLength;
      this.z = forklift.z + cos(forklift.angle) * forkLength;
      this.y = -forklift.liftY - 5; 
      this.angle = forklift.angle;
      this.isHome = false;
    } else {
      this.isHome = (this.z < settings.targetZoneZ + 60 && this.y >= 0);
    }
  }

  display() {
    push();
    translate(this.x, this.y - this.h / 2, this.z);
    rotateY(this.angle);
    if (this.isHome) {
      fill(0, 255, 255);
      stroke(0, 100, 100);
    } else {
      fill(130, 100, 70);
      stroke(60, 40, 20);
    }
    box(this.w, 5, this.d); 
    push();
    translate(0, 8, 0);
    box(this.w, 10, 15);
    translate(0, 0, 30); box(this.w, 10, 10);
    translate(0, 0, -60); box(this.w, 10, 10);
    pop();
    if (this.hasBox) {
      translate(0, -35, 0);
      this.isHome ? fill(100, 200, 200) : fill(190, 150, 110);
      stroke(100, 70, 40);
      box(65, 60, 65);
    }
    pop();
  }
}