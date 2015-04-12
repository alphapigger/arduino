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
int len = 0 ;

void setup() {
    Serial.begin(SRate);
    while(!Serial);
    Serial.println("Serial is ok!");
    sSerial.begin(SSRate);
    Serial.println("sSerial is ok!");
    pinMode(LED_PIN, OUTPUT);
    delay(5000);
    register_device();
}

void loop() {
    if(sSerial.available()){
        unsigned char count = sSerial.readBytesUntil(END_FLAG, rx_data, 90);
        rx_msg = unpack_msg(rx_data, count);
        Serial.println("Receive msg: " + rx_msg);
        handle_msg(rx_msg);
        rx_msg = "";
    }

    if (Serial.available()){

    }
}

/**
**
*/
void handle_msg(String msg){
    if(msg == String("light on")){
        digitalWrite(LED_PIN, HIGH);
    }else if(msg==String("light off")){
        digitalWrite(LED_PIN, LOW);
    }else if(msg==String("humtem")){
        unsigned char humtem[2];
        humtem = get_hum_tem();
        String msg = String(DEVICE_ID) + " " + String(humtem[0]) + " "+ String(humtem[1]);
        send_msg(msg);
    }
}


unsigned char * get_hum_tem(){
    unsigned char res[2];
    unsigned char chk = DHT.read11(DHT11_PIN);
    switch (chk) {
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
            Serial.print("Unkown error, \t");
    }
    unsigned char humidity = DHT.humidity;
    unsigned char temperature = DHT.temperature;
    res[0] = humidity;
    res[1] = temperature;
    Serial.print(DHT.humidity, 1);
    Serial.println(DHT.temperature, 1);
    return res;
}


void send_msg(String msg){
    len = msg.length();
    pack_msg(msg, tx_data, len);
    sSerial.write(tx_data, len+1);
    Serial.println("Send msg: " + msg);
}


/**
** 向控制中心注册设备
*/
void register_device(){
    String msg = String("register ") + String(DEVICE_ID) + " " + String(DEVICE_TYPE);
    send_msg(msg);
}

/**
** 封装将要发送的消息，在消息末尾添加\n
** @param msg  将要发送的消息
*/
void pack_msg(String msg, char *msg_arr, unsigned char n){
    for(char i=0; i < n; i++){
        msg_arr[i] = msg.charAt(i);
    }
    msg_arr[n] = NL;  //添加换行符
}


String unpack_msg(unsigned char * msg, unsigned char n){
    String msg_str = "";
    for(unsigned char i=0; i<n-1; i++){  // 去除结束符
        msg[i] -= 128;
        msg_str += char(msg[i]);
    }
    return msg_str;
}
