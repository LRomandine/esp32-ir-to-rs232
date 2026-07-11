/*
 * sony-IR-to-RS232.ino
 *
 * All the details on gitHub
 * https://github.com/LRomandine/esp32-ir-to-rs232/tree/main
 *
 * Modified from:
 *   https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/SmartIRRepeater/SmartIRRepeater.ino
 */

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <HardwareSerial.h>

// ==================== start of TUNEABLE PARAMETERS ====================

// The GPIO an IR detector/demodulator is connected to. Recommended: 14 (D5)
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
const uint16_t kRecvPin = 36;

// Initialize Serial2 for the ESP32
HardwareSerial TVSerial(2);

// The Serial connection baud rate.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
const uint16_t kCaptureBufferSize = 1024;  // 1024 == ~511 bits

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all UNKNOWN messages will be sent at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

// ==================== end of TUNEABLE PARAMETERS ====================

// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
// Somewhere to store the captured message.
decode_results results;

void setup() {
  irrecv.enableIRIn();  // Start up the IR receiver.

  Serial.begin(kBaudRate, SERIAL_8N1);
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();

  Serial.print("SmartIRRepeater is now running and waiting for IR input on Pin ");
  Serial.println(kRecvPin);

  // Start TV Serial for SONY
  // Configured as 9600 baud, 8 data bits, 1 stop bit, no parity (8N1)
  // https://pro-bravia.sony.net/remote-display-control/serial-control/#accordion_4
  TVSerial.begin(9600, SERIAL_8N1, 16, 17);
  
}

void loop() {
  if (irrecv.decode(&results)) {
    decode_type_t protocol = results.decode_type;
    uint16_t size = results.bits;
    bool success = true;
    if (protocol == decode_type_t::SONY) {
      // Serial.println(resultToHumanReadableBasic(&results));
      // Command reference for "pro displays" https://pro-bravia.sony.net/remote-display-control/serial-control/command/
      // Different command reference everyone says works https://justaddpower.happyfox.com/kb/article/22-sony-rs232-control/
      byte rs232Command[] = {};
      switch (results.command) {
        case 21: {
          Serial.println("Received 'Power' (different from 'Power On' and 'Power Off')");
          Serial.println("We cannot do anything with this, ¯\_(ツ)_/¯");
          Serial.print("Command : ");
          Serial.println(String(results.command));
          break;
        }
        case 35: {
          Serial.println("Received 'Back (35)'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x97, 0x23, 0xB0}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 37: {
          Serial.println("Received 'Input'");
          Serial.println("Normally we cannot do anything but we will enable 'standby mode' for RS-232C Communication.");
          byte rs232Command[] = {0x8C, 0x00, 0x01, 0x02, 0x01, 0x90}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 46: {
          Serial.println("Received 'Power On' (different from 'Power' and 'Power Off')");
          byte rs232Command[] = {0x8C, 0x00, 0x00, 0x02, 0x01, 0x8F}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 47: {
          Serial.println("Received 'Power Off' (different from 'Power On' and 'Power')");
          byte rs232Command[] = {0x8C, 0x00, 0x00, 0x02, 0x00, 0x8E}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 51: {
          Serial.println("Received 'Arrow Right'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x33, 0x2A}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 52: {
          Serial.println("Received 'Arrow Left'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x34, 0x2B}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 58: {
          Serial.println("Received 'Info (Display)'");
          byte rs232Command[] = {0x8C, 0x00, 0x0F, 0x02, 0x00, 0x9D}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 59: {
          Serial.println("Received 'Back (59)'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x97, 0x23, 0xB0}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 75: {
          Serial.println("Received 'Action Menu'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x97, 0x36, 0xC3}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 90: {
          Serial.println("Received 'HDMI 1'");
          byte rs232Command[] = {0x8C, 0x00, 0x02, 0x03, 0x04, 0x01, 0x96}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 91: {
          Serial.println("Received 'HDMI 2'");
          byte rs232Command[] = {0x8C, 0x00, 0x02, 0x03, 0x04, 0x02, 0x97}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 92: {
          Serial.println("Received 'HDMI 3 (ARC)'");
          byte rs232Command[] = {0x8C, 0x00, 0x02, 0x03, 0x04, 0x03, 0x98}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 93: {
          Serial.println("Received 'HDMI 4'");
          byte rs232Command[] = {0x8C, 0x00, 0x02, 0x03, 0x04, 0x04, 0x99}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 96: {
          Serial.println("Received 'Home'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x60, 0x57}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 101: {
          Serial.println("Received 'Enter (OK)'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x65, 0x5C}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 116: {
          Serial.println("Received 'Arrow Up'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x74, 0x6B}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        case 117: {
          Serial.println("Received 'Arrow Down'");
          byte rs232Command[] = {0x8C, 0x00, 0x67, 0x03, 0x01, 0x75, 0x6C}; 
          TVSerial.write(rs232Command, sizeof(rs232Command));
          break;
        }
        default: {
          Serial.println("We cannot do anything with this, ¯\_(ツ)_/¯");
          Serial.print("Command : ");
          Serial.println(String(results.command));
          break;
        }
      }
    }
    irrecv.resume();
  }
  yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
}

