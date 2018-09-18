#include <RCSwitch.h>

#define PIN_ENABLE 2
#define PIN_LAUNCH 3
#define PIN_RETRACT 4
#define PIN_TX 10

#define LOOP_DELAY 250

#define TX_ACTIVE   "101010"
#define TX_LAUNCH   "010101"
#define TX_RETRACT  "001010"

RCSwitch transmitter = RCSwitch();

void setup() {
  Serial.begin(9600);

  setupTransmitter();
  setupButtons();
}

void loop() {
  int enabled = digitalRead(PIN_ENABLE) == LOW;
  int launch = digitalRead(PIN_LAUNCH) == LOW;
  int retract = digitalRead(PIN_RETRACT) == LOW;

  printStatus(enabled, launch, retract);
  
  updateStatusLED(enabled);
  
  if (enabled) {
    transmitMessage(launch, retract);
  } else {
    delay(LOOP_DELAY);
  }
}

void printStatus(int enabled, int launch, int retract) {
  Serial.print("enabled:");
  Serial.print(enabled);
  Serial.print(" launch:");
  Serial.print(launch);
  Serial.print(" retract:");
  Serial.print(retract);
  Serial.println();
}

void updateStatusLED(int enabled) {
  digitalWrite(LED_BUILTIN, enabled);
}

void transmitMessage(int launchPressed, int retractPressed) {
  int message = TX_ACTIVE;
  
  if (launchPressed) {
    message = TX_LAUNCH;
  }
  if (retractPressed) {
    message = TX_RETRACT;
  }
  
  transmitter.send(message);
}

void setupTransmitter() {
  transmitter.enableTransmit(PIN_TX);
}

void setupButtons() {
  // Set button pins to digital input.
  pinMode(PIN_ENABLE, INPUT);
  pinMode(PIN_LAUNCH, INPUT);
  pinMode(PIN_RETRACT, INPUT);

  // Set pull-up resistors.
  digitalWrite(PIN_ENABLE, HIGH);
  digitalWrite(PIN_LAUNCH, HIGH);
  digitalWrite(PIN_RETRACT, HIGH);
}

