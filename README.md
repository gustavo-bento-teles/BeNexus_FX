# 📱 BeNexus_FX - Documentação Técnica

## 🎯 Visão Geral

Firmware para ESP8266 (ESP-12E) que busca passar a ideia de um smartwatch com múltiplas funcionalidades, incluindo relógio, calendário, lanterna, WiFi e sincronização NTP. O projeto utiliza uma arquitetura modular baseada em gerenciadores e telas (screens).

---

## 🏗️ Arquitetura do Sistema

### Padrão Arquitetural

O projeto segue uma arquitetura em camadas com separação clara de responsabilidades:

```
┌─────────────────────────────────────┐
│         main.cpp (Setup)            │
├─────────────────────────────────────┤
│    ScreenManager (Orquestrador)     │
├──────────────┬──────────────────────┤
│ InputManager │   OutputManager      │
├──────────────┴──────────────────────┤
│         Screens (Views)             │
├─────────────────────────────────────┤
│    Drivers (Hardware Abstraction)   │
├─────────────────────────────────────┤
│      Services (Network, etc)        │
└─────────────────────────────────────┘
```

---

## 📦 Componentes Principais

### 1. **Core (Núcleo)**

#### ScreenManager
**Propósito:** Gerencia o ciclo de vida das telas e navegação entre elas.

**Responsabilidades:**
- Controla qual tela está ativa
- Gerencia transições entre telas
- Propaga eventos de input para a tela atual
- Coordena o ciclo de atualização (update → draw)
- Gerencia auto-desligamento do display

**Métodos principais:**
```cpp
void begin()                     // Inicializa a tela inicial
void update()                    // Atualiza lógica (inclui NetworkService)
void draw()                      // Renderiza a tela atual
void handleInput(ButtonEvent ev) // Processa eventos de botões
```

**Fluxo de transição:**
```
currentScreen → nextScreen()
   ↓ (se mudou)
end() → begin() → resetAutoOff()
```

---

### 2. **Input (Entrada)**

#### InputManager
**Propósito:** Abstrai a leitura dos botões e converte em eventos.

**Características:**
- Detecta pressões curtas e longas (hold)
- Debouncing automático (25ms via Bounce2)
- Evita eventos duplicados após hold

**Eventos gerados:**
- `ButtonEvent::Up/Down/Select` - Pressão curta
- `ButtonEvent::UpHold/DownHold/SelectHold` - Pressão longa (500ms)
- `ButtonEvent::None` - Nenhum evento

#### Buttons
**Propósito:** Driver de baixo nível para os botões físicos.

**Configuração:**
- PIN_UP_BTN: GPIO 12
- PIN_DOWN_BTN: GPIO 13
- PIN_SELECT_BTN: GPIO 14
- Modo: INPUT_PULLUP (lógica invertida)

**Detecção de Hold:**
```cpp
// Previne evento duplicado após hold
if (btnUp.rose() && !upHoldFired)
    return ButtonEvent::Up;
```

---

### 3. **Output (Saída)**

#### OutputManager
**Propósito:** Gerencia dispositivos de saída (LEDs, buzzer, etc).

**Funcionalidades atuais:**
- Controle da lanterna (LED)
- Interface unificada para expansão futura

#### Lantern
**Propósito:** Controla um LED/relé conectado ao GPIO 16.

**Características:**
- Lógica invertida (LOW = ligado)
- Toggle simples
- Estado persistente

---

### 4. **Drivers (Hardware)**

#### Display
**Hardware:** OLED SH1106 128x64 (I2C)  
**Biblioteca:** U8g2

**Funcionalidades:**
- Renderização de texto e bitmaps
- Controle de contraste
- Auto-desligamento programável (5 segundos padrão)
- Centralização automática de texto

**Pinos I2C:**
- SDA: GPIO 5
- SCL: GPIO 4

**Ciclo de uso:**
```cpp
display.clear()
display.drawText(x, y, "texto")
display.display() // Envia buffer para tela
```

#### RTCManager
**Hardware:** DS3231 (I2C)

**Funcionalidades:**
- Leitura de hora/data
- Formatação automática (HH:MM:SS, DD/MM/YYYY)
- Sincronização via NTP
- Suporte a dia da semana

**Métodos úteis:**
```cpp
String getTimeString()  // "14:30:45"
String getDateString()  // "25/12/2024"
void setDateTime(tm*)   // Sincroniza com NTP
```

---

### 5. **Services (Serviços)**

#### NetworkService
**Propósito:** Gerencia conexão WiFi de forma global.

**Características:**
- Singleton estático
- Timeout de conexão: 10 segundos
- Auto-desconexão se perder conexão
- Modo WIFI_OFF quando inativo (economia de energia)

**Estados:**
```cpp
WIFI_OFF → connect() → WIFI_STA → WL_CONNECTED
                              ↓
                       (timeout/erro)
                              ↓
                         WIFI_OFF
```

---

## 🖼️ Sistema de Telas

### Hierarquia de Navegação

```
BootScreen (inicial)
    ↓
MenuAppScreen (hub)
    ├→ ClockScreen
    ├→ CalendarScreen
    ├→ WiFiScreen
    ├→ NTPScreen
    └→ FlashlightScreen
```

### Interface Screen (Base)

Todas as telas herdam de uma interface comum:

```cpp
class Screen {
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleInput(ButtonEvent ev) = 0;
    virtual Screen* nextScreen() = 0;
    virtual void end() = 0;
};
```

### Telas Implementadas

1. **BootScreen**: Animação inicial
2. **MenuAppScreen**: Menu principal com seleção de apps
3. **ClockScreen**: Exibe hora e data atual do RTC
4. **CalendarScreen**: Exibe data completa
5. **WiFiScreen**: Gerencia conexão WiFi
6. **NTPScreen**: Sincroniza hora via internet
7. **FlashlightScreen**: Liga/desliga lanterna

---

## 🔄 Fluxo de Execução

### Setup (Inicialização)
```cpp
1. display.begin()
2. menuAppScreen.setScreens(...)  // Configura navegação
3. screenManager.begin()          // Inicia BootScreen
4. inputManager.begin()           // Configura botões
5. outputManager.begin()          // Inicializa lanterna
```

### Loop (Ciclo Principal)
```cpp
void loop {
    screenManager.update()        // Lógica + transições
    inputManager.update()         // Lê botões
    outputManager.update()        // Atualiza saídas
    
    ButtonEvent ev = inputManager.getEvent()
    screenManager.handleInput(ev) // Envia evento
    
    screenManager.draw()          // Renderiza
}
```

---

## ⚙️ Dependências

```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino

lib_deps:
  - U8g2            # Display OLED
  - NTPClient       # Sincronização horário
  - Bounce2         # Debouncing botões
  - DS3231          # RTC (Real Time Clock)
```

---

## 🎛️ Configuração de Hardware

### Pinout

| Componente | Pino GPIO | Função |
|-----------|-----------|---------|
| Botão UP | 12 | INPUT_PULLUP |
| Botão DOWN | 13 | INPUT_PULLUP |
| Botão SELECT | 14 | INPUT_PULLUP |
| Lanterna | 16 | OUTPUT (invertido) |
| OLED SDA | 5 | I2C Data |
| OLED SCL | 4 | I2C Clock |
| RTC | (I2C compartilhado) | - |

### Esquema de Conexão

```
ESP8266 (ESP-12E)
├─ GPIO 12 ────┬─── [Botão UP] ──── GND
├─ GPIO 13 ────┬─── [Botão DOWN] ── GND
├─ GPIO 14 ────┬─── [Botão SELECT]─ GND
├─ GPIO 16 ────── [LED/Relé] ───── GND
├─ GPIO 5 (SDA) ┐
└─ GPIO 4 (SCL) ┴─── [OLED + RTC via I2C]
```

---

## 🧠 Conceitos Importantes

### 1. **Event-Driven Input**
Em vez de polling direto, o sistema usa eventos:
- Evita acoplamento entre botões e lógica
- Facilita adição de novas fontes de input
- Permite tratamento assíncrono

### 2. **Screen Transitions**
Transições são baseadas em retorno de ponteiros:
```cpp
Screen* nextScreen() override {
    // Avalia o gatilho e troca a tela com base nisso
    return nextTriggered ? nextScreenPtr : this;
}
```

### 3. **Auto-Off Display**
Economia de energia através de timer:
- Resetado a cada interação
- Desliga display após timeout
- Primeiro input após desligado apenas acorda

### 4. **Hold Prevention**
Evita eventos duplicados:
```cpp
// Exemplo baseado em como funciona no projeto real

// Marca que hold foi disparado
bool Buttons::upHeld(uint16_t holdMs) {
    if (!upHoldFired &&
        btnUp.read() == LOW &&
        millis() - upPressedAt >= holdMs) {

        upHoldFired = true;
        return ButtonEvent::UpHold;
    }
}

// Ignora release se hold já foi disparado
bool Buttons::upPressed() {
    if (btnUp.rose() && !upHoldFired) return ButtonEvent::Up;
}
```

---

## 📝 Convenções de Código

- **Naming**: PascalCase para classes, camelCase para variáveis
- **Arquivos**: Um par .h/.cpp por classe
- **Organização**: Pasta por categoria (drivers, screens, core, etc)
- **Modularidade**: Cada componente deve ter responsabilidade única

---

## 📚 Referências

- [U8g2 Documentation](https://github.com/olikraus/u8g2)
- [DS3231 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf)
- [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
- [Bounce2 Library](https://github.com/thomasfredericks/Bounce2)

---

## 📖 Documentação Adicional

Quer contribuir ou estender o projeto? Confira nossos guias:

- **[🛠️ Guia de Desenvolvimento](docs/DEVELOPMENT_GUIDE.md)** - Aprenda a adicionar novos drivers, criar telas personalizadas e implementar animações

**Contribuindo:**
1. Fork o projeto
2. Crie sua feature branch (`git checkout -b feature/MinhaFeature`)
3. Commit suas mudanças (`git commit -m 'Add: MinhaFeature'`)
4. Push para a branch (`git push origin feature/MinhaFeature`)
5. Abra um Pull Request

---

**Versão:** 2.3.2  
**Plataforma:** ESP8266 (ESP-12E)  
**Framework:** Arduino