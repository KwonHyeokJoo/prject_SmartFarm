#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Emotion_Farm.h>
#include <DHT.h>

//센서 핀 설정
#define cdsPin A1                   // 조도센서 모듈 핀
#define DHTPIN 4                    // 온습도센서 모듈 핀 1
#define DHTPIN2 7                   // 온습도센서 모듈 핀 2
#define DHTTYPE DHT11               // 온습도 센서타입 설정
#define soilmoisturePin A0          // 토양수분센서 핀

//와이파이 통신을 위해 객체생성?
SoftwareSerial mySerial(2,3);

//LCD 및 온습도 센서 객체 생성 
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

//와이파이 ID 및 Password 설정
const String ssid = "Pi3-AP";
const String password = "raspberry";

//서버 IP 설정 및 Port 설정
const String ip = "192.168.1.1";
const String port = "80";

//토양 %, 온도, 습도 출력을 위한 문자열
char moist_print[5];
char temp_print[5];
char humi_print[5];

//입력된 값 찾기위해 버퍼 선언
char combuffer[50];

uint8_t beforeTemp = 0;

//wifi connection
void connectWifi() {
  
  //Wifi 초기화
  Serial.println(F("\r\n---------- AT+RST ----------\r\n"));
  mySerial.println(F("AT+RST"));
  serialPrint(1000);
  
  //모드 설정(1로 설정)
  Serial.println(F("\r\n---------- AT+CWMODE ----------\r\n"));
  mySerial.println(F("AT+CWMODE=1"));
  serialPrint(500);
  
  //wifi 연결
  Serial.println(F("\r\n---------- AT+CWJAP_DEF ----------\r\n"));
  mySerial.print(F("AT+CWJAP_DEF=\""));
  mySerial.print(ssid);
  mySerial.print(F("\",\""));
  mySerial.print(password);
  mySerial.println(F("\""));
  serialPrint(500);

  //연결된 ip 출력
  Serial.print(F("\r\n---------- AT+CIFSR ----------\r\n"));
  mySerial.println(F("AT+CIFSR"));
  serialPrint(500);

}

//Restart when wifi connection faill
void connectFailRestart(){
  int i;
  int stopAndGo = 1;

  while(stopAndGo){
    Serial.print(F("\r\n---------- AT+CIPSTATUS ----------\r\n"));
    mySerial.println(F("AT+CIPSTATUS"));
    serialPrint(10);
    
    for(i=0; combuffer[i]!=NULL; i++){
      if(combuffer[i] == ':') break;
    }
    
    switch(combuffer[i+1]){
      case '2':
        Serial.print(F("\r\n---------- case 2 ----------\r\n"));
        stopAndGo = 0;
        break;
      case '3':
        Serial.print(F("\r\n---------- case 3 ----------\r\n"));
        mySerial.println(F("AT+CIPCLOSE"));
        serialPrint(100);
        break;
      case '4':
        Serial.print(F("\r\n---------- case 4 ----------\r\n"));
        stopAndGo = 0;
        break;
      case '5':
        Serial.print(F("\r\n---------- case 5 ----------\r\n"));
        connectWifi();
        break;
      default:
        Serial.print(F("\r\n----- default -----\r\n"));
    }
  }
}

//Sensor data send
void dataSend(String url){
  int i,j;
  
  Serial.print(F("\r\n---------- AT+CIPMUX ----------\r\n"));
  mySerial.println(F("AT+CIPMUX=0"));
  serialPrint(50);

  Serial.print(F("\r\n---------- AT+CIPSTART ----------\r\n"));
  mySerial.print(F("AT+CIPSTART=\"TCP\",\""));
  mySerial.print(ip);
  mySerial.print(F("\","));
  mySerial.println(port);
  serialPrint(50);

  Serial.print(F("\r\n---------- AT+CIPSEND ----------\r\n"));
  mySerial.print(F("AT+CIPSEND="));
  mySerial.println(String(url.length()));
  serialPrint(50);

  for(j=0; combuffer[j]!=NULL; j++){
    
  }
  
  for(i=0; i<j-1; i++){
    if(combuffer[i] == 'p'){
      if(combuffer[i+1] == '.'){
        connectFailRestart();
        break;
      }
    }
   }
   
  Serial.print(F("\r\n---------- GET ----------\r\n"));
  mySerial.println(url);
  serialPrint(800);
  
}

void serialPrint(uint16_t second){
  int i, j;

  for(i=0; i<second; i++){
    delay(10);

    for(j=0; mySerial.available(); j++){
      combuffer[j] = mySerial.read();
      Serial.write(combuffer[j]);
    }
  }
}

//현재 온습도 상태 표시
void lcdPrint(uint16_t moist, int8_t temp1, int8_t temp2, uint8_t humi1, uint8_t humi2){
  
  //토양 수분 출력
  lcd.setCursor(1,0);
  lcd.print("MOIST:");
  lcd.setCursor(8,0);
  sprintf(moist_print, "%03d", moist);
  lcd.print(moist_print);
  lcd.setCursor(11,0);
  lcd.print("%");

  //토양습도 값에 따른 LCD에 이모티콘 띄우기
  if(moist >= 0 && moist < 30){
    lcd.setCursor(13,0);
    lcd.write(3);
    lcd.setCursor(14,0);
    lcd.write(4);
  }
  else if(moist >= 30 && moist < 70){
    lcd.setCursor(13,0);
    lcd.print(" ");
    lcd.setCursor(14,0);
    lcd.write(5);
  }
  else if(moist >= 70){
    lcd.setCursor(13,0);
    lcd.write(3);
    lcd.setCursor(14,0);
    lcd.write(6);
  }

  //LCD에 온도값 출력
  lcd.setCursor(1,1);
  lcd.write(0);
  sprintf(temp_print, "%02d", temp1);
  lcd.setCursor(3,1);
  lcd.print(temp_print);
  lcd.write(1);

  lcd.setCursor(7,1);
  lcd.write(0);
  sprintf(temp_print, "%02d", temp2);
  lcd.setCursor(9,1);
  lcd.print(temp_print);
  lcd.write(1);

  delay(5000);
  
  //LCD에 습도값 출력
  lcd.setCursor(1,1);
  lcd.write(2);
  sprintf(humi_print, "%02d", humi1);
  lcd.setCursor(3,1);
  lcd.print(humi_print);
  lcd.print("%");
  
  lcd.setCursor(7,1);
  lcd.write(2);
  sprintf(humi_print, "%02d", humi2);
  lcd.setCursor(9,1);
  lcd.print(humi_print);
  lcd.print("%");

  delay(5000);
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println("----- STRAT -----");

  //LCD 초기화
  lcd.begin();

  //wifi connection
  connectWifi();
 
  pinMode(cdsPin, INPUT);                   //조도 센서
  pinMode(soilmoisturePin, INPUT);          //토양수분센서

  // 라이브러리로 추가한 특수 문자 및 이모티콘 추가
  lcd.createChar(0, temp);
  lcd.createChar(1, C);
  lcd.createChar(2, humi);  
  lcd.createChar(3, Qmark);
  lcd.createChar(4, water);
  lcd.createChar(5, good);
  lcd.createChar(6, wind);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //센서값 측정
  uint16_t cdsValue = analogRead(cdsPin);                                  // 조도센서 값 측정: 0(밝음) ~ 1023(어두움)
  uint16_t soilmoistureValue = analogRead(soilmoisturePin);                // 토양수분 값 측정: 0(습함) ~ 1023(건조)
  uint8_t moist_per = map(soilmoistureValue, 170, 1023, 100, 0);             // 센서 값을 퍼센트로 변경
  //공기 중 습도 값 측정
  uint8_t humiValue1 = (uint8_t)dht.readHumidity();
  uint8_t humiValue2 = (uint8_t)dht2.readHumidity();
  //공기 중 온도 값 측정
  //unsigned char tmpValue1 = dht.readTemperature();
  int8_t tempValue1 = (int8_t)dht.readTemperature();
  int8_t tempValue2 = (int8_t)dht2.readTemperature();
  // 데이터 전송할 url
  String url;
  int8_t differenceTemp;

  //LCD에 현재 온습도 상태 표시
  lcdPrint(moist_per, tempValue1, tempValue2, humiValue1, humiValue2);

  //이전 온도값이 지금 온도값이랑 다르다면 서버에 전체 데이터 송부
  ///////////////////////////////////////////////////////////////////////////////
  differenceTemp = beforeTemp - tempValue1;
  if(abs(differenceTemp) >= 5){
    url = "GET /"+String(moist_per)+"/"+tempValue1+"/"+tempValue2+"/"+humiValue1+
        "/"+humiValue2+"/"+String(cdsValue)+" HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
    dataSend(url);

    beforeTemp = tempValue1;
  }
  ///////////////////////////////////////////////////////////////////////////////

  
  //Restart when wifi connection faill
  connectFailRestart();
  
//  delay(500);

}
