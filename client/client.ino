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

// sensor config
//3 HUM_TEM   4 LED
#define SENSOR_ID 1  // hum&tem
#define SENSOR_TYPE 3
#define SENSOR_1_ID 2   // led
#define SNESOR_1_TYPE 4

#define REGISTER_CMD 1
#define DATA_CMD 2

SoftwareSerial sSerial(RxD, TxD);
dht DHT;

unsigned char rx_data[90];
char tx_data[90];
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
        unpack_msg(rx_data, count);
        Serial.println("Receive msg: " + String((char *)rx_data));
        handle_msg((char *)rx_data);
    }

    if (Serial.available()){
        Serial.println(Serial.read());
    }
}

/**
** 向控制中心注册设备
*/
void register_device(){
    //首次信息存在干扰，需要发送冗余信息
    String msg = String(0) + " " + String(0) + " " + String(0);
    send_msg(msg);
    delay(1000);
    msg = String(REGISTER_CMD) + " " + String(SENSOR_ID) + " " + String(SENSOR_TYPE);
    send_msg(msg);
    delay(1000);
    msg = String(REGISTER_CMD) + " " + String(SENSOR_1_ID) + " " + String(SNESOR_1_TYPE);
    send_msg(msg);
}

/**
**处理控制中心发送过来的命令
*/
void handle_msg(char *msg){
    char *valPosition = strtok(msg, " ");  // msg sensor_id sensor_value
    int data[4];
    char i = 0;
    while(valPosition!=NULL){
        data[i] = int(*valPosition) - 48;
        valPosition = strtok(NULL, " ");
        i = i + 1 ;
    }

    if (data[0] == SENSOR_1_ID){
      Serial.println("led");
        if(int(data[1]) > 0){
            digitalWrite(LED_PIN, HIGH);
        }else{
            digitalWrite(LED_PIN, LOW);
        }
    }else if( data[0] == SENSOR_ID){
        //向控制中心上传数据
        unsigned char humtem[2];
        get_hum_tem(humtem, 2);
        String msg = String(DATA_CMD) + " " + String(SENSOR_ID) + " " + String(SENSOR_TYPE) + " " + String(humtem[1]) + "," + String(humtem[0]);
        send_msg(msg);
    }
}

/**
** 获取温度湿度
*/
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

/**
** 向控制中心发送指令
*/
void send_msg(String msg){
    len = msg.length();
    pack_msg(msg, tx_data, len);
    sSerial.write(tx_data, len+1);
    Serial.println("Send msg: " + msg);
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


void unpack_msg(unsigned char * msg, unsigned char n){
    for(unsigned char i=0; i<n-1; i++){  // 去除结束符
        msg[i] -= 128;
    }
}
