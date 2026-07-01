#pragma once
#include "drivers/Display.hpp"
#include "drivers/RTC.hpp"
#include "screens/Screen.hpp"

enum class CalendarScreenState { IDLE };

class CalendarScreen : public Screen {
public:
  CalendarScreen(Display *disp, RTCManager *rtc, Screen *next)
      : display(disp), rtcManager(rtc), nextScreenPtr(next),
        nextTriggered(false) {}

  const char *name() override { return "CalendarScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  Screen *nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onSelectPressed() override;
  void onUpPressed() override;
  void onDownPressed() override;

  void onSelectHeld() override;
  void onUpHeld() override;
  void onDownHeld() override;

private:
  Display *display;
  RTCManager *rtcManager;
  Screen *nextScreenPtr;
  bool nextTriggered;

  int currentDay;
  int currentMonth;
  int currentYear;

  int month;
  int year;

  int daysInMonth(int month, int year);
  int weekdayOf(int day, int month, int year);

  CalendarScreenState screenState = CalendarScreenState::IDLE;
};
