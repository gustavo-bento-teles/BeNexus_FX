#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

enum class CalendarScreenState { IDLE };

class CalendarScreen : public Screen {
public:
  CalendarScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "CalendarScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::CALENDAR; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::CALENDAR;
  }

  static Screen *create(DriverContext &driverContext) {
    return new CalendarScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onSelectPressed() override;
  void onUpPressed() override;
  void onDownPressed() override;

  void onSelectHeld() override;
  void onUpHeld() override;
  void onDownHeld() override;

private:
  DriverContext &driverContext;
  ScreenID nextScreenID;
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
