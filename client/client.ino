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
#define SENSOR_ID 1
#define SENSOR_TYPE 3   // 1: hum&tem   2: light   3: hum&tem&light

#define REGISTER_CMD 1
#define HUMTEM_CMD 2


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
        Serial.println(Serial.read())
    }
}

/**
**接收控制中心发送过来的命令
**暂时只有控制电灯的cmd
*/
void handle_msg(String msg){
    Serial.println("Receive msg: " + msg);
    char *valPosition = strtok(msg, " ");
    while(valPosition!=NULL){
        Serial.println(valPosition);
        valPosition = strtok(NULL, " ");
    }
}


void get_hum_tem(unsigned char * humtem, unsigned char n){
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
            Serial.print("Unknown error, \t");
    }
    if(chk == 0){
        humtem[0] = DHT.humidity;
        humtem[1] = DHT.temperature;
    }else{
        humtem[0] = 0;
        humtem[1] = 0;
    }
    Serial.print(DHT.humidity, 1);
    Serial.println(DHT.temperature, 1);
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
    String msg = String(REGISTER_CMD) + " " String(SENSOR_ID) + " " + String(SENSOR_TYPE);
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
