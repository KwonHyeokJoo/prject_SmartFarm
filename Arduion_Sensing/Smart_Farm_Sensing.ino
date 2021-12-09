#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Emotion_Farm.h>
#include <DHT.h>

//센서 핀 설정
#define relayPin 3                  // 릴레이 모듈 핀 -> 생장 LED 켜기 위함
#define cdsPin A1                   // 조도센서 모듈 핀
#define DHTPIN 4                    // 온습도센서 모듈 핀
#define DHTTYPE DHT11               // 온습도 센서타입 설정
#define soilmoisturePin A0          // 토양수분센서 핀

//와이파이 통신을 위해 객체생성?
SoftwareSerial mySerial(2,3);

//LCD 및 온습도 센서 객체 생성 
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

//와이파이 ID 및 Password 설정
const String ssid = "Pi3-AP";
const String password = "raspberry";

int num;

//서버 IP 설정 및 Port 설정
const String ip = "192.168.1.1";
const String port = "80";
const String portHumi = "hum";
const String portTemp = "tmp";

//문자열 출력을 위한 변수
char str_M[10];
char str_T[10];
char str_H[10];

char combuffer[50];

//wifi connection
void connectWifi() {
  int i;
  String output;
  
  //Wifi 초기화
  Serial.println(F("\r\n---------- AT+RST ----------\r\n"));
  output = "AT+RST";
  serialPrint(1000, output);
  
  //모드 설정(1로 설정)
  Serial.println(F("\r\n---------- AT+CWMODE ----------\r\n"));
  output = "AT+CWMODE=1";
  serialPrint(500, output);
  
  //wifi 연결
  Serial.println(F("\r\n---------- AT+CWJAP_DEF ----------\r\n"));
  output = "AT+CWJAP_DEF=\""+ssid+"\",\""+password+"\"";
  Serial.print("Connect Wifi...\r\n");
  serialPrint(500, output);

  //연결된 ip 출력
  Serial.print(F("\r\n---------- AT+CIFSR ----------\r\n"));
  output = "AT+CIFSR";
  serialPrint(500, output);

}

//Restart when wifi connection faill
void connectFailRestart(){
  int i;
  int stopAndGo = 1;

  while(stopAndGo){
    Serial.print(F("\r\n---------- AT+CIPSTATUS ----------\r\n"));
    serialPrint(10,"AT+CIPSTATUS");
    
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
        serialPrint(100,"AT+CIPCLOSE");
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
  String output;
  
  Serial.print(F("\r\n---------- AT+CIPMUX ----------\r\n"));
  output = "AT+CIPMUX=0";
  serialPrint(5, output);

  Serial.print(F("\r\n---------- AT+CIPSTART ----------\r\n"));
  output = "AT+CIPSTART=\"TCP\",\"" + ip +"\"," + port;
  serialPrint(5, output);

  Serial.print(F("\r\n---------- AT+CIPSEND ----------\r\n"));
  output = "AT+CIPSEND=" + String(url.length());
  serialPrint(5, output);

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
  serialPrint(1, url);
  
}

void serialPrint(int number, String input){
  int i;
  int j;
  
  mySerial.println(input);
  
  for(i=0; i<number; i++){
    delay(10);
    for(j=0; mySerial.available(); j++){
      combuffer[j] = mySerial.read();
      Serial.write(combuffer[j]);
    }
  }
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
 
  pinMode(relayPin, OUTPUT);
  pinMode(cdsPin, INPUT);
  pinMode(soilmoisturePin, INPUT);

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
  int soilmoistureValue = analogRead(soilmoisturePin);                // 토양수분 값 측정: 0(습함) ~ 1023(건조)
  int soilmoisture_per = map(soilmoistureValue, 170, 1023, 100, 0);   // 센서 값을 퍼센트로 변경
  unsigned char h_Value = dht.readHumidity();                         // 공기 중 습도 값 측정
  unsigned char t_Value = dht.readTemperature();                      // 공기 중 온도 값 측정
  String url;                                                         // 데이터 전송할 url

  //LCD에 토양 습도값 출력
  lcd.setCursor(1,0);
  lcd.print("MOIST:");
  sprintf(str_M, "%03d", soilmoisture_per);
  lcd.print(str_M);
  lcd.setCursor(10,0);
  lcd.print("%");

  //LCD에 온도값 출력
  lcd.setCursor(1,1);
  lcd.write(0);
  sprintf(str_T, "%02d", t_Value);
  lcd.setCursor(3,1);
  lcd.print(str_T);
  lcd.write(1);

  //LCD에 습도값 출력
  lcd.setCursor(7,1);
  lcd.write(2);
  sprintf(str_H, "%02d", h_Value);
  lcd.setCursor(9,1);
  lcd.print(str_H);
  lcd.print("%");

  ///////////////////////////////////////////////////////////////////////////////
  // /hum/현재습도
  //"GET /hum/50 HTTP/1.1\r\nHost: 192.168.1.1\r\n\r\n"
  url = "GET /"+ portHumi + "/" + h_Value + " HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
  dataSend(url);
  //////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // /tmp/현재온도
  //"GET /hum/50 HTTP/1.1\r\nHost: 192.168.1.1\r\n\r\n"
  url = "GET /"+ portTemp + "/" + t_Value + " HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
  dataSend(url);
  //////////////////////////////////////////////////////////////////////////////

  //토양습도 값에 따른 LCD에 이모티콘 띄우기
  if(soilmoisture_per >= 0 && soilmoisture_per < 30){
    lcd.setCursor(13,0);
    lcd.write(3);
    lcd.setCursor(14,0);
    lcd.write(4);
  }
  else if(soilmoisture_per >= 30 && soilmoisture_per < 70){
    lcd.setCursor(13,0);
    lcd.print(" ");
    lcd.setCursor(14,0);
    lcd.write(5);
  }
  else if(soilmoisture_per >= 70){
    lcd.setCursor(13,0);
    lcd.write(3);
    lcd.setCursor(14,0);
    lcd.write(6);
  }

  //Restart when wifi connection faill
  connectFailRestart();
  
  delay(500);

}
