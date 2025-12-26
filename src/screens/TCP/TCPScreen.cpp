#include "TCPScreen.h"
#include "services/NetworkService.h"
#include "services/TCPService.h"
#include <Arduino.h>

void TCPScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
    nextScreenPtr = menuAppScreen;
}

void TCPScreen::update() {
    TCPService::update();

    if (TCPService::isConnected()) {
        String msg = TCPService::getLastReceived();

        if (msg.length() > 0) {
            display->resetAutoOff();
            if (contador < numMaxVisible) {
                tcpCmds[contador++] = msg;
            } else {
                for (int i = 0; i < numMaxVisible - 1; i++)
                    tcpCmds[i] = tcpCmds[i + 1];
                tcpCmds[numMaxVisible - 1] = msg;
            }
        }
    }
}

void TCPScreen::draw() {
    display->clear();

    if (!NetworkService::isConnected()) {
        display->printCentered("Tela TCP", 10);
        display->printCentered("Sem WiFi disponivel!", 30);
        display->printCentered("Use [tela WiFi]", 45);
        display->printCentered("para conectar", 55);
    }
    else if (!TCPService::isServerRunning()) {
        display->printCentered("Tela TCP", 10);
        display->printCentered("[Iniciar TCP]", 38);
    }
    else if (TCPService::isServerRunning() && !TCPService::isConnected()) {
        display->drawText(0, 8, "TCP Ativo");
        display->drawText(0, 20, ("IP: " + WiFi.localIP().toString()).c_str());
        display->drawText(0, 32, ("Port: " + String(TCPService::port())).c_str());
        display->printCentered("Aguardando conexao...", 48);
    }
    else 
        for (int i = 0; i < numMaxVisible; i++) {
            display->drawText(0, 10 + (i * 12), tcpCmds[i].c_str());
        }

    display->display();
}

void TCPScreen::end() {
    display->clear();
    display->display();
}

void TCPScreen::onUpPressed() {
    nextTriggered = true;
}

void TCPScreen::onDownPressed() {
    nextTriggered = true;
}

void TCPScreen::onSelectPressed() {
    if (NetworkService::isConnected()) {
        if (!TCPService::isConnected() && !TCPService::isServerRunning()) {
            TCPService::begin();
        } else if (TCPService::isConnected() || TCPService::isServerRunning()) {
            TCPService::disconnect();
            display->clear();
            display->printCentered("Conexao TCP fechada!", 32);
            display->display();
            delay(1000);
        }
    } else {
        nextScreenPtr = wifiScreen;
        nextTriggered = true;
    }
}
