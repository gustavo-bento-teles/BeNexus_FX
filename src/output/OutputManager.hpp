#pragma once
#include "drivers/Lantern.hpp"

class OutputManager {
public:
  explicit OutputManager(Lantern &lantern);
  void begin();
  void update();

  void setLantern(bool on);
  void toggleLantern();
  bool isLanternOn() const;

private:
  Lantern &lantern;
};
