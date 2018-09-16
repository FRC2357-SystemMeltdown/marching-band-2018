#include <RCSwitch.h>
#include <Servo.h>

#define PIN_RX 0 // Interrupt #0, which is pin D2
#define PIN_SERVO_1 11
#define PIN_SERVO_2 12

#define RX_NONE     0
#define RX_ACTIVE   B101010
#define RX_LAUNCH   B010101
#define RX_RETRACT  B001010

typedef enum State {
  disabled, enabled, launching, retracting
};

#define LOOP_DELAY         100
#define RX_MESSAGE_TIMEOUT 1000

Servo servo1;
Servo servo2;
RCSwitch receiver = RCSwitch();
int command = RX_NONE;
State currentState = RX_NONE;
unsigned long lastMessageMillis = -1;

void setup() {
  Serial.begin(9600);
  Serial.println("Receiver starting up...");

  setupServos();
  setupReceiver();
  Serial.println("Receiver ready!");
}

void loop() {
  readReceiver();
  checkTimeout();
  updateStateLED();
  updateMotors();
  delay(LOOP_DELAY);
}

void updateStateLED() {
  int value = LOW;
  
  switch(currentState) {
    case enabled:
      value = HIGH;
      break;
    case launching:
      value = (millis() % 250 >= 125 ? HIGH : LOW);
      break;
    case retracting:
      value = (millis() % 1000 >= 500 ? HIGH : LOW);
      break;
    case disabled:
      value = LOW;
      break;
  }

  digitalWrite(LED_BUILTIN, value);
}

void updateMotors() {
  int value = 90; // TODO: update with real values.
    
  switch(currentState) {
    case launching:
      value = 140;  // TODO: Update with real values.
      break;
    case retracting:
      value = 40; // TODO: Update with real values.
      break;
  }

  Serial.print("Servo: ");
  Serial.println(value);
  servo1.write(value);
  servo2.write(value);
}

void setCommand(int newCommand) {
  if (newCommand != command) {
    command = newCommand;
    
    switch(command) {
      case RX_ACTIVE:
        setEnabled();
        break;
      case RX_LAUNCH:
        setLaunch();
        break;
      case RX_RETRACT:
        setRetract();
        break;
      default:
        setDisabled();
        break;
    }
  }  
}

void setEnabled() {
  currentState = enabled;
  Serial.println("[state]: enabled");
}

void setDisabled() {
  currentState = disabled;
  Serial.println("[state]: disabled");
}

void setLaunch() {
  currentState = launching;
}

void setRetract() {
  currentState = retracting;
}

void readReceiver() {
  // If data is available from the receiver, read it.
  if (receiver.available()) {
    int value = receiver.getReceivedValue();
    lastMessageMillis = millis();
    setCommand(value);
  }
  receiver.resetAvailable();
}

void checkTimeout() {
  if (isTimedOut()) {
    setCommand(RX_NONE);
  }
}

bool isTimedOut() {
  if (lastMessageMillis == -1) {
    // We haven't received any messages yet.
    return true;
  }

  // Check to see if we've received a message recently.
  unsigned long timeDiff = millis() - lastMessageMillis;
  return (timeDiff > RX_MESSAGE_TIMEOUT);
}

void setupServos() {
  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
}

void setupReceiver() {
  receiver.enableReceive(PIN_RX);
}

