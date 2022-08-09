/*  Made by: Guilherme Oliveira Rolim Silva
 *  Date: 08/2022
 *  
 *  Sends Springer Silentia air conditioners remote IR codes according to pre-established presets.
 *  By default, the Timer signal is sent, but if received a specific NEC command from a regular 
 *  IR remote, changes the default signal to be sent and starts sending another code equivalent 
 *  to the new button. Also sends a 0x000000 code every 350ms after sending a code because of possible 
 *  air conditioners receiving problems.
 *  
 *  Made based on:
 *  SimpleSender from IRremote - https://github.com/Arduino-IRremote/Arduino-IRremote
 *  
 *  More info can be found at: https://github.com/rolim520/Springer-Silentia-IR-remote
 */
#include <Arduino.h>

//#define SEND_PWM_BY_TIMER
//#define USE_NO_SEND_PWM
#define NO_LED_FEEDBACK_CODE // saves 418 bytes program memory

uint32_t Code = 0x27D088;
uint32_t Blank = 0x000000;

unsigned long tempoTimer = millis();
unsigned long tempoBlank = millis();
unsigned long tempoDecoder = millis();

bool readyTimer = false;
bool readyBlank = false;
bool readyDecoder = false;

bool requestBlank = false;
bool requestDecoder = false;

void delayChecker();
void sendCode();
void blinkLED();

#include "PinDefinitionsAndMore.h" //Define macros for input and output pin etc.
#include <IRremote.hpp>

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(13, OUTPUT);

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    IrSender.begin();// Start with IR_SEND_PIN as send pin and if NO_LED_FEEDBACK_CODE is NOT defined, enable feedback LED at default feedback LED pin
    Serial.print(F("Ready to send IR signals at pin "));
    Serial.println(IR_SEND_PIN);

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
    Serial.println();
}

void loop() {

    // Sending the recieved code every second
    if (readyTimer) {
        sendCode(Code);
        Serial.println();
        Serial.println();
        tempoBlank = millis();
        requestBlank = true;
        tempoTimer = millis();
        readyTimer = false;
    }
    
    if (readyBlank) {
        sendCode(Blank);
        Serial.println();
        Serial.println();
        readyBlank = false;
        requestBlank = false;
    }

    //Accepting NEC commands from the IR remote
    if (IrReceiver.decode()) {

        if (!requestDecoder){
            tempoDecoder = millis();
            requestDecoder = true;
            if (IrReceiver.decodedIRData.protocol == NEC) {
                IrReceiver.printIRResultShort(&Serial);
                Serial.println();
            }
        }

        if (IrReceiver.decodedIRData.protocol == NEC) {

            //Relation between recieved signals and sent ones
            switch (IrReceiver.decodedIRData.command) {
                case 0x46:
                    Code = 0x73808C;
                    break;
                            
                case 0x44:
                    Code = 0x1BE084;
                    break;
                            
                case 0x40:
                    Code = 0xE71088;
                    break;
                
                case 0x15:
                    Code = 0xA3508C;
                    break;
                        
                case 0x7:
                    Code = 0x47B088;
                    break;
                        
                case 0x43:
                    Code = 0x27D088;
                    break;
                        
                case 0xD:
                    Code = 0xCB3084;
                    break;
                        
                case 0x5E:
                    Code = 0x3F08C;
                    break;
                        
                case 0x19:
                    Code = 0x8B7084;
                    break;
                
                case 0x16:
                    Code = 0xF30884;
                    break;
                
                case 0x18:
                    Code = 0x738884;
                    break;
                
                case 0xC:
                    Code = 0x3DC88C;
                    break;
            }
            blinkLED(13, 1, 100); 
        }
            
        if (readyDecoder){
            IrReceiver.resume(); // Enable receiving of the next value
            readyDecoder = false;
            requestDecoder = false;
        }
    }
    delayChecker();
}

void blinkLED(int pin, int Times, int time){
  for (int i=1; i<= Times; i++){
    digitalWrite(pin, HIGH);
    delay(time);
    digitalWrite(pin, LOW);
    delay(time);
  }
}

void sendCode(uint32_t tRawData){
    Serial.print(F("Sending now: "));
    if (tRawData != 0x000000){
        switch (tRawData) {
            case 0x73808C:
                Serial.print(F("Power "));
                break;
                
            case 0x1BE084:
                Serial.print(F("Sweep "));
                break;
                
            case 0xE71088:
                Serial.print(F("Turbo "));
                break;
    
            case 0xA3508C:
                Serial.print(F("Temp(+) "));
                break;
            
            case 0x47B088:
                Serial.print(F("Temp(-) "));
                break;
            
            case 0x27D088:
                Serial.print(F("Timer "));
                break;
            
            case 0xCB3084:
                Serial.print(F("Cool "));
                break;
            
            case 0x3F08C:
                Serial.print(F("Fan "));
                break;
            
            case 0x8B7084:
                Serial.print(F("Dry "));
                break;
    
            case 0xF30884:
                Serial.print(F("FanSpeed "));
                break;
    
            case 0x738884:
                Serial.print(F("Sleep "));
                break;
    
            case 0x3DC88C:
                Serial.print(F("EnergySaving "));
                break;
        }
        Serial.print(F("Button --> Code=0x"));
        
    } else if (tRawData == 0x000000){
        Serial.print(F("Blank Code --> Code=0x"));
    }
    Serial.print(tRawData, HEX);
    IrSender.sendPulseDistance(8300, 4150, 550, 1550, 550, 500, &tRawData, 24, false, 0, 0);
    Serial.flush();
}

void delayChecker(){
  
  if(!readyTimer && millis() - tempoTimer >= 700){
    readyTimer = true;
  }
  if(requestBlank && !readyBlank && millis() - tempoBlank >= 350){
    readyBlank = true;
  }
  if(requestDecoder && !readyDecoder && millis() - tempoDecoder >= 200){
    readyDecoder = true;
  }
}
