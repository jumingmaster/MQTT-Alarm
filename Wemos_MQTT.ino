#include <DS18B20.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define D0 3
#define RX 3
#define D1 1
#define TX 1
#define D2 16
#define D3 5
#define SCL 5
#define D4 4
#define SDA 4
#define D5 14
#define SCK 14
#define D6 12
#define MISO 12
#define D7 13
#define MOSI 13
#define D8 0
#define D9 2
#define TX1 2
#define D10 15
#define D11 13
#define D12 12
#define D13 14
#define D14 4
#define D15 5
#define SS 15
#define A0 17
#define MQ2 D2
#define Relay D10
const char* ssid = "Fatfatpig";               //WIFI-SSID
const char* password = "h990611.";            //WIFI-PASSWD
const char* mqttServer = "192.168.1.101";     //Host
const long mqttPort = 1883;                    //PORT
const char* TOPIC = "一号机";                   //TOPIC
const char* client_id = "Wemos";              //DEVICE-ID

WiFiClient espClient;                         //声明类WIFIClien的对象，该对象允许建立到特点ip和端口的连接
PubSubClient client(espClient);               //声明类PubSubClient的对象，接受先前定义的WiFiClient 作为构造函数的输入
DS18B20 ds(D14);                              //声明类DS18B20的对象，初始化D14引脚

/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }//串口打印出收到的信息
  Serial.println();

  // 如果收到1作为第一个字符，则打开LED
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // 这里他说接受的‘1’就打开灯 但是我在用的时候 接收到0才会打开  这一行的‘LOW’和下面的‘HIGH’应该换下位置，下面也说了 ESP-01是这样的
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
*/

void reconnect() 
{
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(client_id)) 
    {
      Serial.println("connected");
      
        
    } 
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  // put your setup code here, to run once:
     Serial.begin(115200);                  //设置串口频率
     pinMode(D14,INPUT);
     pinMode(MQ2,INPUT);
     pinMode(A0,INPUT);
     pinMode(Relay,OUTPUT);
     pinMode(LED_BUILTIN,OUTPUT);
     digitalWrite(Relay,HIGH);
     
     WiFi.begin(ssid, password);            //连接WIFI
     while (WiFi.status() != WL_CONNECTED) 
     {
          delay(500);
          Serial.print("Connecting to WiFi…");
     }
     Serial.println("Connected to the WiFi network");
     
     client.setServer(mqttServer, mqttPort);    //指定MQTT服务器的IP和端口


     while (!client.connected())              //连接MQTT
     {
     Serial.println("Connecting to MQTT...");
 
     if (client.connect("ESP8266Client"))
     {
         Serial.println("connected");  
   
     }
     else
     {
   
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
   
     }

    
    }
}
void loop()
{
  // put your main code here, to run repeatedly:
     char msg[10] = {};
     if (!client.connected()) 
     {
        reconnect();
     }
     client.loop();
     
     int mq2_flag = digitalRead(MQ2);
     int mq2ppm = 9.67 * analogRead(A0);
     double tempc;
     
     if(mq2_flag == 0)
     {
        
          digitalWrite(Relay, LOW);
     }  
        
     else
          digitalWrite(Relay, HIGH);
        
    
     
     while (ds.selectNext())
     {
        tempc = ds.getTempC();
        snprintf (msg, sizeof(float) + sizeof(int),"%d %.2f",mq2ppm,tempc);
     }

      
     Serial.print("温度：");
     Serial.print(tempc);
     Serial.println("℃");
     Serial.print("烟雾浓度：");
     Serial.print(mq2ppm);
     Serial.println("PPM");
     
     client.publish(TOPIC,msg);
     delay(5000);
}
