#include <SoftwareSerial.h>
#define RxD 2
#define TxD 3
#define LedPin 13
#define SerialRate 9600
#define ZGSerialRate 38400

SoftwareSerial ZGSerial(RxD, TxD);


void setup(){
    Serial.begin(SerialRate);
    while(!Serial);
    Serial.println("Serial is ok!");
    ZGSerial.begin(ZGSerialRate);
    Serial.println("ZGSerial is ok!");
    pinMode(LedPin, OUTPUT);
}


void loop(){
    if(ZGSerial.available()){
        int val = ZGSerial.read();
        Serial.println(val);
        // digitalWrite(LedPin, HIGH);
        // delay(5000);
        // digitalWrite(LedPin, LOW);
    }

    if(Serial.available()){
        int val = Serial.read();
        ZGSerial.write(val);
    }
}



