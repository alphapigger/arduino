// client.ino
#include <SoftwareSerial.h>
#include <dht.h>

#define RxD 2
#define TxD 3
#define LED_PIN 13
#define DHT11_PIN 4
#define SRate 9600
#define SSRate 38400
#define END_FLAG 220
#define NL 10
#define DELTA 128

//device config
#define DEVICE_ID 1
#define DEVICE_TYPE 3   // 1: hum&tem   2: light   3: hum&tem&light


SoftwareSerial sSerial(RxD, TxD);
dht DHT;

unsigned char rx_data[90];
char tx_data[90];
String rx_msg = "";
String tx_msg = "";
int length = 0 ;

void setup() {
    Serial.begin(SRate);
    while(!Serial);
    Serial.println('Serial is ok!');
    sSerial.begin(SSRate);
    Serial.println('sSerial is ok!');
    pinMode(LED_PIN, OUTPUT);
    delay(5000);
    register_device();
}

void loop() {
    if(sSerial.available()){
        unsigned char count = sSerial.readBytesUntil(END_FLAG, rx_data, 90);
        rx_msg = unpack_msg(rx_data, count);
        Serial.println("Receive msg: " + rx_msg);
        handle_msg(tx_msg);
        rx_msg = "";
    }

    if (Serial.available()){

    }
}

/**
**
*/
void handle_msg(String msg){
    if(msg == "light on"){
        digitalWrite(LED_PIN, HIGH);
    }else if(msg == "light off"){
        digitalWrite(LED_PIN, LOW);
    }else if(msg == "humtem"){
        int chk = DHT.read11(DHT11_PIN);
        unsigned char hum = DHT.humidity;
        unsigned char tem = DHT.temperature;
        tx_msg = "";
        tx_msg = String(DEVICE_ID) + " " +String(hum) + " "+ String(tem);
        len = tx_msg.length();
        pack_msg(tx_msg, tx_data, len);
        sSerial.write(tx_data, len+1);
        Serial.println("Send msg: " + tx_msg);
        tx_msg = "";
    }
}



/**
** 向控制中心注册设备
*/
void register_device(){
    tx_msg = "";
    tx_msg + = "register " + String(DEVICE_ID) + " "+ String(DEVICE_TYPE);
    len = tx_msg.length();
    pack_msg(text_msg, tx_data, len);
    sSerial.write(tx_data, len+1);
    Serial.println("Send msg: " + tx_msg);
    tx_msg = "";
}

/**
** 封装将要发送的消息，在消息末尾添加\n
** @param msg  将要发送的消息
*/
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
