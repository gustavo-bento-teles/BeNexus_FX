#pragma once
#include "Screen.h"
#include "drivers/Display.h"
#include "drivers/RTC.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "animations/LineGrowAnimation.h"
#include "animations/Animator.h"

class NTPScreen : public Screen {
public:
    NTPScreen(Display* disp, RTCManager* rtc, Screen* next, Screen* wifi)
        : display(disp),  rtcManager(rtc), nextScreenPtr(next), wifiScreen(wifi), nextTriggered(false) {}

    const char* name() override { return "NTPScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    void atualizarNTP();

    Animator animator;

    Display* display;
    RTCManager* rtcManager;
    Screen* nextScreenPtr;

    Screen* wifiScreen;

    bool nextTriggered;
    bool firstClient = true;

    WiFiUDP ntpUDP;
    NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);
};