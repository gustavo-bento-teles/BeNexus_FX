// ======================
// Definições de notas
// ======================
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047

#include "Buzzer.h"

// Notas
int melodyMario[] = {
  NOTE_E5, NOTE_E5, 0, NOTE_E5, 
  0, NOTE_C5, NOTE_E5, 0,
  NOTE_G5, 0, 0,  0,
  NOTE_G4, 0, 0, 0,
};

int noteDurationsMario[] = {
  150, 150, 150, 150,
  150, 150, 150, 150,
  150, 150, 150, 150,
  150, 150, 150, 150,
};

int melodySizeMario = sizeof(melodyMario) / sizeof(int);

void playMario(int buzzerPin) {
  for (int i = 0; i < melodySizeMario; i++) {
    int noteDuration = noteDurationsMario[i];
    if (melodyMario[i] != 0) {
      tone(buzzerPin, melodyMario[i], noteDuration);
    }
    delay(noteDuration * 1.3);
    noTone(buzzerPin);
  }
}

void alertSound(int buzzerPin) {
  tone(buzzerPin, NOTE_C5, 150);
  delay(150);
  tone(buzzerPin, NOTE_C6, 150);
  delay(150);
  noTone(buzzerPin);
}