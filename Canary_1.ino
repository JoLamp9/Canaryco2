
/*Canary house co2 project using ardiuno nano.
motor is 28byj
sensor is scd40*/




#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

#define STEPPER_PIN_1 9
#define STEPPER_PIN_2 10
#define STEPPER_PIN_3 11
#define STEPPER_PIN_4 12
#define RESETBUTT 5

SensirionI2cScd4x sensor;
bool co2High = false;
int above1k = 0;
int step_number = 0;
int steps = 0;
bool forward = true;
bool deadBird = false;
int co2SR = 0;

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);
    Wire.begin();
    sensor.begin(Wire, SCD41_I2C_ADDR_62);


}

void loop() {

    uint16_t co2Concentration = 0;
    float temperature = 0.0;
    float relativeHumidity = 0.0;
    delay(2);
    co2SR++;
    if(co2SR >=2500){// Slow down the sampling to 0.2Hz. 
        sensor.readMeasurement(co2Concentration, temperature, relativeHumidity); //reading temperature and relativeHumidity for potential future usage
        Serial.print("CO2 concentration [ppm]: ");
        Serial.print(co2Concentration);
        Serial.print(" above 1k: ");
        Serial.print(above1k);
        Serial.print(" c02High: ");
        Serial.print(co2High);
        Serial.println();
        co2SR = 0;


        if (co2Concentration >= 1000){ // high condition, if this is seen for 2 readings (above1k >= 2) then sensor reads as high, intention is change 2 to be more like 120 (10mins)
            Serial.println("above");
            above1k++;
            if (above1k >= 2){ 
                co2High = true;
            }
        } else if (co2Concentration <= 800){ // low condition, once windows have been opened co2 should drop.
            Serial.println("below");
            above1k = 0;
            co2High = false;
        }
    }
    


    if(co2High != deadBird){  // movement condition, deadBird should match the co2 level.
        if(steps < 1024){ // motor is 2048 fullsteps for full rotation
            OneStep(deadBird);
            steps++;
        }else {
            delay(500);
            steps = 0;
            deadBird = !deadBird;
        }
    } 


}

void OneStep(bool dir){ //class writen to move the motor using a fullstep pattern, in either direction. fullstep chosen over half, for speed/torque reasons.
  if(dir){
    switch(step_number){
      case 0:
      digitalWrite(STEPPER_PIN_1, HIGH);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 1:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, HIGH);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 2:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, HIGH);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 3:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, HIGH);
      break;
    } 
  }else{
    switch(step_number){
      case 0:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, HIGH);
      break;
      case 1:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, HIGH);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 2:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, HIGH);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 3:
      digitalWrite(STEPPER_PIN_1, HIGH);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
    } 
  }
  step_number++;
  if(step_number > 3){
    step_number = 0;
  }
}

