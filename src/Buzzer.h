#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>  // garante que as funções do Arduino (tone, delay, etc) estejam disponíveis

// Protótipos
void playMario(int buzzerPin);
void alertSound(int buzzerPin);

#endif