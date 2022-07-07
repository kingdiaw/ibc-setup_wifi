//(1)-Inlude Library
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

//(2)-Define Constant Value
const uint16_t WM_TIMEOUT = 120;
const char* NAME_AP = "ESP32_AP";  //You may change to other name


//(3)-Object Mapping
WiFiManager wm;

//(4)-I/O Mapping
const uint8_t WM_TRIGGER_PIN = 15;
const uint8_t LED_PIN = 2;


//(5)-Global Variable Declaration
bool configPortal = false;

//====================== SETUP ==========================
void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Setup_io();
  Setup_wifi();
}

//====================== LOOP ===========================
void loop() {
  // put your main code here, to run repeatedly:
  Led_blink( WM_running(configPortal));

}

//=======================================================
//(6)-User Define Function
//6.1 - Setup_IO
void Setup_io(void) {
  pinMode (LED_PIN, OUTPUT);
  pinMode (WM_TRIGGER_PIN, INPUT_PULLUP);
}

//6.2 - Setup wifi
void Setup_wifi(void) {
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(WM_TIMEOUT);
  if (wm.autoConnect(NAME_AP)) {
    Serial.println("connected...yeey :)");
    configPortal = false;
  }
  else {
    Serial.println("Configportal running");
    configPortal = true;
  }
}

//6.3 - If button pressed, Enter WM configportal
uint32_t WM_running(bool wifiConfig) {
  static uint32_t kLedInterval;

  wm.process();

  if (WiFi.status() == WL_CONNECTED) {
    kLedInterval = 250;
    wifiConfig = false;
  }
  if (wifiConfig) {
    kLedInterval = 50;
  }
  else if (WiFi.status() != WL_CONNECTED) {
    kLedInterval = 2000;
  }

  // is configuration portal requested?
  if (digitalRead(WM_TRIGGER_PIN) == LOW) {

    //reset settings - for testing
    wm.resetSettings();

    // set configportal timeout
    wm.setConfigPortalTimeout(WM_TIMEOUT);

    if (!wm.startConfigPortal(NAME_AP)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    //Just incase berjaya startConfigPortal
    kLedInterval = 250;
  }
  return kLedInterval;
}

//6.4 - Blink Built-in LED at pin D2
void Led_blink(uint16_t interval) {
  static uint32_t kTick;
  if (kTick < millis()) {
    kTick = millis() + interval;
    digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
  }
}
