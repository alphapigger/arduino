#include <dht.h>
dht DHT;

#define DHT11_PIN 4
#define LED_PIN 13


void setup(){
    // init
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
    Serial.println("A: turn on light");
    Serial.println("B: turn off light");
    Serial.println("C: get humidity and temperature");
    Serial.println("Enter your choice: ");
}


void loop(){
    if (Serial.available() > 0){
        char choice = Serial.read();
        switch(choice){
            case 'A':
                switchLight(1);
                break;
            case 'B':
                switchLight(0);
                break;
            case 'C':
                showHumTem();
                break;
            default:
                Serial.println("Unknown choice");
                break;
        }
    }
}


void showHumTem(){
    Serial.println("Type, \tstatus, \tHumidity (%), \tTemperature (C)");
    int chk = DHT.read11(DHT11_PIN);

    switch(chk){
        case 0:
            Serial.print("OK, \t");
            break;
        case -1:
            Serial.print("Checksum error, \t");
            break;
        case -2:
            Serial.print("Time out error, \t");
            break;
        default:
            Serial.print("Unknown error, \t");
            break;
    }

    Serial.print(DHT.humidity);
    Serial.print(", \t");
    Serial.println(DHT.temperature);
}


void switchLight(int signal){
    if (signal == 1){
        digitalWrite(LED_PIN, HIGH);
    }else{
        digitalWrite(LED_PIN, LOW);
    }
}




