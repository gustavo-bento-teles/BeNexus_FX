#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

enum class Tela {
    CLOCK,
    DATE,
    WIFI,
    UPDATE_CLOCK,
    LANTERNA,
    INFORMACOES,
    
    COUNT
};

Tela nextTela(Tela t);
Tela prevTela(Tela t);

#endif