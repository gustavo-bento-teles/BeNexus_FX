# 🛠️ Guia de Desenvolvimento - BeNexus_FX

## 📋 Índice
1. [Adicionando Novos Drivers](#-adicionando-novos-drivers)
2. [Criando Novas Telas](#%EF%B8%8F-criando-novas-telas)
3. [Sistema de Animações](#-sistema-de-animações)
4. [Boas Práticas](#-boas-práticas)

---

## 🔌 Adicionando Novos Drivers

### Estrutura Base

Todo driver deve seguir o padrão de **arquivo duplo** (.h + .cpp) e implementar ao menos um método `begin()` para inicialização.

### Passo a Passo

#### 1️⃣ Criar os Arquivos do Driver

**Exemplo: Buzzer**

**`drivers/Buzzer.h`**
```cpp
#pragma once
#include <Arduino.h>

class Buzzer {
public:
    Buzzer(uint8_t pin);
    
    void begin();
    void beep(uint16_t frequency, uint16_t duration);
    void playTone(uint16_t frequency);
    void stopTone();
    bool isPlaying() const;

private:
    uint8_t pin;
    bool playing = false;
    unsigned long stopTime = 0;
};
```

**`drivers/Buzzer.cpp`**
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

bool Buzzer::isPlaying() const {
    return playing;
}
```

#### 2️⃣ Classificar o Driver (Input ou Output)

**Para dispositivos de SAÍDA** (LED, Buzzer, Motor, etc):
- Adicionar ao **OutputManager**

**Para dispositivos de ENTRADA** (Sensores, Botões extras, etc):
- Adicionar ao **InputManager**

#### 3️⃣ Integrar no Manager Correspondente

**Exemplo: OutputManager com Buzzer**

**`output/OutputManager.h`**
```cpp
#pragma once
#include "drivers/Lantern.h"
#include "drivers/Buzzer.h"  // ← Adicionar include

class OutputManager {
public:
    // Adicionar referência no construtor
    explicit OutputManager(Lantern& lantern, Buzzer& buzzer);
    
    void begin();
    void update();

    // Lantern
    void setLantern(bool on);
    void toggleLantern();
    bool isLanternOn() const;
    
    // Buzzer - adicionar métodos de abstração
    void playBeep(uint16_t freq, uint16_t duration);
    void playTone(uint16_t freq);
    void stopSound();
    bool isBuzzerPlaying() const;

private:
    Lantern& lantern;
    Buzzer& buzzer;  // ← Adicionar referência
};
```

**`output/OutputManager.cpp`**
```cpp
#include "output/OutputManager.h"

// Atualizar construtor
OutputManager::OutputManager(Lantern& l, Buzzer& b)
    : lantern(l), buzzer(b) {}

void OutputManager::begin() {
    lantern.begin();
    buzzer.begin();  // ← Adicionar inicialização
}

void OutputManager::update() {
    // Atualizar lógica se necessário
}

// Implementar métodos de abstração
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

// Métodos existentes da Lantern...
```

#### 4️⃣ Atualizar main.cpp

```cpp
#include "drivers/Buzzer.h"  // ← Adicionar include

const uint8_t PIN_BUZZER = 15;  // ← Definir pino

// Criar instância
Buzzer buzzer(PIN_BUZZER);

// Atualizar construtor do OutputManager
OutputManager outputManager(lantern, buzzer);

void setup() {
    // ... resto do código
    outputManager.begin();  // Já vai inicializar o buzzer também
}
```

### 📊 Diagrama de Fluxo

```
┌─────────────────┐
│ Hardware (GPIO) │
└────────┬────────┘
         │
    ┌────▼────┐
    │ Driver  │  (Buzzer.h/cpp)
    │ .begin()│
    │ .beep() │
    └────┬────┘
         │
  ┌──────▼──────────┐
  │ OutputManager   │  (camada de abstração)
  │ .playBeep()     │
  └──────┬──────────┘
         │
    ┌────▼─────┐
    │ Screens  │  (uso nas telas)
    └──────────┘
```

---

## 🖼️ Criando Novas Telas

### Interface Obrigatória

Toda tela **DEVE** implementar a interface `Screen`:

**`screens/Screen.h`**
```cpp
#pragma once
#include "input/InputManager.h"

class Screen {
public:
    virtual ~Screen() {}

    virtual const char* name() { return "Unnamed"; }

    virtual void begin() {}        // Inicialização
    virtual void end() {}          // Limpeza
    virtual void update() {}       // Lógica (chamado todo frame)
    virtual void draw() = 0;       // Renderização (obrigatório!)
    virtual void handleInput(ButtonEvent ev);  // Entrada
    virtual Screen* nextScreen() { return nullptr; }  // Navegação

protected:
    // Callbacks de botões (override opcional)
    virtual void onUpPressed() {}
    virtual void onDownPressed() {}
    virtual void onSelectPressed() {}

    virtual void onUpHeld() {}
    virtual void onDownHeld() {}
    virtual void onSelectHeld() {}
};
```

### Passo a Passo

#### 1️⃣ Criar Arquivos da Tela

**Exemplo: Cronômetro**

**`screens/Stopwatch/StopwatchScreen.h`**
```cpp
#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"

class StopwatchScreen : public Screen {
public:
    StopwatchScreen(Display* disp, Screen* menuScreen);

    const char* name() override { return "StopwatchScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;
    
    Screen* nextScreen() override;

protected:
    void onSelectPressed() override;  // Start/Stop
    void onUpPressed() override;      // Reset
    void onDownHeld() override;       // Voltar ao menu

private:
    Display* display;
    Screen* backScreen;
    
    bool running = false;
    unsigned long startTime = 0;
    unsigned long elapsedTime = 0;
    bool shouldGoBack = false;
    
    String formatTime(unsigned long ms);
};
```

**`screens/Stopwatch/StopwatchScreen.cpp`**
```cpp
#include "StopwatchScreen.h"

StopwatchScreen::StopwatchScreen(Display* disp, Screen* menu)
    : display(disp), backScreen(menu) {}

void StopwatchScreen::begin() {
    shouldGoBack = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
}

void StopwatchScreen::update() {
    if (running) {
        elapsedTime = millis() - startTime;
    }
}

void StopwatchScreen::draw() {
    display->clear();
    
    // Título
    display->fontSet(u8g2_font_6x10_tr);
    display->printCentered("CRONOMETRO", 10);
    
    // Tempo
    display->fontSet(u8g2_font_10x20_tr);
    display->printCentered(formatTime(elapsedTime).c_str(), 35);
    
    // Status
    display->fontSet(u8g2_font_6x10_tr);
    const char* status = running ? "RODANDO" : "PARADO";
    display->printCentered(status, 55);
    
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
        running = false;
    } else {
        running = true;
        startTime = millis() - elapsedTime;
    }
}

void StopwatchScreen::onUpPressed() {
    running = false;
    elapsedTime = 0;
}

void StopwatchScreen::onDownHeld() {
    shouldGoBack = true;
}

String StopwatchScreen::formatTime(unsigned long ms) {
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    seconds = seconds % 60;
    unsigned long millis = (ms % 1000) / 10;
    
    char buf[12];
    sprintf(buf, "%02lu:%02lu.%02lu", minutes, seconds, millis);
    return String(buf);
}
```

#### 2️⃣ Instanciar no main.cpp

```cpp
#include "screens/Stopwatch/StopwatchScreen.h"

// Criar instância (ordem importa: depende do menu)
StopwatchScreen stopwatchScreen(&display, &menuAppScreen);

void setup() {
    // ... código existente ...
    
    // Se quiser adicionar ao menu, precisa implementar lógica no MenuAppScreen
}
```

#### 3️⃣ Adicionar ao Menu (Futuro)

⚠️ **Nota:** A lógica dinâmica de adição ao menu ainda não foi implementada. Por enquanto, é necessário:

1. Adicionar manualmente a opção no array `options[]` do `MenuAppScreen`
2. Criar um ponteiro para a nova tela no `MenuAppScreen.h`
3. Adicionar o case no switch do método `onSelectPressed()`

---

## 🎬 Sistema de Animações

### Interface Base

**`animations/Animation.h`**
```cpp
#pragma once

class Animation {
public:
    virtual ~Animation() {}

    virtual void start() = 0;                    // Inicializar animação
    virtual void update() = 0;                   // Atualizar frame
    virtual void draw(Display* display) = 0;     // Renderizar
    virtual bool finished() = 0;                 // Verificar se terminou
};
```

### Gerenciador de Animações

O `Animator` gerencia múltiplas animações simultaneamente:

**`animations/Animator.h`**
```cpp
#pragma once
#include <vector>
#include "Animation.h"

class Animator {
private:
    std::vector<Animation*> animations;

public:
    void add(Animation* anim) {
        anim->start();
        animations.push_back(anim);
    }

    void update() {
        // Atualiza e remove animações finalizadas
        for (int i = animations.size() - 1; i >= 0; i--) {
            animations[i]->update();
            if (animations[i]->finished()) {
                delete animations[i];
                animations.erase(animations.begin() + i);
            }
        }
    }

    void draw(Display* display) {
        for (auto* anim : animations) {
            anim->draw(display);
        }
    }

    void clear() {
        for (auto* anim : animations) delete anim;
        animations.clear();
    }

    ~Animator() { clear(); }
};
```

### Criando Nova Animação

**Exemplo: FadeAnimation**

**`animations/FadeAnimation.h`**
```cpp
#pragma once
#include "Animation.h"
#include "drivers/Display.h"

class FadeAnimation : public Animation {
public:
    FadeAnimation(int startContrast, int endContrast, int durationMs)
        : start(startContrast), end(endContrast), duration(durationMs) {}

    void start() override {
        startTime = millis();
        currentContrast = start;
    }

    void update() override {
        unsigned long elapsed = millis() - startTime;
        float progress = (float)elapsed / duration;
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            done = true;
        }
        
        currentContrast = start + (end - start) * progress;
    }

    void draw(Display* display) override {
        display->setDisplayContrast(currentContrast);
    }

    bool finished() override {
        return done;
    }

private:
    int start, end;
    int duration;
    unsigned long startTime = 0;
    int currentContrast = 0;
    bool done = false;
};
```

### Usando Animações em Telas

**Exemplo: MenuAppScreen**

**`screens/MenuAppScreen.h`**
```cpp
#pragma once
#include "Screen.h"
#include "drivers/Display.h"
#include "animations/Animator.h"
#include "animations/LineGrowAnimation.h"

class MenuAppScreen : public Screen {
public:
    MenuAppScreen(Display* disp)
        : display(disp), selectedIndex(0) {}

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    Display* display;
    int selectedIndex = 0;
    
    Animator animator;  // ← Gerenciador de animações
    
    static constexpr const char* options[5] = {
        "Relogio/Data",
        "Calendario",
        "Tela WiFi",
        "Tela NTP",
        "Lanterna"
    };
    static constexpr int numOptions = 5;
};
```

**`screens/MenuAppScreen.cpp`**
```cpp
#include "MenuAppScreen.h"

void MenuAppScreen::begin() {
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
    display->display();

    // Adicionar animação inicial
    animator.add(new LineGrowAnimation(
        22, 
        12 + (selectedIndex * 12), 
        display->getWStr(options[selectedIndex]) - 19, 
        3, 
        true, 
        true
    ));
}

void MenuAppScreen::update() {
    animator.update();  // ← Atualiza todas as animações
}

void MenuAppScreen::draw() {
    display->clear();
    
    animator.draw(display);  // ← Desenha todas as animações
    
    // Resto do código de desenho...
    
    display->display();
}

void MenuAppScreen::end() {
    animator.clear();  // ← Limpa todas as animações
    display->clear();
    display->display();
}

void MenuAppScreen::onUpPressed() {
    if (selectedIndex > 0) {
        selectedIndex--;
    } else {
        selectedIndex = numOptions - 1;
    }
    
    // Adiciona nova animação ao trocar opção
    animator.add(new LineGrowAnimation(
        22, 
        12 + (selectedIndex * 12), 
        display->getWStr(options[selectedIndex]) - 19, 
        3, 
        true, 
        true
    ));
}
```

### 🎭 Fluxo de Animação

```
┌──────────────────┐
│ Screen::begin()  │
│   animator.add() │  ← Adiciona animação
└────────┬─────────┘
         │
    ┌────▼──────────┐
    │ Animation     │
    │ .start()      │  ← Inicializa
    └────┬──────────┘
         │
    ┌────▼──────────┐  (todo frame)
    │ Screen::update│
    │ animator.     │
    │   update()    │  ← Atualiza frames
    └────┬──────────┘
         │
    ┌────▼──────────┐  (todo frame)
    │ Screen::draw()│
    │ animator.     │
    │   draw()      │  ← Renderiza
    └────┬──────────┘
         │
    ┌────▼──────────┐
    │ finished()?   │
    │   true → 🗑️   │  ← Auto-deleta quando termina
    └───────────────┘
```

---

## ✅ Boas Práticas

### Nomenclatura
```cpp
// Classes: PascalCase
class MenuAppScreen { };

// Variáveis/métodos: camelCase
int selectedIndex = 0;
void updateDisplay();

// Constantes: UPPER_SNAKE_CASE
const uint8_t PIN_BUZZER = 15;
```

### Estrutura de Arquivos
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
│   └── Buzzer.{h,cpp}           ← Novos drivers aqui
├── screens/
│   ├── Screen.h
│   ├── BootScreen.{h,cpp}
│   ├── MenuAppScreen.{h,cpp}
|   ├── ...
|   └── NewScreen.{h,cpp}        ← Novas screens aqui
├── animations/
│   ├── Animation.h
│   ├── Animator.h
│   |── LineGrowAnimation.{h,cpp}
|   └── NewAnimation.{h,cpp}     ← Novas animações aqui
├── input/
│   ├── InputManager.{h,cpp}
│   └── ButtonEvent.h
├── output/
│   └── OutputManager.{h,cpp}
└── services/
    └── NetworkService.{h,cpp}
```

### Responsabilidade Única
```cpp
// ❌ ERRADO: Tela fazendo controle de hardware
void ClockScreen::draw() {
    digitalWrite(LED_PIN, HIGH);  // NÃO!
    display->drawText(...);
}

// ✅ CORRETO: Usar OutputManager
void ClockScreen::update() {
    if (needLight) {
        outputManager->setLantern(true);  // SIM!
    }
}
```

### Gerenciamento de Memória
```cpp
// ⚠️ ATENÇÃO: Animações são auto-deletadas
animator.add(new FadeAnimation(...));  // OK, Animator gerencia

// ❌ NÃO faça isso:
FadeAnimation* anim = new FadeAnimation(...);
animator.add(anim);
delete anim;  // ERRO! Animator já vai deletar
```

### Transições de Tela
```cpp
// Sempre usar o padrão nextScreen()
Screen* nextScreen() override {
    return shouldTransition ? targetScreen : this;
}

// ❌ NÃO manipule screenManager diretamente da tela
```

### Inicialização e Limpeza
```cpp
class MyScreen : public Screen {
    void begin() override {
        // Sempre resetar estado
        counter = 0;
        animator.clear();
        display->clear();
    }
    
    void end() override {
        // Sempre limpar recursos
        animator.clear();
        display->clear();
        display->display();
    }
};
```

---

## 🚀 Checklist de Desenvolvimento

### Adicionando Driver
- [ ] Criar `Driver.h` e `Driver.cpp` em `drivers/`
- [ ] Implementar método `begin()`
- [ ] Adicionar ao `InputManager` ou `OutputManager`
- [ ] Atualizar `main.cpp` com instância
- [ ] Definir pino GPIO como constante
- [ ] Testar isoladamente antes de integrar

### Criando Tela
- [ ] Herdar de `Screen`
- [ ] Implementar `draw()` (obrigatório)
- [ ] Implementar `name()` para debug
- [ ] Implementar `begin()` e `end()` para cleanup
- [ ] Implementar `nextScreen()` para navegação
- [ ] Override callbacks de botões se necessário
- [ ] Instanciar no `main.cpp`
- [ ] Adicionar ao menu (quando implementado)

### Criando Animação
- [ ] Herdar de `Animation`
- [ ] Implementar `start()`, `update()`, `draw()`, `finished()`
- [ ] Testar performance (evitar operações pesadas)
- [ ] Garantir que `finished()` sempre retorna true eventualmente
- [ ] Adicionar via `Animator::add()` com `new`
- [ ] Não deletar manualmente (Animator gerencia)

---

## 🐛 Debug Tips

### Tela não aparece
```cpp
// Verificar se está implementando draw()
void MyScreen::draw() {
    display->clear();
    // ... seu código ...
    display->display();  // ← NÃO ESQUEÇA!
}
```

### Animação não funciona
```cpp
// Verificar se update() e draw() estão sendo chamados
void MyScreen::update() {
    animator.update();  // ← Necessário!
}

void MyScreen::draw() {
    animator.draw(display);  // ← Necessário!
}
```

### Driver não responde
```cpp
// Verificar se begin() foi chamado
void OutputManager::begin() {
    buzzer.begin();  // ← Necessário!
}
```

### Vazamento de memória
```cpp
// Sempre limpar em end()
void MyScreen::end() {
    animator.clear();  // ← Libera memória!
}
```

---

## 📚 Recursos Adicionais

- **Exemplos práticos:** Veja as telas já implementadas como referência
- **Bibliotecas:** Documentação das libs no `platformio.ini`
- **Hardware:** Pinout completo na documentação principal
- **Patterns:** Observer (eventos), State (telas), Strategy (animações)

---

**Boa codificação! 🎮**
