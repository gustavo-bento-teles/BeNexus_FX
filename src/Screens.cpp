#include "Screens.h"

Tela nextTela(Tela t) {
    int valor = static_cast<int>(t);
    valor = (valor + 1) % static_cast<int>(Tela::COUNT);
    return static_cast<Tela>(valor);
}

Tela prevTela(Tela t) {
    int valor = static_cast<int>(t);
    valor = (valor - 1 + static_cast<int>(Tela::COUNT)) % static_cast<int>(Tela::COUNT);
    return static_cast<Tela>(valor);
}