// Include
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <U8g2lib.h>
#include <Bounce2.h>
#include <Wire.h>
#include <DS3231.h>

#include "Buzzer.h"
#include "Screens.h"

// Define
#define UP_BTN     12
#define DOWN_BTN   13
#define SELECT_BTN 14

#define LED_LATERAL 16

#define BUZZER 0

const int SUB_INFORMACOES_HARDWARE = 0;
const int SUB_INFORMACOES_SOFTWARE = 1;

// Tela inicial e variável que controla a tela
Tela tela = Tela::CLOCK;
int subTela = SUB_INFORMACOES_HARDWARE;

bool dentroSubTela = false;

// Debounce nos botões
Bounce upBtn     = Bounce();
Bounce downBtn   = Bounce();
Bounce selectBtn = Bounce();

// Credenciais da internet na qual o relógio vai se conectar
const char* ssid     = "Hotispoti";
const char* password = "#-&S3nha";

// Rádio Wifi
WiFiMode_t wifiMode;

// Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, 4, 5);

// RTC
DS3231 rtc;
bool h12, pm;

// Ícones

// engrenagem, 32x32px
const unsigned char epd_bitmap_engrenagem [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 
	0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0xc0, 0xf9, 0x9f, 0x03, 
	0xe0, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0x0f, 0xf8, 0x3f, 0xfc, 0x1f, 
	0xf8, 0x1f, 0xf8, 0x1f, 0xe0, 0x0f, 0xf0, 0x07, 0xc0, 0x0f, 0xf0, 0x03, 0xc0, 0x07, 0xe0, 0x03, 
	0xc0, 0x07, 0xe0, 0x03, 0xc0, 0x0f, 0xf0, 0x03, 0xe0, 0x0f, 0xf0, 0x07, 0xf8, 0x1f, 0xf8, 0x1f, 
	0xf8, 0x7f, 0xfe, 0x1f, 0xf0, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0x07, 
	0xc0, 0xf9, 0x9f, 0x03, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 
	0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// lanternaDesligada, 32x32px
const unsigned char epd_bitmap_lanternaDesligada [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x03, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 
	0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 
	0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 
	0x00, 0xf8, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 
	0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 
	0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00
};

// lanternaLigada, 32x32px
const unsigned char epd_bitmap_lanternaLigada [] PROGMEM = {
	0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x80, 0x80, 0x01, 0x01, 
	0xc0, 0x81, 0x81, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x07, 0xe0, 0x00, 
	0x00, 0xc2, 0x43, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 
	0xf8, 0xf8, 0x1f, 0x1f, 0xf8, 0xf8, 0x1f, 0x1f, 0xf8, 0xf8, 0x1f, 0x1f, 0x00, 0xf8, 0x1f, 0x00, 
	0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 
	0x00, 0xf8, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 
	0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 
	0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00
};


// NTP do Brasil no horário de Brasília
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3*3600, 1000);
bool firstClient = true;

// Variável que avalia se a tela está ligada
bool telaLigada = true;

bool lanternaLigada = false;

bool tentandoConectarWifi = false;

bool inicializacaoSilenciosa = true;

// Configuração do tempo limite
const unsigned long SCREEN_TIMEOUT = 5000; 

// Guarda o último tempo de interação
unsigned long lastInteraction = 0;

// Liga o WiFi e conecta
void ligarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return; // já está conectado

  tentandoConectarWifi = true;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
  }
  
  tentandoConectarWifi = false;
}

// Desliga o WiFi
void desligarWiFi() {
  WiFi.disconnect(true);  // desconecta e apaga credenciais
  WiFi.mode(WIFI_OFF);    // desliga rádio
}


void atualizarNTP() {
  display.clearBuffer();
  display.drawStr((128 - display.getStrWidth("Updating NTP...")) / 2, 32, "Updating NTP...");
  display.drawStr((128 - display.getStrWidth("Wait...")) / 2, 45, "Wait...");
  display.sendBuffer();

  if (firstClient) {
    timeClient.begin();
    firstClient = false;
  }

  bool ok = timeClient.forceUpdate(); // tenta atualizar uma vez
  display.clearBuffer();

  if (ok) {
    display.drawStr((128 - display.getStrWidth("NTP updated!")) / 2, 32, "NTP updated!");
  } else {
    display.drawStr((128 - display.getStrWidth("Update failed!")) / 2, 32, "Update failed!");
    return;
  }

  display.sendBuffer();
  delay(1000);

  String formattedTime = timeClient.getFormattedTime();  // "14:37:52"

  int hour   = formattedTime.substring(0, 2).toInt();    // 14
  int minute = formattedTime.substring(3, 5).toInt();    // 37
  int second = formattedTime.substring(6, 8).toInt();    // 52

  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(second);

  time_t rawtime = timeClient.getEpochTime();
  struct tm *ti = localtime(&rawtime);

  int dia   = ti->tm_mday;
  int mes   = ti->tm_mon + 1;
  int ano   = ti->tm_year + 1900;
  int semana = ti->tm_wday; // 0 = domingo, 1 = segunda...

  rtc.setYear(ano - 2000);
  rtc.setMonth(mes);
  rtc.setDate(dia);
  rtc.setDoW((semana == 0) ? 1 : semana + 1);

  display.clearBuffer();

  display.drawStr((128 - display.getStrWidth("RTC updated!")) / 2, 32, "RTC updated!");

  display.sendBuffer();
  delay(1000);
}



void setup() {
  Wire.begin();
  rtc.setClockMode(false); // 24h

	display.begin();
  display.setContrast(0);
  display.setPowerSave(0);

  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 10, "Starting buttons...");
  display.sendBuffer();

  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(SELECT_BTN, INPUT_PULLUP);
	delay(250);

  display.drawStr(0, 20, "Starting leds...");
  display.sendBuffer();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_LATERAL, OUTPUT);
  digitalWrite(LED_LATERAL, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);

	display.drawStr(0, 30, "Doing debounce...");
  display.sendBuffer();

  upBtn.attach(UP_BTN); upBtn.interval(25);
  downBtn.attach(DOWN_BTN); downBtn.interval(25);
  selectBtn.attach(SELECT_BTN); selectBtn.interval(25);
	delay(250);

  display.drawStr(0, 40, "Starting buzzer...");
  display.sendBuffer();
  delay(250);

  pinMode(BUZZER, OUTPUT);

  if (selectBtn.read() == LOW) {
    inicializacaoSilenciosa = false;
  }

  if (downBtn.read() == LOW) {
    return;
  }

  display.setFont(u8g2_font_ncenB08_tr);

  if (inicializacaoSilenciosa) {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 50, "Init S Mode...");
    display.sendBuffer();
    delay(250);
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(int(128 - display.getStrWidth("Init S..."))/2, 32, "Init S...");
    display.sendBuffer();

  } else {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 50, "Init D Mode...");
    display.sendBuffer();
    delay(250);
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(int(128 - display.getStrWidth("Init D..."))/2, 32, "Init D...");
    display.sendBuffer();
    alertSound(BUZZER);
  }

  delay(500);

  display.clearBuffer();

  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(int(128 - display.getStrWidth("v1.1"))/2, 10, "v1.1");

  display.setFont(u8g2_font_9x15_mf);
  display.drawStr(int(128 - display.getStrWidth("BeNexus_FX"))/2, 32, "BeNexus_FX");

  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(int(128 - display.getStrWidth("by Gustavo Bento"))/2, 60, "by Gustavo Bento");

  display.sendBuffer();
  
  if (!inicializacaoSilenciosa) {
    digitalWrite(LED_BUILTIN, LOW);
    playMario(BUZZER);
    delay(100);
	  digitalWrite(LED_BUILTIN, HIGH);
  } else {
    delay(2000);
  }

  desligarWiFi();

  lastInteraction = millis();
}

void loop() {
  upBtn.update();
  downBtn.update();
  selectBtn.update();

	if (selectBtn.fell()) {
    if (!telaLigada) {
        // Se a tela está desligada → só liga
        telaLigada = true;
        lastInteraction = millis();
        return;
    } else {
        if (tela == Tela::WIFI || tela == Tela::NTP_SCREEN || tela == Tela::LANTERNA || tela == Tela::INFORMACOES) {
          // deixa passar
        } else if (tela == Tela::NTP_SCREEN && WiFi.status() != WL_CONNECTED) {
          tela = Tela::WIFI;
          lastInteraction = millis();
          return;

        } else {
            telaLigada = false;
        }
    }

    lastInteraction = millis();
  }

  if(upBtn.fell() && telaLigada) {
    if (tela == Tela::INFORMACOES && dentroSubTela) {
      // Se estiver dentro da subtela, apenas passa
      // Aqui é onde acontece a navegação entre subtelas
    } else {
      tela = nextTela(tela);
    }
  }

  if(downBtn.fell() && telaLigada) {
    if (tela == Tela::INFORMACOES && dentroSubTela) {
      // Se estiver dentro da subtela, apenas passa
      // Aqui é onde acontece a navegação entre subtelas
    } else {
      tela = prevTela(tela);
    }
  }

  // Se a tela está ligada → conta tempo de inatividade
  if (telaLigada) {
      if (upBtn.fell() || downBtn.fell()) {
          lastInteraction = millis(); // interação só se já está ligada
      }

      // Verifica se passou do tempo limite
      if (millis() - lastInteraction > SCREEN_TIMEOUT) {
          telaLigada = false; // apaga a tela automaticamente
      }
  }

  wifiMode = WiFi.getMode();

  if (WiFi.status() != WL_CONNECTED && wifiMode != WIFI_OFF && !tentandoConectarWifi) {
    desligarWiFi();
  }

  // Atualiza display só se a tela estiver ligada
  if(telaLigada){
    switch (tela) {
      case Tela::CLOCK: {
        display.setPowerSave(0);
        display.clearBuffer();

        // Hora centralizada
        display.setFont(u8g2_font_fub20_tr);
        bool h12Flag, pmFlag;

        int hour   = rtc.getHour(h12Flag, pmFlag);
        int minute = rtc.getMinute();
        int second = rtc.getSecond();

        char buf[10];
        sprintf(buf, "%02d:%02d:%02d", hour, minute, second);

        int x = (128 - display.getStrWidth(buf)) / 2;
        display.drawStr(x, 42, buf);

        display.sendBuffer();
        break;
      }
      

      case Tela::DATE: {
        display.setPowerSave(0);
        display.clearBuffer();
        display.setFont(u8g2_font_6x10_tr);

        // Nome dos dias da semana
        const char* diasSemana[] = {
          "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
        };

        bool century = false;

        // Linha 1: Data completa
        char buf[20];
        sprintf(buf,
                "%02d/%02d/%04d",
                rtc.getMonth(century),
                rtc.getDate(),
                rtc.getYear() + 2000);
        int x = (128 - display.getStrWidth(buf)) / 2;
        display.drawStr(x, 20, buf);

        // Linha 2: Nome do dia da semana
        int semana = rtc.getDoW() - 1;
        if (semana < 0) semana = 6;

        const char* nomeDia = diasSemana[semana];
        int x2 = (128 - display.getStrWidth(nomeDia)) / 2;
        display.drawStr(x2, 40, nomeDia);

        display.sendBuffer();

        break;
      }
        
      
      case Tela::WIFI: {
        display.setPowerSave(0);
        display.clearBuffer();
        display.setFont(u8g2_font_6x10_tr);

        if(WiFi.status() == WL_CONNECTED) {
          String ip = WiFi.localIP().toString();
          int rssi = WiFi.RSSI();

          display.drawStr(0, 10, "SSID:");
          display.drawStr(40, 10, WiFi.SSID().c_str());

          display.drawStr(0, 20, "IP:");
          display.drawStr(40, 20, ip.c_str());

          char sinal[20];
          sprintf(sinal, "RSSI: %d dBm", rssi);
          display.drawStr(0, 30, sinal);

          wifiMode = WiFi.getMode();

          switch (wifiMode) {
            case WIFI_OFF:
              display.drawStr((128 - display.getStrWidth("Radio WiFi OFF")) / 2, 45, "Radio WiFi OFF");
              break;
            
            case WIFI_AP:
              display.drawStr((128 - display.getStrWidth("Radio Wifi AP")) / 2, 45, "Radio Wifi AP");
              break;
            
            case WIFI_STA:
              display.drawStr((128 - display.getStrWidth("Radio Wifi STA")) / 2, 45, "Radio Wifi STA");
              break;

            case WIFI_AP_STA:
              display.drawStr((128 - display.getStrWidth("Radio Wifi STA e AP")) / 2, 45, "Radio Wifi STA e AP");
              break;
          
            default:
              break;
          }

          display.drawStr(int(128 - display.getStrWidth("[OFF WiFi]")) / 2, 62, "[OFF WiFi]");

          if (selectBtn.fell()) {
            desligarWiFi();   // desliga WiFi se já está conectado
          }

        } else {
          display.drawStr((128 - display.getStrWidth("WiFi Screen")) / 2, 8, "WiFi Screen");
          display.drawHLine(0, 12, 128);
          display.drawStr((128 - display.getStrWidth("[Turn on WiFi]")) / 2, 38, "[Turn on WiFi]");

          wifiMode = WiFi.getMode();

          switch (wifiMode) {
            case WIFI_OFF:
              display.drawStr((128 - display.getStrWidth("Radio WiFi OFF")) / 2, 60, "Radio WiFi OFF");
              break;
            
            case WIFI_AP:
              display.drawStr((128 - display.getStrWidth("Radio Wifi AP")) / 2, 60, "Radio Wifi AP");
              break;
            
            case WIFI_STA:
              display.drawStr((128 - display.getStrWidth("Radio Wifi STA")) / 2, 60, "Radio Wifi STA");
              break;

            case WIFI_AP_STA:
              display.drawStr((128 - display.getStrWidth("Radio Wifi STA e AP")) / 2, 60, "Radio Wifi STA e AP");
              break;
          
            default:
              break;
          }

          if (selectBtn.fell()) {
            display.clearBuffer();
            display.drawStr((128 - display.getStrWidth("Connecting on:")) / 2, 30, "Connecting on:");
            display.drawStr((128 - display.getStrWidth(ssid)) / 2, 45, ssid);

            display.sendBuffer();
            
            ligarWiFi();

            lastInteraction = millis();
          }
        }

        display.sendBuffer();
        break;
      }

      case Tela::NTP_SCREEN: {
        display.setPowerSave(0);
        display.clearBuffer();
        display.setFont(u8g2_font_6x10_tr);

        if(WiFi.status() == WL_CONNECTED) {
          display.drawStr((128 - display.getStrWidth("NTP Screen")) / 2, 8, "NTP Screen");
          display.drawHLine(0, 12, 128);
          display.drawStr((128 - display.getStrWidth("[Update NTP]")) / 2, 38, "[Update NTP]");

          if (selectBtn.fell()) {
            atualizarNTP();   // atualiza se já tem WiFi
          }

        } else {
          display.drawStr((128 - display.getStrWidth("NTP Screen")) / 2, 8, "NTP Screen");
          display.drawHLine(0, 12, 128);
          display.drawStr((128 - display.getStrWidth("No WiFi available!")) / 2, 30, "No WiFi available!");
          display.drawStr((128 - display.getStrWidth("Use [WiFi Screen]")) / 2, 45, "Use [WiFi Screen]");
          display.drawStr((128 - display.getStrWidth("to connect")) / 2, 55, "to connect");
        }

        display.sendBuffer();
        break;
      }

      case Tela::LANTERNA: {
        display.setPowerSave(0);
        display.clearBuffer();
        display.setFont(u8g2_font_6x10_tr);

        if (selectBtn.fell()) {
          lanternaLigada = !lanternaLigada;
        }

        if (lanternaLigada) {
          display.drawXBMP(48, 8, 32, 32, epd_bitmap_lanternaLigada);
          display.drawStr((128 - display.getStrWidth("[Lantern ON]")) / 2, 60, "[Lantern ON]");
          digitalWrite(LED_LATERAL, LOW);

        } else {
          display.drawXBMP(48, 8, 32, 32, epd_bitmap_lanternaDesligada);
          display.drawStr((128 - display.getStrWidth("[Lantern OFF]")) / 2, 60, "[Lantern OFF]");
          digitalWrite(LED_LATERAL, HIGH);
        }

        display.sendBuffer();
        break;
      }

      case Tela::INFORMACOES: {
        if (dentroSubTela) {
          display.setPowerSave(0);
          display.clearBuffer();
          switch (subTela) {
            case SUB_INFORMACOES_HARDWARE: {
              display.setFont(u8g2_font_6x10_tr);
              display.drawStr((128 - display.getStrWidth("Hardware Info")) / 2, 8, "Hardware Info");
              display.drawHLine(0, 10, 128);
              // CPU Info
              char cpuFreq[32];
              sprintf(cpuFreq, "CPU: %d MHz", ESP.getCpuFreqMHz());
              display.drawStr(2, 20, cpuFreq);

              // Flash Info
              char flashInfo[32];
              sprintf(flashInfo, "Flash: %d KB", ESP.getFlashChipRealSize() / 1024);
              display.drawStr(2, 30, flashInfo);

              // RAM Info
              char ramInfo[32];
              sprintf(ramInfo, "Free RAM: %d KB", ESP.getFreeHeap() / 1024);
              display.drawStr(2, 40, ramInfo);

              // Flash Speed
              char flashSpeed[32];
              sprintf(flashSpeed, "Flash Speed: %d MHz", ESP.getFlashChipSpeed() / 1000000);
              display.drawStr(2, 50, flashSpeed);

              // Battery Info
              float voltage = (analogRead(A0) * 4.2) / 1024.0;
              char batInfo[32];
              sprintf(batInfo, "Battery: %.2fV", voltage);
              display.drawStr(2, 60, batInfo);

              display.sendBuffer();

              if(upBtn.fell() && telaLigada || downBtn.fell() && telaLigada) {
                subTela = SUB_INFORMACOES_SOFTWARE;
              }

              if (selectBtn.fell()) {
                dentroSubTela = false;
              }

              break;
            }

            case SUB_INFORMACOES_SOFTWARE: {
              display.setFont(u8g2_font_6x10_tr);
              display.drawStr((128 - display.getStrWidth("Software Info")) / 2, 8, "Software Info");
              display.drawHLine(0, 10, 128);

              // SDK Version
              char sdkVer[32];
              sprintf(sdkVer, "SDK: %s", ESP.getSdkVersion());
              display.drawStr(2, 20, sdkVer);

              // Flash Info
              char flashFree[32];
              sprintf(flashFree, "Flash Free: %d KB", ESP.getFreeSketchSpace() / 1024);
              display.drawStr(2, 30, flashFree);

              // WiFi Status
              String wifiStatus = (WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected";
              char wifiInfo[32];
              sprintf(wifiInfo, "WiFi: %s", wifiStatus.c_str());
              display.drawStr(2, 40, wifiInfo);

              // Uptime
              unsigned long uptime = millis() / 1000; // Converte para segundos
              int uptimeHours = uptime / 3600;
              int uptimeMinutes = (uptime % 3600) / 60;
              int uptimeSeconds = uptime % 60;
              
              char uptimeStr[32];
              sprintf(uptimeStr, "Uptime: %02d:%02d:%02d", uptimeHours, uptimeMinutes, uptimeSeconds);
              display.drawStr(2, 50, uptimeStr);

              // Firmware Version
              display.drawStr(2, 60, "FW: BeNexus_FX v1.1");

              display.sendBuffer();

              if(upBtn.fell() && telaLigada || downBtn.fell() && telaLigada) {
                subTela = SUB_INFORMACOES_HARDWARE;
              }

              if (selectBtn.fell()) {
                dentroSubTela = false;
                subTela = SUB_INFORMACOES_HARDWARE;
              }

              break;
            }

            default:
              break;
          }
        } else {
          display.setPowerSave(0);
          display.clearBuffer();
          display.drawXBMP(48, 8, 32, 32, epd_bitmap_engrenagem);

          display.setFont(u8g2_font_6x10_tr);
          display.drawStr((128 - display.getStrWidth("[HW/SW Info]")) / 2, 60, "[HW/SW Info]");

          display.sendBuffer();

          if (selectBtn.fell()) {
            dentroSubTela = true;
          }
        }

        break;
      }
      
      default:
        break;
    }
  } else {
		display.setPowerSave(1);
    display.clearBuffer();
    display.sendBuffer();
	}
}