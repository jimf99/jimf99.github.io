#include "pitches.h" // Standard Arduino note definitions

// Melody (notes as per the ABC, F major)
int melody[] = {
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, // F4 f4
  NOTE_E5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_F5, // Am e2 cd e2 f2
  NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, // Bb F4 d4
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, // F c8
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_BB4, // Bb D4 Bbm B4
  NOTE_A4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_B4, // F A2 FG A2 Gm B2
  NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4, // G2 EF C7 G2 F A2
  // Repeat or second ending
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_C5, NOTE_C5, // F8 or F6 c2
  // Next phrase
  NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, // F AcAc AcAc
  NOTE_BB4, NOTE_C5, NOTE_BB4, NOTE_C5, NOTE_BB4, NOTE_C5, NOTE_BB4, NOTE_C5, // Bb BcBc Bbm BcBc
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, // Dm d4 d4
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_C5, NOTE_C5, // d6 F c2
  NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, // AcAc AcAc
  NOTE_B4, NOTE_D5, NOTE_B4, NOTE_D5, NOTE_B4, NOTE_D5, NOTE_B4, NOTE_D5, // E7 =BdBd BdBd
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, // Am e4 e4
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_BB4, // Gm g4 Bb d4
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, // F4 f4
  NOTE_E5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_F5, // Am e2 cd e2 f2
  NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, // Bb F4 d4
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, // F c8
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_BB4, NOTE_BB4, NOTE_BB4, NOTE_BB4, // Bb D4 Bbm B4
  NOTE_A4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_B4, // F A2 FG A2 Gm B2
  NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4, // G2 EF C7 G2 F A2
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_C5, NOTE_C5, // F6 c2
  NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, // AcAc AcAc
  NOTE_BB4, NOTE_C5, NOTE_BB4, NOTE_C5, NOTE_BB4, NOTE_C5, 0, 0, // Bb BcBc Bbm z4 (rest)
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, // Gm B2 C c2 Gm d2 C e2 F f8
};

// Durations: 4 = quarter note, 8 = eighth note, etc. (based on L:1/8 and M:4/4)
// For simplicity, let's map 8 -> eighth, 4 -> quarter, 2 -> half, etc.
int noteDurations[] = {
  4,4,4,4,4,4,4,4, // F4 f4
  2,2,2,2,2,2,2,2, // Am e2 cd e2 f2
  4,4,4,4,4,4,4,4, // Bb F4 d4
  8,8,8,8,8,8,8,8, // F c8
  4,4,4,4,4,4,4,4, // Bb D4 Bbm B4
  2,2,2,2,2,2,2,2, // F A2 FG A2 Gm B2
  2,2,2,2,2,2,     // G2 EF C7 G2 F A2
  8,8,8,8,8,8,2,2, // F8 or F6 c2
  2,2,2,2,2,2,2,2, // F AcAc AcAc
  2,2,2,2,2,2,2,2, // Bb BcBc Bbm BcBc
  4,4,4,4,4,4,4,4, // Dm d4 d4
  2,2,2,2,2,2,2,2, // d6 F c2
  2,2,2,2,2,2,2,2, // AcAc AcAc
  2,2,2,2,2,2,2,2, // E7 =BdBd BdBd
  4,4,4,4,4,4,4,4, // Am e4 e4
  4,4,4,4,4,4,4,4, // Gm g4 Bb d4
  4,4,4,4,4,4,4,4, // F4 f4
  2,2,2,2,2,2,2,2, // Am e2 cd e2 f2
  4,4,4,4,4,4,4,4, // Bb F4 d4
  8,8,8,8,8,8,8,8, // F c8
  4,4,4,4,4,4,4,4, // Bb D4 Bbm B4
  2,2,2,2,2,2,2,2, // F A2 FG A2 Gm B2
  2,2,2,2,2,2,     // G2 EF C7 G2 F A2
  8,8,8,8,8,8,2,2, // F6 c2
  2,2,2,2,2,2,2,2, // AcAc AcAc
  2,2,2,2,2,2,2,2, // Bb BcBc Bbm z4 (rest)
  2,2,2,2,4,4,4,4, // Gm B2 C c2 Gm d2 C e2 F f8
};

const int notes = sizeof(melody) / sizeof(melody[0]);
const int buzzerPin = 8; // Change as needed

void setup() {
  for (int thisNote = 0; thisNote < notes; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    if (melody[thisNote] == 0) {
      // Rest
      noTone(buzzerPin);
    } else {
      tone(buzzerPin, melody[thisNote], noteDuration);
    }
    delay(noteDuration * 1.2); // Add a slight pause between notes
    noTone(buzzerPin);
  }
}

void loop() {
  // Nothing here
}