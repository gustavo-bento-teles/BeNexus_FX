# 📱 BeNexus_FX — Documentação Técnica

> Firmware para ESP8266 (ESP-12E) que simula um smartwatch minimalista com relógio, calendário, lanterna, WiFi e sincronização NTP.

**Versão:** 2.4.0-stable  
**Plataforma:** ESP8266 (ESP-12E)  
**Framework:** Arduino (via PlatformIO)

---

## 🎯 Visão Geral

O BeNexus_FX é um firmware modular para ESP8266, projetado em camadas com separação clara de responsabilidades. A interação com o usuário acontece por meio de três botões físicos e um display OLED SH1106 128×64, com navegação baseada em eventos e troca de telas via ponteiros.

---

## 🏗️ Arquitetura do Sistema

```
┌─────────────────────────────────────┐
│         main.cpp (Ponto de entrada) │
├─────────────────────────────────────┤
│    ScreenManager (Orquestrador)     │
├──────────────┬──────────────────────┤
│ InputManager │   OutputManager      │
├──────────────┴──────────────────────┤
│         Screens (Views)             │
│  Screen (base) → telas concretas    │
├─────────────────────────────────────┤
│    Drivers (Hardware Abstraction)   │
│  Display · RTC · Buttons · Lantern  │
├─────────────────────────────────────┤
│      Services (Network, etc)        │
│        NetworkService (static)      │
└─────────────────────────────────────┘
```

---

## 📦 Componentes Principais

### 1. Core — ScreenManager

**Arquivo:** `src/core/ScreenManager.{h,cpp}`

Orquestrador central do firmware. Gerencia o ciclo de vida das telas, propaga eventos de input e coordena o auto-desligamento do display.

**Construtor:**
```cpp
ScreenManager(Screen* initialScreen, Display* display, RTCManager* rtc);
```

**Métodos públicos:**
```cpp
void begin()                     // Inicializa tela ativa e RTC
void update()                    // Atualiza NetworkService, RTC, tela e display
void draw()                      // Delega renderização à tela atual
void handleInput(ButtonEvent ev) // Gerencia wake-up do display e input
```

**Fluxo de transição de tela:**
```
currentScreen->update()
  → nextScreen() retorna ponteiro diferente
  → currentScreen->end()
  → currentScreen = next
  → currentScreen->begin()
  → display->resetAutoOff()
```

**Detecção de mudança de estado:**  
O `ScreenManager` também monitora `currentScreen->getState()`. Se o estado interno da tela mudar, o auto-off do display é resetado automaticamente — útil para telas que atualizam sua UI sem input do usuário.

**Lógica de input:**
- Se o display estiver desligado e qualquer botão for pressionado, **apenas acorda o display** (o input não é repassado à tela).
- Se o display estiver ligado, o evento é repassado normalmente e o timer de auto-off é resetado.

---

### 2. Input — InputManager + Buttons

**Arquivos:** `src/input/InputManager.{h,cpp}`, `src/drivers/Buttons.{h,cpp}`, `src/input/ButtonEvent.h`

#### InputManager
Abstrai a leitura dos botões e expõe um único evento por ciclo de `loop`.

**Eventos gerados** (`ButtonEvent`):
| Evento | Trigger |
|---|---|
| `Up` / `Down` / `Select` | Pressão curta |
| `UpHold` / `DownHold` / `SelectHold` | Pressão longa ≥ 500 ms |
| `None` | Nenhum evento no ciclo |

#### Buttons (Driver)
Driver de baixo nível. Usa a biblioteca **Bounce2** para debouncing automático (25 ms).

**Pinos configurados:**
| Botão | GPIO | Modo |
|---|---|---|
| UP | 12 | INPUT_PULLUP |
| DOWN | 13 | INPUT_PULLUP |
| SELECT | 14 | INPUT_PULLUP |

**Hold prevention:** após o evento de hold ser disparado, o evento `rose()` subsequente (soltar o botão) é ignorado para evitar duplo evento.

```cpp
// Lógica de prevenção de hold
if (btnUp.rose() && !upHoldFired)
    return ButtonEvent::Up;
```

---

### 3. Output — OutputManager + Lantern

**Arquivos:** `src/output/OutputManager.{h,cpp}`, `src/drivers/Lantern.{h,cpp}`

#### OutputManager
Camada de abstração para dispositivos de saída. Gerencia a `Lantern` e serve como ponto de extensão para novos periféricos (buzzer, motor, etc.).

#### Lantern (Driver)
Controla um LED/relé no GPIO 16.

- Lógica invertida: `LOW` = ligado
- Toggle simples com estado persistente

---

### 4. Drivers — Display + RTC

#### Display
**Arquivo:** `src/drivers/Display.{h,cpp}`  
**Hardware:** OLED SH1106 128×64 via I2C  
**Biblioteca:** U8g2

**Pinos I2C:**
| Sinal | GPIO |
|---|---|
| SCL | 4 |
| SDA | 5 |

**API pública:**
```cpp
void begin()                                      // Inicializa U8g2, liga display, contraste 0, auto-off 5s
void clear()                                      // Limpa o buffer interno
void display()                                    // Envia buffer para o hardware
void drawText(int x, int y, const char* text)
void printCentered(const char* text, int y)
void fontSet(const uint8_t* font)
void drawFrame(int x, int y, int w, int h)
void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
void drawHorizontalLine(int x, int y, int w)
void drawBitmap(int x, int y, int w, int h, const uint8_t* bitmap)
void drawColorSet(uint8_t colorIndex)
int  getWStr(const char* text)                    // Retorna largura em pixels da string
void setDisplayContrast(int contrastLevel)
void displayOn(bool status)                       // Liga/desliga via setPowerSave
bool getDisplayStatus()
void setAutoOff(unsigned long timeout)            // Configura timeout em ms
void handleAutoOff()                              // Chamado a cada loop pelo ScreenManager
void resetAutoOff()                               // Reinicia o timer e liga display se desligado
```

**Ciclo de uso padrão em telas:**
```cpp
display->clear();
// ... drawText, printCentered, etc.
display->display(); // obrigatório para enviar para a tela
```

#### RTCManager
**Arquivo:** `src/drivers/RTC.{h,cpp}`  
**Hardware:** DS3231 via I2C compartilhado

```cpp
void begin()
void update()                // Deve ser chamado todo loop (via ScreenManager)
String getTimeString()       // "14:30:45"
String getDateString()       // "25/12/2024"
String getDayOfWeek()
void setDateTime(tm* t)      // Usado pelo NTPScreen para sincronizar
```

---

### 5. Services — NetworkService

**Arquivo:** `src/services/NetworkService.{h,cpp}`

Singleton estático que gerencia o ciclo de vida da conexão WiFi.

**Estados (`ConnectionState`):**
```
IDLE → connect() → CONNECTING
CONNECTING → WL_CONNECTED → CONNECTED
CONNECTING → timeout (10s) → FAILED
CONNECTED → perda de sinal → IDLE
IDLE/FAILED → WiFi não desligado → disconnect() automático
```

**API:**
```cpp
static void connect(const char* ssid, const char* password)
static void disconnect(ConnectionState newState = IDLE)
static bool isConnected()
static ConnectionState getState()
static void update()   // Chamado automaticamente pelo ScreenManager
static void begin()    // Chama WiFi.mode(WIFI_OFF)
```

> **Nota:** O `NetworkService::update()` é chamado dentro de `ScreenManager::update()` a cada frame, então as telas não precisam gerenciá-lo diretamente.

---

## 🖼️ Sistema de Telas

### Interface `Screen` (Base)

**Arquivo:** `src/screens/Screen.h`

```cpp
class Screen {
public:
    virtual const char* name()            // Identificação para debug
    virtual void begin()                  // Inicialização ao entrar na tela
    virtual void end()                    // Limpeza ao sair da tela
    virtual void update()                 // Lógica (chamada todo frame)
    virtual void draw() = 0;             // Renderização (OBRIGATÓRIO)
    virtual void handleInput(ButtonEvent ev);  // Dispatch de eventos (implementado em Screen.cpp)
    virtual Screen* nextScreen()          // Retorna this (permanecer) ou ponteiro para próxima
    virtual uint8_t getState() const = 0; // Estado interno para detecção pelo ScreenManager

protected:
    // Callbacks de botão (override opcional)
    virtual void onUpPressed()    {}
    virtual void onDownPressed()  {}
    virtual void onSelectPressed(){}
    virtual void onUpHeld()       {}
    virtual void onDownHeld()     {}
    virtual void onSelectHeld()   {}
};
```

> **`getState()` é obrigatório.** Retorne um enum castado para `uint8_t` representando o estado visual atual da tela. O `ScreenManager` usa isso para resetar o auto-off quando a tela muda de estado internamente.

### Hierarquia de Navegação

```
BootScreen (inicial, 500 ms)
    ↓ automático
MenuAppScreen (hub central)
    ├→ ClockScreen        [Relogio]
    ├→ CalendarScreen     [Calendario]
    ├→ WiFiScreen         [Tela WiFi]
    ├→ NTPScreen          [Tela NTP]
    └→ FlashlightScreen   [Lanterna]
         (todas voltam ao MenuAppScreen)
```

### Telas Implementadas

| Tela | Arquivo | Descrição |
|---|---|---|
| `BootScreen` | `screens/Boot/` | Splash screen com animação, dura 500 ms |
| `MenuAppScreen` | `screens/MenuApp/` | Menu com scroll e animação LineGrow no item selecionado |
| `ClockScreen` | `screens/Clock/` | Exibe hora e data via RTC |
| `CalendarScreen` | `screens/Calendar/` | Exibe data completa via RTC |
| `WiFiScreen` | `screens/WiFi/` | Conecta/desconecta WiFi, exibe IP e RSSI |
| `NTPScreen` | `screens/NTP/` | Sincroniza RTC via internet |
| `FlashlightScreen` | `screens/Flashlight/` | Toggle da lanterna via OutputManager |

---

## 🎬 Sistema de Animações

**Arquivos:** `src/screens/animations/`

### Interface `Animation`
```cpp
class Animation {
    virtual void start() = 0;
    virtual void update() = 0;
    virtual void draw(Display* display) = 0;
    virtual bool finished() = 0;
};
```

### `Animator`
Gerenciador que faz *auto-delete* ao término das animações:
```cpp
animator.add(new LineGrowAnimation(...)); // Animator assume ownership
animator.update();      // Atualiza todos, remove os finished()
animator.draw(display); // Renderiza todos
animator.clear();       // Limpa todos (chamar em end())
```

> **Atenção:** O `Animator` faz `delete` automaticamente em animações cujo `finished()` retorna `true`. Nunca delete manualmente uma animação após adicioná-la.

### `LineGrowAnimation`
Única animação implementada atualmente. Desenha uma linha horizontal que cresce a partir de `x` até `maxWidth` pixels.

```cpp
LineGrowAnimation(
    int x,              // posição X inicial
    int y,              // posição Y da linha
    int maxWidth,       // largura final em pixels
    int speed,          // pixels por frame (negativo = encolhe)
    bool stopAtEnd,     // para ao atingir maxWidth
    bool removeOnFinish // remove do Animator ao terminar
)
```

---

## 🔄 Fluxo de Execução

### `setup()`
```cpp
display.begin();                              // 1. Display primeiro
menuAppScreen.setScreens(...);                // 2. Configura navegação do menu
screenManager.begin();                        // 3. Inicia BootScreen + RTC
inputManager.begin();                         // 4. Configura botões
outputManager.begin();                        // 5. Inicializa lanterna
```

### `loop()`
```cpp
screenManager.update();    // NetworkService, RTC, tela atual, transições, auto-off
inputManager.update();     // Bounce2 debounce
outputManager.update();    // Estado dos outputs

ButtonEvent ev = inputManager.getEvent();
screenManager.handleInput(ev);

screenManager.draw();      // Renderiza tela atual
```

---

## ⚙️ Dependências (platformio.ini)

```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
monitor_speed = 115200
upload_speed = 115200

lib_deps =
    olikraus/U8g2@^2.36.12
    arduino-libraries/NTPClient@^3.2.1
    thomasfredericks/Bounce2@^2.72
    northernwidget/DS3231@^1.1.2
```

---

## 🎛️ Configuração de Hardware

### Pinout

| Componente | GPIO | Função |
|---|---|---|
| Botão UP | 12 | INPUT_PULLUP |
| Botão DOWN | 13 | INPUT_PULLUP |
| Botão SELECT | 14 | INPUT_PULLUP |
| Lanterna (LED/Relé) | 16 | OUTPUT (lógica invertida) |
| OLED SDA | 5 | I2C Data |
| OLED SCL | 4 | I2C Clock |
| RTC DS3231 | (I2C compartilhado) | — |

### Esquema de Conexão

```
ESP8266 (ESP-12E)
├─ GPIO 12 ──── [Botão UP]     ──── GND
├─ GPIO 13 ──── [Botão DOWN]   ──── GND
├─ GPIO 14 ──── [Botão SELECT] ──── GND
├─ GPIO 16 ──── [LED / Relé]   ──── GND
├─ GPIO 5 (SDA) ─┐
└─ GPIO 4 (SCL) ─┴── [OLED SH1106 + RTC DS3231 via I2C]
```

---

## 🧠 Conceitos-chave

### Event-Driven Input
Botões não são lidos diretamente pelas telas. O `InputManager` produz um único `ButtonEvent` por ciclo, desacoplando hardware de lógica de apresentação.

### Screen Transitions via Ponteiro
Telas nunca manipulam o `ScreenManager` diretamente. A transição acontece pelo retorno de `nextScreen()`:
```cpp
Screen* nextScreen() override {
    return shouldGoBack ? backScreen : this;
}
```

### Auto-Off Display
O timer é resetado por qualquer input ou por mudança no valor de `getState()`. O primeiro input após o desligamento apenas religa o display, sem repassar o evento à tela.

### Hold Prevention
Após um evento `*Hold` ser disparado, o `rose()` do botão é ignorado para evitar que um evento `*Pressed` também seja gerado ao soltar.

### getState() e Mudança de Estado
Cada tela expõe seu estado interno via `getState()`. O `ScreenManager` compara a cada frame — se mudou, chama `display->resetAutoOff()`. Use para telas com atualizações assíncronas (ex.: WiFiScreen mostrando progresso de conexão).

---

## 📝 Convenções de Código

- **Classes:** PascalCase (`MenuAppScreen`, `RTCManager`)
- **Variáveis/métodos:** camelCase (`selectedIndex`, `handleInput`)
- **Constantes/pinos:** UPPER_SNAKE_CASE (`PIN_UP_BTN`, `PIN_LANTERN`)
- **Arquivos:** um par `.h`/`.cpp` por classe, dentro de pasta por categoria
- **Enums de estado:** `enum class NomeDaTelaState { IDLE, ... }`

---

## 📚 Referências

- [U8g2 Documentation](https://github.com/olikraus/u8g2)
- [DS3231 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf)
- [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
- [Bounce2 Library](https://github.com/thomasfredericks/Bounce2)
- [NTPClient Library](https://github.com/arduino-libraries/NTPClient)

---

## 📖 Documentação Adicional

- **[🛠️ Guia de Desenvolvimento](docs/DEVELOPMENT_GUIDE.md)** — Como adicionar drivers, criar telas e implementar animações

**Contribuindo:**
1. Fork o projeto
2. Crie sua feature branch: `git checkout -b feature/MinhaFeature`
3. Commit: `git commit -m 'Add: MinhaFeature'`
4. Push: `git push origin feature/MinhaFeature`
5. Abra um Pull Request
