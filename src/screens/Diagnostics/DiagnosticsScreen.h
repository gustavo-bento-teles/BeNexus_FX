#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"

class DiagnosticsScreen : public Screen {
public:
    DiagnosticsScreen(Display* disp)
        : display(disp), selectedIndex(0), nextScreenPtr(nullptr), nextTriggered(false) {}

    const char* name() override { return "DiagnosticsScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

    void setScreens(Screen* rtcDiag) {
        rtcDiagnostics = rtcDiag;
    }

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    Display* display;
    Screen* nextScreenPtr;
    bool nextTriggered;

    Screen* rtcDiagnostics;

    int selectedIndex = 0;
    int offset = 0;
    const int numMaxVisible = 5;
    
    static constexpr const char* options[3] = {
        "RTC",
        "Lanterna",
        "Display"
    };

    static constexpr int numOptions = 3;
};