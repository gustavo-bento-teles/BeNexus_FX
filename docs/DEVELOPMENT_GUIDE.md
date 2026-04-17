# 🛠️ Guia de Desenvolvimento — BeNexus_FX

Este guia cobre os padrões e passos necessários para estender o projeto: adicionar novos drivers de hardware, criar telas, implementar animações e seguir as convenções do codebase.

---

## 📋 Índice

1. [Estrutura de Arquivos](#-estrutura-de-arquivos)
2. [Adicionando Novos Drivers](#-adicionando-novos-drivers)
3. [Criando Novas Telas](#%EF%B8%8F-criando-novas-telas)
4. [Sistema de Animações](#-sistema-de-animações)
5. [Boas Práticas](#-boas-práticas)
6. [Checklists](#-checklists)
7. [Debug Tips](#-debug-tips)

---

## 📁 Estrutura de Arquivos

```
src/
├── main.cpp
├── core/
│   └── ScreenManager.{h,cpp}
├── drivers/
│   ├── Display.{h,cpp}
│   ├── RTC.{h,cpp}
│   ├── Buttons.{h,cpp}
│   ├── Lantern.{h,cpp}
│   └── <NovoDriver>.{h,cpp}      ← Novos drivers aqui
├── screens/
│   ├── Screen.{h,cpp}            ← Interface base
│   ├── animations/
│   │   ├── Animation.h
│   │   ├── Animator.h
│   │   ├── LineGrowAnimation.h
│   │   └── <NovaAnimacao>.h      ← Novas animações aqui
│   ├── Boot/
│   │   └── BootScreen.{h,cpp}
│   ├── MenuApp/
│   │   └── MenuAppScreen.{h,cpp}
│   ├── Clock/
│   ├── Calendar/
│   ├── WiFi/
│   ├── NTP/
│   ├── Flashlight/
│   └── <NovaTela>/
│       └── <NovaTela>Screen.{h,cpp}   ← Novas telas aqui
├── input/
│   ├── InputManager.{h,cpp}
│   └── ButtonEvent.h
├── output/
│   └── OutputManager.{h,cpp}
└── services/
    └── NetworkService.{h,cpp}
```

---

## 🔌 Adicionando Novos Drivers

Todo driver deve seguir o padrão de **arquivo duplo** (`.h` + `.cpp`) e implementar ao menos `begin()` para inicialização.

### Passo a Passo

#### 1. Criar os Arquivos do Driver

**Exemplo: Buzzer**

**`src/drivers/Buzzer.h`**
```cpp
#pragma once
#include <Arduino.h>

class Buzzer {
public:
    explicit Buzzer(uint8_t pin);

    void begin();
    void beep(uint16_t frequency, uint16_t duration);
    void playTone(uint16_t frequency);
    void stopTone();
    bool isPlaying() const;
    void update(); // Gerencia auto-stop por millis()

private:
    uint8_t pin;
    bool playing = false;
    unsigned long stopTime = 0;
};
```

**`src/drivers/Buzzer.cpp`**
```cpp
#include "Buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin)
    : pin(buzzerPin) {}

void Buzzer::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::beep(uint16_t frequency, uint16_t duration) {
    tone(pin, frequency);
    playing = true;
    stopTime = millis() + duration;
}

void Buzzer::playTone(uint16_t frequency) {
    tone(pin, frequency);
    playing = true;
}

void Buzzer::stopTone() {
    noTone(pin);
    playing = false;
}

void Buzzer::update() {
    if (playing && millis() >= stopTime) {
        stopTone();
    }
}

bool Buzzer::isPlaying() const {
    return playing;
}
```

#### 2. Classificar o Driver (Input ou Output)

- **Dispositivos de SAÍDA** (LED, Buzzer, Motor): integrar ao `OutputManager`
- **Dispositivos de ENTRADA** (sensores, botões extras): integrar ao `InputManager`

#### 3. Integrar no Manager Correspondente

**`src/output/OutputManager.h`** — adicionar referência:
```cpp
#pragma once
#include "drivers/Lantern.h"
#include "drivers/Buzzer.h"  // ← novo include

class OutputManager {
public:
    explicit OutputManager(Lantern& lantern, Buzzer& buzzer); // ← atualizar construtor

    void begin();
    void update();

    // Lantern
    void setLantern(bool on);
    void toggleLantern();
    bool isLanternOn() const;

    // Buzzer
    void playBeep(uint16_t freq, uint16_t duration);
    void playTone(uint16_t freq);
    void stopSound();
    bool isBuzzerPlaying() const;

private:
    Lantern& lantern;
    Buzzer& buzzer;  // ← nova referência
};
```

**`src/output/OutputManager.cpp`**:
```cpp
#include "output/OutputManager.h"

OutputManager::OutputManager(Lantern& l, Buzzer& b)
    : lantern(l), buzzer(b) {}

void OutputManager::begin() {
    lantern.begin();
    buzzer.begin();  // ← inicializa novo driver
}

void OutputManager::update() {
    buzzer.update(); // ← propaga update se necessário
}

void OutputManager::playBeep(uint16_t freq, uint16_t duration) {
    buzzer.beep(freq, duration);
}

void OutputManager::playTone(uint16_t freq) {
    buzzer.playTone(freq);
}

void OutputManager::stopSound() {
    buzzer.stopTone();
}

bool OutputManager::isBuzzerPlaying() const {
    return buzzer.isPlaying();
}
```

#### 4. Atualizar main.cpp

```cpp
#include "drivers/Buzzer.h"

const uint8_t PIN_BUZZER = 15;

Buzzer buzzer(PIN_BUZZER);
OutputManager outputManager(lantern, buzzer); // ← atualizar construtor

void setup() {
    // ...
    outputManager.begin(); // já inicializa o buzzer internamente
}
```

### Diagrama de Integração

```
┌─────────────────┐
│ Hardware (GPIO) │
└────────┬────────┘
         │
    ┌────▼────┐
    │ Driver  │  Buzzer.{h,cpp}
    │ begin() │
    │ update()│
    └────┬────┘
         │
  ┌──────▼──────────┐
  │ OutputManager   │  camada de abstração
  │ playBeep()      │
  └──────┬──────────┘
         │
    ┌────▼─────┐
    │  Screens │  consomem via OutputManager*
    └──────────┘
```

---

## 🖼️ Criando Novas Telas

### Interface Obrigatória

Toda tela **deve** herdar de `Screen` e implementar obrigatoriamente `draw()` e `getState()`.

```cpp
// src/screens/Screen.h
class Screen {
public:
    virtual const char* name()   { return "Unnamed"; }
    virtual void begin()         {}   // chamado ao entrar
    virtual void end()           {}   // chamado ao sair
    virtual void update()        {}   // lógica por frame
    virtual void draw() = 0;         // OBRIGATÓRIO
    virtual void handleInput(ButtonEvent ev);  // dispatch automático
    virtual Screen* nextScreen() { return nullptr; }
    virtual uint8_t getState() const = 0; // OBRIGATÓRIO

protected:
    virtual void onUpPressed()    {}
    virtual void onDownPressed()  {}
    virtual void onSelectPressed(){}
    virtual void onUpHeld()       {}
    virtual void onDownHeld()     {}
    virtual void onSelectHeld()   {}
};
```

### Passo a Passo

#### 1. Criar os Arquivos da Tela

**Exemplo: Cronômetro**

**`src/screens/Stopwatch/StopwatchScreen.h`**
```cpp
#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"

enum class StopwatchState {
    STOPPED,
    RUNNING
};

class StopwatchScreen : public Screen {
public:
    StopwatchScreen(Display* disp, Screen* menuScreen);

    const char* name() override { return "StopwatchScreen"; }

    void begin()  override;
    void update() override;
    void draw()   override;
    void end()    override;

    Screen* nextScreen() override;
    uint8_t getState() const override { return static_cast<uint8_t>(screenState); }

protected:
    void onSelectPressed() override; // Start/Stop
    void onUpPressed()     override; // Reset
    void onDownHeld()      override; // Voltar ao menu

private:
    Display* display;
    Screen*  backScreen;

    bool          running     = false;
    unsigned long startTime   = 0;
    unsigned long elapsedTime = 0;
    bool          shouldGoBack = false;

    StopwatchState screenState = StopwatchState::STOPPED;

    String formatTime(unsigned long ms);
};
```

**`src/screens/Stopwatch/StopwatchScreen.cpp`**
```cpp
#include "StopwatchScreen.h"

StopwatchScreen::StopwatchScreen(Display* disp, Screen* menu)
    : display(disp), backScreen(menu) {}

void StopwatchScreen::begin() {
    shouldGoBack = false;
    running      = false;
    elapsedTime  = 0;
    screenState  = StopwatchState::STOPPED;
    display->clear();
    display->display();
}

void StopwatchScreen::update() {
    if (running) {
        elapsedTime = millis() - startTime;
        screenState = StopwatchState::RUNNING;
    }
}

void StopwatchScreen::draw() {
    display->clear();

    display->fontSet(u8g2_font_6x10_tr);
    display->printCentered("CRONOMETRO", 10);

    display->fontSet(u8g2_font_10x20_tr);
    display->printCentered(formatTime(elapsedTime).c_str(), 35);

    display->fontSet(u8g2_font_6x10_tr);
    display->printCentered(running ? "RODANDO" : "PARADO", 55);

    display->display();
}

void StopwatchScreen::end() {
    display->clear();
    display->display();
}

Screen* StopwatchScreen::nextScreen() {
    return shouldGoBack ? backScreen : this;
}

void StopwatchScreen::onSelectPressed() {
    if (running) {
        running     = false;
        screenState = StopwatchState::STOPPED;
    } else {
        running    = true;
        startTime  = millis() - elapsedTime;
        screenState = StopwatchState::RUNNING;
    }
}

void StopwatchScreen::onUpPressed() {
    running     = false;
    elapsedTime = 0;
    screenState = StopwatchState::STOPPED;
}

void StopwatchScreen::onDownHeld() {
    shouldGoBack = true;
}

String StopwatchScreen::formatTime(unsigned long ms) {
    unsigned long secs    = ms / 1000;
    unsigned long mins    = secs / 60;
    secs                  = secs % 60;
    unsigned long centis  = (ms % 1000) / 10;

    char buf[12];
    sprintf(buf, "%02lu:%02lu.%02lu", mins, secs, centis);
    return String(buf);
}
```

#### 2. Instanciar no main.cpp

```cpp
#include "screens/Stopwatch/StopwatchScreen.h"

StopwatchScreen stopwatchScreen(&display, &menuAppScreen);
```

#### 3. Adicionar ao Menu

Editar `MenuAppScreen.h` e `MenuAppScreen.cpp`:

```cpp
// MenuAppScreen.h — adicionar ponteiro e opção
Screen* stopwatchScreen;

static constexpr const char* options[6] = {
    "Relogio",
    "Calendario",
    "Tela WiFi",
    "Tela NTP",
    "Lanterna",
    "Cronometro"   // ← nova entrada
};
static constexpr int numOptions = 6;

// Atualizar setScreens() para receber a nova tela
void setScreens(Screen* clk, Screen* calendar, Screen* wifi,
                Screen* ntp, Screen* flash, Screen* stopwatch);
```

```cpp
// MenuAppScreen.cpp — adicionar case no switch
case 5:
    nextScreenPtr = stopwatchScreen;
    nextTriggered = true;
    break;
```

```cpp
// main.cpp — atualizar chamada de setScreens
menuAppScreen.setScreens(
    &clockScreen,
    &calendarScreen,
    &wifiScreen,
    &ntpScreen,
    &flashlightScreen,
    &stopwatchScreen    // ← nova tela
);
```

---

## 🎬 Sistema de Animações

### Interface `Animation`

**`src/screens/animations/Animation.h`**
```cpp
#pragma once

class Animation {
public:
    virtual ~Animation() {}
    virtual void start()              = 0; // chamado pelo Animator ao add()
    virtual void update()             = 0; // atualiza estado por frame
    virtual void draw(Display* disp)  = 0; // renderiza
    virtual bool finished()           = 0; // sinal para auto-delete
};
```

### Gerenciador `Animator`

```cpp
// Animator assume ownership das animações adicionadas com add()
animator.add(new MinhaAnimacao(...)); // OK — Animator gerencia memória
animator.update();                    // atualiza todos, remove finished()
animator.draw(display);               // renderiza todos
animator.clear();                     // delete todos (usar em end())
```

### Criando Nova Animação

**Exemplo: FadeAnimation**

```cpp
// src/screens/animations/FadeAnimation.h
#pragma once
#include "Animation.h"
#include "drivers/Display.h"

class FadeAnimation : public Animation {
public:
    FadeAnimation(int fromContrast, int toContrast, int durationMs)
        : from(fromContrast), to(toContrast), duration(durationMs) {}

    void start() override {
        startTime = millis();
        current   = from;
        done      = false;
    }

    void update() override {
        float progress = (float)(millis() - startTime) / duration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            done     = true;
        }
        current = from + (to - from) * progress;
    }

    void draw(Display* display) override {
        display->setDisplayContrast(current);
    }

    bool finished() override { return done; }

private:
    int  from, to, duration;
    int  current   = 0;
    bool done      = false;
    unsigned long startTime = 0;
};
```

**Usando em uma tela:**
```cpp
void MinhaScreen::begin() {
    // Fade-in ao abrir a tela
    animator.add(new FadeAnimation(0, 200, 500));
}

void MinhaScreen::update() {
    animator.update();
}

void MinhaScreen::draw() {
    display->clear();
    animator.draw(display); // renderiza animações por cima
    // ... resto da UI
    display->display();
}

void MinhaScreen::end() {
    animator.clear(); // SEMPRE limpar em end()
    display->clear();
    display->display();
}
```

### LineGrowAnimation (já implementada)

Cresce uma linha horizontal da posição `x` até `maxWidth` pixels:

```cpp
LineGrowAnimation(
    int x,              // X inicial
    int y,              // Y da linha
    int maxWidth,       // largura alvo em pixels
    int speed,          // px por frame (negativo = encolher)
    bool stopAtEnd,     // parar ao atingir maxWidth
    bool removeOnFinish // se true, Animator remove ao terminar
)
```

**Cálculo de largura para alinhar ao texto:**
```cpp
// Padrão usado no MenuAppScreen
int width = display->getWStr(options[selectedIndex]) - 19;
animator.add(new LineGrowAnimation(22, y, width, 3, true, true));
```

---

## ✅ Boas Práticas

### Responsabilidade Única

```cpp
// ❌ ERRADO: tela manipulando hardware diretamente
void ClockScreen::draw() {
    digitalWrite(16, HIGH); // nunca!
}

// ✅ CORRETO: usar a camada de abstração
void ClockScreen::update() {
    if (needLight) outputManager->setLantern(true);
}
```

### Transições de Tela

```cpp
// ✅ CORRETO: sempre via nextScreen()
Screen* nextScreen() override {
    return shouldTransition ? targetScreen : this;
}

// ❌ ERRADO: não acesse screenManager diretamente de uma tela
```

### Gerenciamento de Memória com Animator

```cpp
// ✅ CORRETO
animator.add(new FadeAnimation(...)); // Animator gerencia delete

// ❌ ERRADO: double-free
FadeAnimation* anim = new FadeAnimation(...);
animator.add(anim);
delete anim; // CRASH — Animator já vai deletar
```

### Inicialização e Limpeza

```cpp
void MinhaScreen::begin() {
    // Sempre resetar estado ao entrar
    counter      = 0;
    triggered    = false;
    screenState  = MinhaState::IDLE;
    animator.clear();
    display->clear();
    display->display();
}

void MinhaScreen::end() {
    // Sempre limpar recursos ao sair
    animator.clear();
    display->clear();
    display->display();
}
```

### getState() e Estados

```cpp
// Declare um enum de estado para cada tela
enum class MinhaTelaState {
    IDLE,
    LOADING,
    SUCCESS,
    ERROR
};

// Retorne corretamente em getState()
uint8_t getState() const override {
    return static_cast<uint8_t>(screenState);
}

// Atualize o estado na lógica
void MinhaScreen::update() {
    if (algo) screenState = MinhaTelaState::LOADING;
}
// O ScreenManager detecta a mudança e reseta o auto-off automaticamente
```

### Nomenclatura

```cpp
class MenuAppScreen {};         // Classes: PascalCase
int selectedIndex = 0;          // Variáveis: camelCase
void handleInput();              // Métodos: camelCase
const uint8_t PIN_BUZZER = 15;  // Constantes: UPPER_SNAKE_CASE
enum class BootScreenState {};  // Enums de estado: PascalCase + "State"
```

---

## 🚀 Checklists

### Adicionando Driver
- [ ] Criar `Driver.h` e `Driver.cpp` em `src/drivers/`
- [ ] Implementar `begin()`
- [ ] Implementar `update()` se necessário (auto-stop, polling, etc.)
- [ ] Integrar ao `InputManager` ou `OutputManager`
- [ ] Atualizar o construtor do manager correspondente
- [ ] Instanciar em `main.cpp` e passar ao manager
- [ ] Definir pino GPIO como constante em `main.cpp`

### Criando Tela
- [ ] Herdar de `Screen`
- [ ] Implementar `draw()` (obrigatório)
- [ ] Implementar `getState()` com enum de estado (obrigatório)
- [ ] Implementar `name()` para debug
- [ ] Implementar `begin()` com reset de estado e limpeza do display
- [ ] Implementar `end()` com `animator.clear()` e limpeza do display
- [ ] Implementar `nextScreen()` para navegação
- [ ] Fazer override dos callbacks de botão necessários
- [ ] Instanciar em `main.cpp`
- [ ] Adicionar ao `MenuAppScreen`

### Criando Animação
- [ ] Herdar de `Animation`
- [ ] Implementar `start()`, `update()`, `draw()`, `finished()`
- [ ] Garantir que `finished()` eventualmente retorna `true` (se `removeOnFinish = true`)
- [ ] Testar performance (evitar operações pesadas por frame)
- [ ] Adicionar via `animator.add(new ...)` — não deletar manualmente

---

## 🐛 Debug Tips

### Tela não aparece
```cpp
void MinhaScreen::draw() {
    display->clear();
    // ...
    display->display(); // ← você esqueceu isso
}
```

### Animação não funciona
```cpp
void MinhaScreen::update() {
    animator.update(); // ← necessário todo frame
}
void MinhaScreen::draw() {
    animator.draw(display); // ← necessário todo frame
}
```

### Tela fica presa / não transita
```cpp
// Certifique que nextScreen() retorna o ponteiro correto
Screen* nextScreen() override {
    return triggered ? nextScreenPtr : this; // ← null aqui causa crash
}
// E que triggered é setado corretamente em algum callback
```

### Display some depois de 5 segundos
Isso é o auto-off normal. Se quiser desabilitar em uma tela específica, chame `display->resetAutoOff()` no `update()`. Mas prefira implementar `getState()` corretamente — o `ScreenManager` faz isso automaticamente.

### Driver não responde
```cpp
void OutputManager::begin() {
    lantern.begin();
    buzzer.begin(); // ← você adicionou ao begin()?
}
```

### Vazamento de memória
```cpp
void MinhaScreen::end() {
    animator.clear(); // ← libera todas as animações alocadas com new
}
```

---

## 📚 Recursos Adicionais

- Veja as telas já implementadas como referência de padrão real
- Padrões de projeto utilizados: **State** (telas), **Observer** (eventos), **Strategy** (animações)
- Documentação das bibliotecas no `platformio.ini`
- Pinout completo e arquitetura geral no `README.md`

---

**Bora codar! 🎮**
