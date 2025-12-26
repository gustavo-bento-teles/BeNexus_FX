#include "TCPService.h"

void TCPService::begin() {
    serverTCP.begin();
    serverTCP.setNoDelay(true);
    servidorLigado = true;
    lastReceived = "Servidor iniciado";
    newMsg = true;
}

void TCPService::update() {
    if (!servidorLigado) return;

    // Aceitar novo cliente se não houver cliente ativo ou antigo desconectou
    if (!activeClient || !activeClient.connected()) {
        WiFiClient newClient = serverTCP.available();
        if (newClient) {
            if (activeClient) activeClient.stop();
            activeClient = newClient;
            activeClient.setTimeout(8000);
            lastReceived = "Cliente conectado!";
            newMsg = true;

            activeClient.println("Conexao bem sucedida!");
            activeClient.print("> ");
            activeClient.flush();
        }
    }

    // Processar dados do cliente ativo
    if (activeClient && activeClient.connected()) {
        if (activeClient.available()) {
            String line = activeClient.readStringUntil('\n');
            line.trim();
            if (line.length() > 0) {
                lastReceived = line;
                newMsg = true;
                activeClient.println("Recebido: " + line);
                activeClient.print("> ");
                activeClient.flush();
            }
        }
    }
    else if (activeClient && !activeClient.connected()) {
        activeClient.stop();
        activeClient = WiFiClient();
        lastReceived = "Cliente desconectado";
        newMsg = true;
    }
}

void TCPService::disconnect() {
    if (activeClient && activeClient.connected()) {
        activeClient.println("Desconectando...");
        activeClient.flush();
        delay(100);
        activeClient.stop();
    }
    activeClient = WiFiClient();
    lastReceived = "Desconectado";
    newMsg = true;
    servidorLigado = false;
}

bool TCPService::isConnected() {
    return activeClient && activeClient.connected();
}

String TCPService::getLastReceived() {
    if (!newMsg)
        return "";

    newMsg = false;
    String tmp = lastReceived;
    lastReceived = "";
    tmp = normalizeText(tmp);
    return tmp;
}

String TCPService::normalizeText(const String& input) {
    String out;
    out.reserve(input.length());

    for (size_t i = 0; i < input.length(); i++) {
        uint8_t c = input[i];

        if (c < 128) {
            out += char(c);
        } else {
            // UTF-8 two-byte chars (á, é, í, ó, ú, ç, etc)
            if (i + 1 < input.length()) {
                uint8_t c2 = input[i + 1];

                // A
                if (c == 0xC3 && (c2 == 0xA1 || c2 == 0xA0 || c2 == 0xA2 || c2 == 0xA3 || c2 == 0xA4))
                    out += 'a';
                // E
                else if (c == 0xC3 && (c2 == 0xA9 || c2 == 0xAA || c2 == 0xA8 || c2 == 0xAB))
                    out += 'e';
                // I
                else if (c == 0xC3 && (c2 == 0xAD || c2 == 0xAC || c2 == 0xAF || c2 == 0xAE))
                    out += 'i';
                // O
                else if (c == 0xC3 && (c2 == 0xB3 || c2 == 0xB4 || c2 == 0xB2 || c2 == 0xB5 || c2 == 0xB6))
                    out += 'o';
                // U
                else if (c == 0xC3 && (c2 == 0xBA || c2 == 0xB9 || c2 == 0xBB || c2 == 0xBC))
                    out += 'u';
                // C
                else if (c == 0xC3 && c2 == 0xA7)
                    out += 'c';
                // desconhecido → ignora
                else
                    out += '?';

                i++; // pula o segundo byte
            }
        }
    }

    return out;
}
