#include<ESP8266WiFi.h>
#include<DallasTemperature.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

/******************mfrc522配置*********************/
#define RST_PIN 5 // 配置针脚
#define SS_PIN 4  
MFRC522 mfrc522(SS_PIN, RST_PIN); // 创建新的RFID实例
//使用union结构，合并4个vbyte数据，转换为1个long
union long_byte
{
  long long_data;
  byte byte_data[4];
};
long_byte lb;
long EEPROM_RFID1 = -1727906733;  //EEPROM中保存的管理员门禁卡ID1
long EEPROM_RFID2 = -1;           //EEPROM2-7中保存的用户门禁卡
long EEPROM_RFID3 = -1;
long EEPROM_RFID4 = -1;
long EEPROM_RFID5 = -1;
long EEPROM_RFID6 = -1;
long EEPROM_RFID7 = -1;
long read_RFID = -1;      //当前读取到的门禁卡ID
long last_RFID = -1;      //上一次读取到的门禁卡ID
int num ;

/*****************光敏模块***************************/
int sunflag = 0;
/******************TCPClient配置********************/
const char*ssid ="helloworld";
const char*password ="516qwert";
const char*serverIP ="192.168.123.188";
int serverPort =8080;
WiFiClient client;
bool bConnected =false;
char buff[512];
int nm =0;
int flag = 0;

/******************DS18B20配置***********************/
#define ONE_WIRE_BUS 2               //1-wire数据总线连接在IO2
OneWire oneWire(ONE_WIRE_BUS);       //声明
DallasTemperature sensors(&oneWire); //声明
unsigned long previousMillis = 0; //毫秒时间记录
const long interval = 2000;       //时间间隔

void setup(){
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  EEPROM.begin(1024);
  Serial.print("Connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
  sensors.begin(); //初始化总线
  sensors.setWaitForConversion(false); //设置为非阻塞模式
  eepromfind();
  IO_init(); 
  RFID_init();
}

void eepromfind() {
    //读取EEPROM索引的值            我这里设置了七张卡，不够也可以再加
  for (int i = 0; i < 4; i++)
  {
    lb.byte_data[i] = EEPROM.read(i);   //卡1
  }
  EEPROM_RFID1 = lb.long_data;
  
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+4);   //卡2
  }
  EEPROM_RFID2 = lb.long_data;
 
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+8);   //卡3
  }
  EEPROM_RFID3 = lb.long_data;
 
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+12);    //卡4
  }
  EEPROM_RFID4 = lb.long_data;
 
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+16);   //卡5
  }
  EEPROM_RFID5 = lb.long_data;
 
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+20);   //卡6
  }
  EEPROM_RFID6 = lb.long_data;
 
  for (int i = 0  ; i<4 ; i++ )
  {
    lb.byte_data[i] = EEPROM.read(i+24);    //卡7
  }
  EEPROM_RFID7 = lb.long_data;
 
  num = EEPROM.read(30);
}

void ds18b20(void) {
    //以下段落相当于每秒读取前次温度，并发起新一次温度转换
//  unsigned long currentMillis = millis();         //读取当前时间
//  if (currentMillis - previousMillis >= interval) //如果和前次时间大于等于时间间隔
//  {
//    previousMillis = currentMillis; //更新时间记录
//
//    float tempC = sensors.getTempCByIndex(0); //获取索引号0的传感器摄氏温度数据
//    if (tempC != DEVICE_DISCONNECTED_C)       //如果获取到的温度正常
//    {
//      Serial.print("\n当前温度是： ");
//      Serial.print(tempC);
//      Serial.println(" ℃");
//      if(tempC >= 20.0 && flag == 0){
//        client.print("CMD_Mot_101");       //WiFi发送电机启动指令
//        flag = 1;
//      }
//      else if(tempC < 20.0 && flag == 1){
//        client.print("CMD_Mot_100");       //WiFi发送电机关闭指令
//        flag = 0;
//      }
//    }
//    sensors.requestTemperatures();        //发起新的温度转换
//  }
//
//  delay(100);
}

void Btn_Event()
{
  EEPROM.begin(1024);
  delay(200);
  if (read_RFID == -1)
  {
    Serial.println("当前未读卡");
  }
  
  else{
    switch(num){
   
   case 0 :   
            lb.long_data = read_RFID;
            EEPROM_RFID1 = lb.long_data;
            for (int i = 0; i < 4; i++)
            {
              EEPROM.write(i, lb.byte_data[i]);
            }
            num = 1;
            EEPROM.write(30, num);//EEPROM第30个字节保存第一个空卡
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID1已保存");
            break;
  case 1 :
            lb.long_data = last_RFID;
            EEPROM_RFID2 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+4, lb.byte_data[i]);
            }
            num=2;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID2已保存");
            break;

  case 2 :
            lb.long_data = last_RFID;
            EEPROM_RFID3 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+8, lb.byte_data[i]);
            }
            num=3;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID3已保存");
            break;
  case 3 :
            lb.long_data = last_RFID;
            EEPROM_RFID4 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+12, lb.byte_data[i]);
            }
            num=4;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID4已保存");
            break;

  case 4 :
            lb.long_data = last_RFID;
            EEPROM_RFID5 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+16, lb.byte_data[i]);
            }
            num=5;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID5已保存");
            break;
  case 5 :
            lb.long_data = last_RFID;
            EEPROM_RFID6 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+20, lb.byte_data[i]);
            }
            num=6;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID6已保存");
            break;
  case 6 :
            lb.long_data = last_RFID;
            EEPROM_RFID7 = lb.long_data;
            for (int i = 0 ; i < 4; i++ )
            {
              EEPROM.write(i+24, lb.byte_data[i]);
            }
            num=1;
            EEPROM.write(30, num);
            EEPROM.commit();//执行保存EEPROM
            Serial.println("门禁卡ID7已保存");
            break;
  
    }
  }
}
 
//初始化IO
void IO_init()
{

}
 
/***********************RFID读卡自定义函数***********************/
//初始化读卡
void RFID_init()
{
  SPI.begin();        // SPI开始
  mfrc522.PCD_Init(); // 初始化
  Serial.println("初始化读卡");
}
 
//运行读卡
void RFID_read()
{
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  else
  {
    last_RFID = read_RFID;
    read_RFID = RFID_toLong(mfrc522.uid.uidByte);
    RFID_Handler(read_RFID);
    mfrc522.PICC_HaltA();//停止 PICC
    mfrc522.PCD_StopCrypto1(); //停止加密PCD
    return;
  }
}
 
//用户ID转换类型
long RFID_toLong(byte *by)
{
  long data;
  for (int i = 0; i < 4; i++)
  {
    lb.byte_data[i] = by[i];
  }
  data = lb.long_data;
  return data;
}
 
//把当前读到卡的ID，对比EEPROM中的ID
void RFID_Handler(long data)
{
  Serial.println(data);

    if (data == EEPROM_RFID1) {
      Serial.println("发现管理员卡，注册成功！");
      Btn_Event();
      client.print("CMD_LED_301");
      delay(1000);
      client.print("CMD_LED_300");
    }
    else if(data == EEPROM_RFID2){
      Serial.println("ID2正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }
 
    else if(data == EEPROM_RFID3){
      Serial.println("ID3正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }
 
    else if(data == EEPROM_RFID4){
      Serial.println("ID4正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }
    else if(data == EEPROM_RFID5){
      Serial.println("ID5正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }
 
    else if(data == EEPROM_RFID6){
      Serial.println("ID6正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }
 
    else if(data == EEPROM_RFID7){
      Serial.println("ID7正确，验证通过");
      client.print("CMD_LED_201");
      delay(1000);
      client.print("CMD_LED_200");
      }  
    else
    {
      Serial.println("ID错误，验证失败");
      client.print("CMD_LED_101");
      delay(1000);
      client.print("CMD_LED_100");
    }
  
}

void loop(){
  delay(1);
  if(bConnected)
  {
   ds18b20();
   int sensorValue = analogRead(A0);// 读取模拟引脚A0上的输入
   float voltage = sensorValue * (5.0/1023.0);// 将模拟读数（从0 - 1023变为0）转换为电压（0 - 5V)
   if(voltage >= 4 && sunflag == 0) {
    client.print("CMD_LIGHT_1");
    sunflag = 1;
   }else if(voltage < 2 && sunflag == 1){
    client.print("CMD_LIGHT_0");
    sunflag = 0;
   }
  }
  if(bConnected ==false)
  {
    if(!client.connect(serverIP,serverPort))
    {
     Serial.println("connection failed");
     delay(5000);
     return;
    }
    bConnected=true;
    Serial.println("connection ok");
  }
  else if(client.available())
  {
    Serial.println("data is coming");
    while(client.available())
    {
       buff[nm++]=client.read();
       if(nm>=511)break;
    }
    buff[nm]=0x00;
    nm=0;
    Serial.print(buff);
    client.print(buff);
    client.flush();
  }
  RFID_read();               //读卡并做分析处理
}
