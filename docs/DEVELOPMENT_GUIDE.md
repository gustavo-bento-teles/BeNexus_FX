# 🛠️ Guia de Desenvolvimento — BeNexus_FX

Este guia cobre os padrões e passos necessários para estender o projeto: adicionar novos drivers de hardware, criar telas e implementar animações, seguindo as convenções atuais do codebase (baseadas em `DriverContext` e `StaticRegistry`).

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
│   ├── ScreenManager.{hpp,cpp}
│   └── StaticRegistry.{hpp,cpp}      ← ScreenID + fábrica de telas
├── drivers/
│   ├── DriverContext.{hpp,cpp}       ← Struct central com todos os drivers
│   ├── Display.{hpp,cpp}
│   ├── RTC.{hpp,cpp}
│   ├── Buttons.{hpp,cpp}
│   ├── Lantern.{hpp,cpp}
│   └── <NovoDriver>.{hpp,cpp}        ← Novos drivers aqui
├── screens/
│   ├── Screen.{hpp,cpp}              ← Interface base
│   ├── animations/
│   │   ├── Animation.hpp
│   │   ├── Animator.hpp
│   │   ├── LineGrowAnimation.hpp
│   │   └── <NovaAnimacao>.hpp        ← Novas animações aqui
│   ├── Boot/
│   │   └── BootScreen.{hpp,cpp}
│   ├── MenuApp/
│   │   └── MenuAppScreen.{hpp,cpp}
│   ├── Clock/
│   ├── Calendar/
│   ├── WiFi/
│   ├── NTP/
│   ├── Flashlight/
│   └── <NovaTela>/
│       └── <NovaTela>Screen.{hpp,cpp}   ← Novas telas aqui
├── input/
│   ├── InputManager.{hpp,cpp}
│   └── ButtonEvent.hpp
└── services/
    └── NetworkService.{hpp,cpp}
```

> Não existe mais uma pasta `output/` ou `OutputManager`. Dispositivos de saída (como a `Lantern`) são acessados diretamente através do `DriverContext`.

---

## 🔌 Adicionando Novos Drivers

Todo driver deve seguir o padrão de **arquivo duplo** (`.hpp` + `.cpp`) e implementar ao menos `begin()` para inicialização.

### Passo a Passo

#### 1. Criar os Arquivos do Driver

**Exemplo: Buzzer**

**`src/drivers/Buzzer.hpp`**
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
#include "Buzzer.hpp"

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

#### 2. Integrar ao `DriverContext`

Todo driver — de entrada ou saída — é registrado diretamente no `DriverContext`. Não há mais separação entre `InputManager`/`OutputManager` para posse dos drivers: essas classes apenas leem o `DriverContext` por referência.

**`src/drivers/DriverContext.hpp`**
```cpp
#pragma once

#include "drivers/Buttons.hpp"
#include "drivers/Buzzer.hpp"   // ← novo include
#include "drivers/Display.hpp"
#include "drivers/Lantern.hpp"
#include "drivers/RTC.hpp"

struct DriverContext {
  Display *display;
  RTC *rtc;
  Buttons *buttons;
  Lantern *lantern;
  Buzzer *buzzer;               // ← novo campo
};

extern Display display;
extern RTC rtc;
extern Buttons buttons;
extern Lantern lantern;
extern Buzzer buzzer;           // ← nova instância global

DriverContext initDriverContext();
```

**`src/drivers/DriverContext.cpp`**
```cpp
#include "DriverContext.hpp"

const uint8_t PIN_UP_BTN = 12;
const uint8_t PIN_DOWN_BTN = 13;
const uint8_t PIN_SELECT_BTN = 14;

const uint8_t PIN_LANTERN = 16;
const uint8_t PIN_BUZZER = 15;   // ← novo pino

Display display;
RTC rtc;
Buttons buttons(PIN_UP_BTN, PIN_DOWN_BTN, PIN_SELECT_BTN);
Lantern lantern(PIN_LANTERN);
Buzzer buzzer(PIN_BUZZER);       // ← nova instância

DriverContext initDriverContext() {
  return {&display, &rtc, &buttons, &lantern, &buzzer};
}
```

#### 3. Inicializar o Driver no `ScreenManager::begin()`

Se o driver precisar de uma etapa de inicialização de hardware (`begin()`), adicione a chamada em `ScreenManager::begin()`, junto dos demais drivers:

```cpp
void ScreenManager::begin() {
  driverContext.display->begin();
  driverContext.rtc->begin();
  driverContext.lantern->begin();
  driverContext.buzzer->begin();   // ← novo driver inicializado aqui

  currentScreen = createScreen(initialScreenID, driverContext);
  currentScreen->begin();
}
```

Se o driver exigir `update()` contínuo (como o auto-stop do `Buzzer`), chame-o em `ScreenManager::update()`.

#### 4. Consumir o Driver em uma Tela

Qualquer tela recebe `DriverContext &` no construtor, então o novo driver já está disponível sem nenhuma mudança adicional de assinatura:

```cpp
void MinhaScreen::onSelectPressed() {
  driverContext.buzzer->beep(1000, 100);
}
```

### Diagrama de Integração

```
┌─────────────────┐
│ Hardware (GPIO) │
└────────┬────────┘
         │
    ┌────▼────┐
    │ Driver  │  Buzzer.{hpp,cpp}
    │ begin() │
    │ update()│
    └────┬────┘
         │
  ┌──────▼───────────┐
  │  DriverContext    │  struct com ponteiros para todos os drivers
  └──────┬────────────┘
         │
   ┌─────▼──────┐
   │  Screens   │  consomem via driverContext.buzzer->...
   └────────────┘
```

---

## 🖼️ Criando Novas Telas

### Interface Obrigatória

Toda tela **deve** herdar de `Screen` e implementar obrigatoriamente `draw()` e `getState()`. Também deve expor um método estático `create(DriverContext&)`, que será registrado no `StaticRegistry`.

```cpp
// src/screens/Screen.hpp
class Screen {
public:
    virtual ~Screen() {}

    virtual const char* name()   { return "Unnamed"; }
    virtual void begin()         {}   // chamado ao entrar
    virtual void end()           {}   // chamado ao sair
    virtual void update()        {}   // lógica por frame
    virtual void draw() = 0;         // OBRIGATÓRIO
    virtual void handleInput(ButtonEvent ev);  // dispatch automático

    virtual ScreenID selfScreenID() const { return ScreenID::NONE; }
    virtual ScreenID nextScreen()   const { return ScreenID::NONE; }

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

#### 1. Registrar o `ScreenID`

**`src/core/StaticRegistry.hpp`** — adicionar o identificador da nova tela:
```cpp
enum class ScreenID {
  NONE,
  BOOT,
  CALENDAR,
  CLOCK,
  FLASHLIGHT,
  MENUAPP,
  NTP,
  STOPWATCH,   // ← novo ScreenID
  WIFI
};
```

#### 2. Criar os Arquivos da Tela

**Exemplo: Cronômetro**

**`src/screens/Stopwatch/StopwatchScreen.hpp`**
```cpp
#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

enum class StopwatchState {
  STOPPED,
  RUNNING
};

class StopwatchScreen : public Screen {
public:
  StopwatchScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "StopwatchScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::STOPWATCH; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::STOPWATCH;
  }

  static Screen *create(DriverContext &driverContext) {
    return new StopwatchScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<uint8_t>(screenState); }

protected:
  void onSelectPressed() override; // Start/Stop
  void onUpPressed()     override; // Reset
  void onDownHeld()      override; // Voltar ao menu

private:
  DriverContext &driverContext;
  ScreenID nextScreenID;
  bool nextTriggered;

  bool          running     = false;
  unsigned long startTime   = 0;
  unsigned long elapsedTime = 0;

  StopwatchState screenState = StopwatchState::STOPPED;

  String formatTime(unsigned long ms);
};
```

**`src/screens/Stopwatch/StopwatchScreen.cpp`**
```cpp
#include "StopwatchScreen.hpp"

void StopwatchScreen::begin() {
  nextTriggered = false;
  running       = false;
  elapsedTime   = 0;
  screenState   = StopwatchState::STOPPED;
  driverContext.display->clear();
  driverContext.display->display();
}

void StopwatchScreen::update() {
  if (running) {
    elapsedTime = millis() - startTime;
  }
}

void StopwatchScreen::draw() {
  driverContext.display->clear();
  driverContext.display->printCentered(formatTime(elapsedTime).c_str(), 32);
  driverContext.display->display();
}

void StopwatchScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
}

void StopwatchScreen::onSelectPressed() {
  running = !running;
  screenState = running ? StopwatchState::RUNNING : StopwatchState::STOPPED;
  if (running) startTime = millis() - elapsedTime;
}

void StopwatchScreen::onUpPressed() {
  elapsedTime = 0;
}

void StopwatchScreen::onDownHeld() {
  nextTriggered = true;
}

String StopwatchScreen::formatTime(unsigned long ms) {
  unsigned long totalSeconds = ms / 1000;
  char buf[9];
  sprintf(buf, "%02lu:%02lu:%02lu", totalSeconds / 3600,
          (totalSeconds / 60) % 60, totalSeconds % 60);
  return String(buf);
}
```

#### 3. Registrar a Tela no `StaticRegistry`

**`src/core/StaticRegistry.cpp`** — incluir o header e adicionar uma linha na tabela de registro:
```cpp
#include "screens/Stopwatch/StopwatchScreen.hpp"   // ← novo include

static const ScreenRegistry registry[] = {
    makeScreenRegistry<BootScreen>(ScreenID::BOOT),
    makeScreenRegistry<CalendarScreen>(ScreenID::CALENDAR),
    makeScreenRegistry<ClockScreen>(ScreenID::CLOCK),
    makeScreenRegistry<FlashlightScreen>(ScreenID::FLASHLIGHT),
    makeScreenRegistry<MenuAppScreen>(ScreenID::MENUAPP),
    makeScreenRegistry<NTPScreen>(ScreenID::NTP),
    makeScreenRegistry<StopwatchScreen>(ScreenID::STOPWATCH), // ← nova linha
    makeScreenRegistry<WiFiScreen>(ScreenID::WIFI),
};
```

Não é necessário instanciar a tela em `main.cpp` nem passar ponteiros manualmente — o `ScreenManager` a criará sob demanda via `createScreen()` sempre que outra tela retornar `ScreenID::STOPWATCH` em `nextScreen()`.

#### 4. Adicionar a Entrada no `MenuAppScreen`

**`src/screens/MenuApp/MenuAppScreen.cpp`** — incluir a nova opção na tabela `appsRegistry`:
```cpp
constexpr AppRegistry appsRegistry[6] = {{"Relogio", ScreenID::CLOCK},
                                         {"Calendario", ScreenID::CALENDAR},
                                         {"Tela WiFi", ScreenID::WIFI},
                                         {"Tela NTP", ScreenID::NTP},
                                         {"Lanterna", ScreenID::FLASHLIGHT},
                                         {"Cronometro", ScreenID::STOPWATCH}}; // ← nova entrada
```

O `numOptions` é calculado automaticamente via `sizeof(appsRegistry) / sizeof(appsRegistry[0])`, portanto nenhum ajuste manual de contagem é necessário.

---

## 🎬 Sistema de Animações

### Interface `Animation`

**`src/screens/animations/Animation.hpp`**
```cpp
#pragma once
#include "drivers/Display.hpp"

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
// src/screens/animations/FadeAnimation.hpp
#pragma once
#include "Animation.hpp"
#include "drivers/Display.hpp"

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
    driverContext.display->clear();
    animator.draw(driverContext.display); // renderiza animações por cima
    // ... resto da UI
    driverContext.display->display();
}

void MinhaScreen::end() {
    animator.clear(); // SEMPRE limpar em end()
    driverContext.display->clear();
    driverContext.display->display();
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
int width = driverContext.display->getWStr(options[selectedIndex]) - 19;
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

// ✅ CORRETO: usar o driver correspondente via DriverContext
void FlashlightScreen::onSelectPressed() {
    driverContext.lantern->toggle();
}
```

### Transições de Tela

```cpp
// ✅ CORRETO: sempre via nextScreen(), retornando um ScreenID
ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : selfScreenID();
}

// ❌ ERRADO: não acesse o ScreenManager diretamente a partir de uma tela,
//            nem tente guardar/retornar Screen* — a navegação é feita por ScreenID
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

### Gerenciamento de Memória das Telas

O `ScreenManager` aloca cada tela com `new` ao entrar nela e a destrói com `delete` ao sair. Isso significa que:

- Nenhuma tela deve ser instanciada manualmente em `main.cpp`.
- Todo estado que precisa sobreviver entre transições (como `selectedIndex` do `MenuAppScreen`) deve ser declarado como membro `static` da classe, e não como membro de instância.

```cpp
// ✅ CORRETO: estado persistente entre recriações da tela
static int selectedIndex;

// ❌ ERRADO: seria resetado toda vez que a tela for recriada
int selectedIndex = 0;
```

### Inicialização e Limpeza

```cpp
void MinhaScreen::begin() {
    // Sempre resetar estado ao entrar
    counter      = 0;
    nextTriggered = false;
    screenState  = MinhaState::IDLE;
    animator.clear();
    driverContext.display->clear();
    driverContext.display->display();
}

void MinhaScreen::end() {
    // Sempre limpar recursos ao sair
    animator.clear();
    driverContext.display->clear();
    driverContext.display->display();
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
class MenuAppScreen {};          // Classes: PascalCase
int selectedIndex = 0;           // Variáveis: camelCase
void handleInput();              // Métodos: camelCase
const uint8_t PIN_BUZZER = 15;   // Constantes: UPPER_SNAKE_CASE
enum class BootScreenState {};   // Enums de estado: PascalCase + "State"
enum class ScreenID {};          // Identificadores de tela: PascalCase
```

---

## 🚀 Checklists

### Adicionando Driver
- [ ] Criar `Driver.hpp` e `Driver.cpp` em `src/drivers/`
- [ ] Implementar `begin()`
- [ ] Implementar `update()` se necessário (auto-stop, polling, etc.)
- [ ] Adicionar o ponteiro do driver ao struct `DriverContext`
- [ ] Declarar a instância global e o pino em `DriverContext.cpp`
- [ ] Atualizar `initDriverContext()` para incluir o novo ponteiro
- [ ] Chamar `begin()` do driver em `ScreenManager::begin()`
- [ ] Chamar `update()` do driver em `ScreenManager::update()`, se aplicável

### Criando Tela
- [ ] Adicionar o novo `ScreenID` em `StaticRegistry.hpp`
- [ ] Herdar de `Screen`
- [ ] Implementar `draw()` (obrigatório)
- [ ] Implementar `getState()` com enum de estado (obrigatório)
- [ ] Implementar `name()` para debug
- [ ] Implementar `begin()` com reset de estado e limpeza do display
- [ ] Implementar `end()` com `animator.clear()` e limpeza do display
- [ ] Implementar `selfScreenID()` e `nextScreen()`
- [ ] Implementar o método estático `create(DriverContext&)`
- [ ] Fazer override dos callbacks de botão necessários
- [ ] Registrar a tela na tabela `registry[]` em `StaticRegistry.cpp`
- [ ] Adicionar a entrada correspondente em `appsRegistry` (`MenuAppScreen.cpp`)

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
    driverContext.display->clear();
    // ...
    driverContext.display->display(); // ← você esqueceu isso
}
```

### Animação não funciona
```cpp
void MinhaScreen::update() {
    animator.update(); // ← necessário todo frame
}
void MinhaScreen::draw() {
    animator.draw(driverContext.display); // ← necessário todo frame
}
```

### Tela fica presa / não transita
```cpp
// Certifique-se de que nextScreen() retorna o ScreenID correto
ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : selfScreenID(); // ← selfScreenID() aqui, não NONE
}
// E que nextTriggered é setado corretamente em algum callback
```

Verifique também se a tela foi registrada em `StaticRegistry.cpp`: se `createScreen()` não encontrar o `ScreenID` na tabela, retorna `nullptr`, e o `ScreenManager` não fará a transição.

### Display some depois de 5 segundos
Isso é o auto-off normal. Se quiser desabilitar em uma tela específica, chame `driverContext.display->resetAutoOff()` no `update()`. Mas prefira implementar `getState()` corretamente — o `ScreenManager` faz isso automaticamente.

### Driver não responde
```cpp
void ScreenManager::begin() {
  driverContext.display->begin();
  driverContext.rtc->begin();
  driverContext.lantern->begin();
  driverContext.buzzer->begin(); // ← você adicionou a chamada de begin() do novo driver?
  // ...
}
```

### Vazamento de memória
```cpp
void MinhaScreen::end() {
    animator.clear(); // ← libera todas as animações alocadas com new
}
```

Lembre-se também que o próprio `ScreenManager` já cuida do `delete` da tela anterior a cada transição — não é necessário (nem correto) gerenciar isso manualmente dentro da tela.

---

## 📚 Recursos Adicionais

- Veja as telas já implementadas como referência de padrão real
- Padrões de projeto utilizados: **Factory** (`StaticRegistry`/`createScreen`), **State** (telas), **Observer** (eventos), **Strategy** (animações)
- Documentação das bibliotecas no `platformio.ini`
- Pinout completo e arquitetura geral no `README.md`
