#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <bluefruit.h>

/*
 * Team 6 "Super Six" - 4 Motors on Adafruit Bluefruit Feather nRF52 model with DC Motor/Stepper FeatherWing
 * Author: Courtney Yovich
 * Reads from Bluetooth app controller and allows for skid steering with control of left motors, right motors, or all motors at once.
 * Increments and decrements speed by 15.
 * 
 */

BLEUart bleuart;
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

//motors
Adafruit_DCMotor *leftMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *leftMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *rightMotor1 = AFMS.getMotor(3);
Adafruit_DCMotor *rightMotor2 = AFMS.getMotor(4);


// Function prototypes for packetparser.cpp
uint8_t readPacket (BLEUart *ble_uart, uint16_t timeout);
float   parsefloat (uint8_t *buffer);
void    printHex   (const uint8_t * data, const uint32_t numBytes);

// Packet buffer
extern uint8_t packetbuffer[];

//Speed of vehicle to be modified by control slider
uint8_t speed = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Bluefruit Connect Motor Test");

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("TEAM6Bluefruit52");

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // Set the speed to start, from 0 (off) to 255 (max speed)
  leftMotor1->setSpeed(0);
  leftMotor2->setSpeed(0);
  rightMotor1->setSpeed(0);
  rightMotor2->setSpeed(0);
  leftMotor1->run(FORWARD);
  leftMotor2->run(FORWARD);
  rightMotor1->run(FORWARD);
  rightMotor2->run(FORWARD);
  // turn on motor
  leftMotor1->run(RELEASE);
  leftMotor2->run(RELEASE);
  rightMotor1->run(RELEASE);
  rightMotor2->run(RELEASE);

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println();
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() {
  uint8_t len = readPacket(&bleuart, 500);
  if (len == 0) return;

  // reads in button presses from controller
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    switch (buttnum) {
      case 1: //left motors forward
        if (pressed) {
          leftMotor1->run(FORWARD);
          leftMotor2 -> run(FORWARD);
          leftMotor1 -> setSpeed(speed);
          leftMotor2 -> setSpeed(speed);
        } else {
          leftMotor1 ->setSpeed(0);
          leftMotor2 ->setSpeed(0);
        }
        break;
      case 2: //all motors forward
        if (pressed) {
          leftMotor1->run(FORWARD);
          leftMotor2 -> run(FORWARD);
          rightMotor1 ->run(FORWARD);
          rightMotor2 ->run(FORWARD);
          leftMotor1 -> setSpeed(speed);
          leftMotor2 -> setSpeed(speed);
          rightMotor1 -> setSpeed(speed);
          rightMotor2 -> setSpeed(speed);
        } else {
          leftMotor1 ->setSpeed(0);
          leftMotor2 ->setSpeed(0);
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
        }
        break;
      case 3: //right motors forward
        if (pressed) {
          rightMotor1 ->run(FORWARD);
          rightMotor2 ->run(FORWARD);
          rightMotor1 -> setSpeed(speed);
          rightMotor2 -> setSpeed(speed);
        } else {
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
        }
        break;
      case 4: //left motors reverse
        if (pressed) {
          leftMotor1->run(BACKWARD);
          leftMotor2 -> run(BACKWARD);
          leftMotor1 -> setSpeed(speed);
          leftMotor2 -> setSpeed(speed);
        } else {
          leftMotor1 ->setSpeed(0);
          leftMotor2 ->setSpeed(0);
        }
        break;
      case 5: //all motors reverse
        if (pressed) {
          leftMotor1->run(BACKWARD);
          leftMotor2 -> run(BACKWARD);
          rightMotor1 ->run(BACKWARD);
          rightMotor2 ->run(BACKWARD);
          leftMotor1 -> setSpeed(speed);
          leftMotor2 -> setSpeed(speed);
          rightMotor1 -> setSpeed(speed);
          rightMotor2 -> setSpeed(speed);
        } else {
          leftMotor1 ->setSpeed(0);
          leftMotor2 ->setSpeed(0);
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
        }
        break;
      case 6: //right motors reverse
        if (pressed) {
          rightMotor1 ->run(BACKWARD);
          rightMotor2 ->run(BACKWARD);
          rightMotor1 -> setSpeed(speed);
          rightMotor2 -> setSpeed(speed);
        } else {
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
        }
        break;
      case 7: //speed +15
        if (pressed) {
          speed += 15;
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
          leftMotor1 -> setSpeed(0);
          leftMotor2 -> setSpeed(0);
        }
        break;
      case 8: //speed -15
        if (pressed) {
          speed -= 15;
          rightMotor1 -> setSpeed(0);
          rightMotor2 -> setSpeed(0);
          leftMotor1 -> setSpeed(0);
          leftMotor2 -> setSpeed(0);
        }
        break;
    }
  }
}

