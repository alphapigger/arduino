// zg_serial_1.ino
#include <SoftwareSerial.h>
#define RxD 2
#define TxD 3
#define LED_PIN 13
#define SRate 9600
#define SSRate 38400
#define END_FLAG 220
#define NL 10
#define DELTA 128


SoftwareSerial sSerial(RxD, TxD);
unsigned char rx_data[90];
char tx_data[90];
String rx_msg = "";

void setup() {
    Serial.begin(SRate);
    while(!Serial);
    Serial.println('Serial is ok!');
    sSerial.begin(SSRate);
    Serial.println('sSerial is ok!');
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    if(sSerial.available()){
        unsigned char count = sSerial.readBytesUntil(END_FLAG, rx_data, 90); // 以\结尾 发送过来的ascii相差128
        rx_msg = unpack_msg(rx_data, count);
        Serial.println("receive msg: " + rx_msg);
        rx_msg = "";
    }

    if (Serial.available()){
        String test = "abc";
        int len = test.length();
        pack_msg(test, tx_data, len);
        sSerial.write(tx_data, len+1);  //以换行符结尾
    }
}

void pack_msg(String msg, char *msg_arr, unsigned char n){
    for(char i=0; i < n; i++){
        msg_arr[i] = msg.charAt(i);
    }
    msg_arr[n] = NL;
}

String unpack_msg(unsigned char * msg, unsigned char n){
    String msg_str = "";
    for(unsigned char i=0; i<n; i++){
        msg[i] -= 128;
        msg_str += char(msg[i]);
    }
    return msg_str;
}




