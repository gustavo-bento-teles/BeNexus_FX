#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"
#include "drivers/RTC.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

enum class NTPScreenState {
    CONNECTED,
    DISCONNECTED,
    UPDATING
};

class NTPScreen : public Screen {
public:
    NTPScreen(Display* disp, RTCManager* rtc, Screen* next, Screen* wifi)
        : display(disp),  rtcManager(rtc), nextScreenPtr(next), wifiScreen(wifi), menuAppScreen(next), nextTriggered(false) {}

    const char* name() override { return "NTPScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

    uint8_t getState() const override { return static_cast<int>(screenState); } 

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    void atualizarNTP();

    Display* display;
    RTCManager* rtcManager;
    Screen* nextScreenPtr;

    Screen* wifiScreen;
    Screen* menuAppScreen;

    bool nextTriggered;
    bool firstClient = true;

    WiFiUDP ntpUDP;
    NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

    NTPScreenState screenState = NTPScreenState::DISCONNECTED;
};