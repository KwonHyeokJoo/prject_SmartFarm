#include <Servo.h>
#include <Adafruit_ESP8266.h>

#define FANCONTROL_IN_1 22
#define FANCONTROL_IN_2 24
#define FANCONTROL_SPEED 7

#define LEFT_WINDOW_PIN 8
#define RIGHT_WINDOW_PIN 9

#define HEATER_PIN 2
#define LED_PIN 3
#define PUMP_PIN 4
#define PUMP_LED_PIN_1 5
#define PUMP_LED_PIN_2 6
#define PUMP_LED_PIN_3 7

#define TIMEOUT 10000               // TimeOut 시간 설정
#define WINDOW_ANGLE_MAX 30         // 개폐기 최대 각도
#define WINDOW_ANGLE_MIN 10         // 개폐기 최소 각도

Servo leftWindow;
Servo rightWindow;

// 와이파이 ID 및 Password 설정
const String ssid = "Pi3-AP";
const String password = "raspberry";

// 서버 IP 설정 및 Port 설정
const String ip = "192.168.1.1";
const String port = "80";

//수동 및 자동 제어, 0이면 수동제어, 1이면 자동제어
// 토양수분, 온도1, 온도2, 습도1, 습도2, 조도,
// 수동자동, 팬상태, 팬스피드, 왼쪽개폐기, 오른쪽개폐기, 열풍기
// Receive ALL Data
const uint8_t sensingDataCount = 6;
const uint8_t controlDataCount = 6;
const uint8_t allDataCount = sensingDataCount + controlDataCount;
String receiveType[allDataCount] = {"grd","tmp1","tmp2","hum1","hum2","lux",
                                    "getManualControl","getFanState","getFanSpeed",
                                    "getLeftWindow","getRightWindow","getHeaterState"};
int16_t receiveValue[allDataCount] = {0,0,0,0,0,0,1,0,0,0,0,0};
// 개폐기 초기 열림 각도 : 최대값으로 설정
int8_t leftWindowAngle = WINDOW_ANGLE_MAX;
int8_t rightWindowAngle = WINDOW_ANGLE_MAX;

//wifi connection
void connectWifi() {
  uint8_t connectionStatus;
  
  while (1){
    //Wifi 초기화
    Serial.println(F("\r\n---------- AT+RST ----------\r\n"));
    Serial3.println(F("AT+RST"));
    responseSerial("IP");
  
    
    //모드 설정(1로 설정)
    Serial.println(F("\r\n---------- AT+CWMODE ----------\r\n"));
    Serial3.println(F("AT+CWMODE=1"));
    responseSerial("OK");

    while(1){
      //wifi 연결
      Serial.println(F("\r\n---------- AT+CWJAP_DEF ----------\r\n"));
      Serial3.print(F("AT+CWJAP_DEF=\""));
      Serial3.print(ssid);
      Serial3.print(F("\",\""));
      Serial3.print(password);
      Serial3.println(F("\""));
      responseSerial("OK");
      
      //연결된 ip 출력
      Serial.println(F("\r\n---------- AT+CIFSR ----------\r\n"));
      Serial3.println(F("AT+CIFSR"));
      responseSerial("OK");

      //2 반환시 ip할당 성공, 5 반환시 ip할당 실패로 연결 재시도
      if(nowStatus() == 5){
        continue;
      }
      else {
        return;
      }
    }
  }
}
// 현재 상태 출력
uint8_t nowStatus(){
  uint8_t connectionStatus;

  Serial.println(F("\r\n---------- AT+CIPSTATUS ----------\r\n"));
  Serial3.println(F("AT+CIPSTATUS"));
  // 'STATUS:' 가 들어오면 뒤에 숫자 값을 받아 리턴
  if(Serial3.find("STATUS:")){
    connectionStatus = Serial3.parseInt();
  }
  responseSerial("OK");

  return connectionStatus;
}
//Command 값 가져오기
int16_t returnCommand(){
  int16_t returnCommand;
  
  if(Serial3.find('@')){
    returnCommand = Serial3.parseInt();
  }

  return returnCommand;
}


//Serial 출력 및 Command 정상으로 들어갔나 확인
void responseSerial(char* keyword){
  uint8_t cutChar = 0;
  uint8_t keywordLength = 0;
  unsigned long startTime;
  int i;

  for(i=0; keyword[i] != NULL; i++){
    keywordLength++;
  }

  startTime = millis();
  while((millis() - startTime) < TIMEOUT){
    if(Serial3.available()){
      char ch = Serial3.read();
      Serial.write(ch);

      if(ch == keyword[cutChar]){
        cutChar++;
        if(cutChar == keywordLength){
          return;
        }
      }
      else {
        cutChar = 0;
      }
    }
  }
  return;
}

void receiveData(String returnType, String url){
  uint8_t startStop = 1;

  while(1){
    Serial.print(F("\r\n----- AT+CIPMUX -----\r\n"));
    Serial3.println(F("AT+CIPMUX=0"));
    responseSerial("OK");

    while (1){
      Serial.print(F("\r\n----- AT+CIPSTART -----\r\n"));
      Serial3.print(F("AT+CIPSTART=\"TCP\",\""));
      Serial3.print(ip);
      Serial3.print(F("\","));
      Serial3.println(port);
      responseSerial("OK");

      //TCP가 연결 되었으면 while문 break; 아니면 다시 연결 시도
      if(nowStatus() == 3){
        break;
      }
      else {
        continue;
      }
    }
  
    Serial.print(F("\r\n----- AT+CIPSEND -----\r\n"));
    Serial3.print(F("AT+CIPSEND="));
    Serial3.println(url.length());
    responseSerial("OK\r\n>");
  
    Serial.print(F("\r\n---------- GET ----------\r\n"));
    Serial3.println(url);
    receiveAllData(returnType);
    responseSerial("CLOSED");

    startStop = nowStatus();
    //5: ip할당 실패, 2: ip할당 성공했으나, 중간에 끊긴것임으로 함수 종료
    //3, send 실패 다시 시도, 4 : 정상 send 후 종료
    if(startStop == 5){
      return;
    }
    else if(startStop == 2) {
      return;
    }
    else if(startStop == 3){
      continue;
    }
    else {
      break;
    }
  }
}
// Sensing & Control Data 가져오기
void receiveAllData(String returnType){
  int i;

  for(i=0; i<allDataCount; i++){
    if(returnType == receiveType[i]){
      receiveValue[i] = returnCommand();
    }
  }
}

// Moter Control
void moterControl(uint8_t moterStatus, uint8_t in_1, uint8_t in_2){
  //정방향 작동시 입력값 1, 역방향 -1, 나머지 값 멈춤
  if(moterStatus == 1){
    Serial.print(F("\r\n === Moter forward === \r\n "));
    digitalWrite(in_1, HIGH);
    digitalWrite(in_2, LOW);
  }
  else if(moterStatus == -1){
    Serial.print(F("\r\n === Moter reverse === \r\n "));
    digitalWrite(in_1, LOW);
    digitalWrite(in_2, HIGH);
  }
  else {
    Serial.print(F("\r\n === Moter Off === \r\n"));
    digitalWrite(in_1, LOW);
    digitalWrite(in_2, LOW);
  }
}

// LOW 가 on, HIGH가 off
void relayControl(uint8_t pinNumber,uint8_t relayOnOff){
  if(relayOnOff == 1){
    Serial.print(F("\r\n=== Relay On ===\r\n"));
    digitalWrite(pinNumber, LOW);
  }
  else {
    Serial.print(F("\r\n=== Relay Off ===\r\n"));
    digitalWrite(pinNumber, HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);

  Serial.println(F("===== START ====="));

  //wifi connection
  connectWifi();

  // Servo Moter 핀 설정
  leftWindow.attach(LEFT_WINDOW_PIN);
  rightWindow.attach(RIGHT_WINDOW_PIN);

  // Fan Moter 핀 설정
  pinMode(FANCONTROL_IN_1, OUTPUT);
  pinMode(FANCONTROL_IN_2, OUTPUT);
  pinMode(FANCONTROL_SPEED, OUTPUT);

  // Relay 핀 설정
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_LED_PIN_1, OUTPUT);
  pinMode(PUMP_LED_PIN_2, OUTPUT);
  pinMode(PUMP_LED_PIN_3, OUTPUT);

  // Relay 초기화
  digitalWrite(HEATER_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(PUMP_LED_PIN_1, HIGH);
  digitalWrite(PUMP_LED_PIN_2, HIGH);
  digitalWrite(PUMP_LED_PIN_3, HIGH);
}

void loop() {
  uint8_t manualControl;
  
  int i,j;
  String url;

  //센싱 값 입력 받기
  ///////////////////////////////////////////////////////////////////////////////
  for(i=0; i<allDataCount; i++){
    url = "GET /"+ receiveType[i] +" HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
    receiveData(receiveType[i], url);
  }
  ///////////////////////////////////////////////////////////////////////////////

  //현재 작동 상태 파악, Manual Mode인지, Auto Mode인지
  for(i=0; i<allDataCount; i++){
    if(receiveType[i] == receiveType[sensingDataCount]){
      manualControl = receiveValue[i];
    }
  }

  //수동 제어
  if(manualControl == 0){
    uint8_t fanState;
    uint8_t fanSpeed;
    uint8_t LeftControl;
    uint8_t RightControl;
    uint8_t heater;

    // 제어할 data 및 종료 가져오기
    for(i=sensingDataCount; i<allDataCount; i++){
      if(receiveType[i] == receiveType[sensingDataCount+1]){
        fanState = receiveValue[i];
      }
      else if(receiveType[i] == receiveType[sensingDataCount+2]){
        fanSpeed = receiveValue[i];
      }
      else if(receiveType[i] == receiveType[sensingDataCount+3]){
        LeftControl = receiveValue[i];
      }
      else if(receiveType[i] == receiveType[sensingDataCount+4]){
        RightControl = receiveValue[i];
      }
      else if(receiveType[i] == receiveType[sensingDataCount+5]){
        heater = receiveValue[i];
      }
    }

    //Fan Control
    // 현재 fan이 가동중인데, 앱에서 정지(0)하라는 명령이 들어오면 정지
    if(digitalRead(FANCONTROL_IN_1) == HIGH && fanState == 0){
      moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
    }
    // 현재 fan이 정지이고, 앱에서 가동(1)하라는 명령이 들어오면 가동
    else if(digitalRead(FANCONTROL_IN_1) == LOW && fanState == 1){
      moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
      digitalWrite(FANCONTROL_SPEED, fanSpeed);   //fanSpeed 설정
    }

    // Left Window 개폐
    // 개폐기가 닫혀있거나, 또는 MAX(30)보다 적게 열려있을경우
    // 최대 각도까지 열어준다
    if(LeftControl == 1 && leftWindowAngle < WINDOW_ANGLE_MAX){
      leftWindowAngle = WINDOW_ANGLE_MAX;
    }
    else if(LeftControl == 0 && leftWindowAngle > WINDOW_ANGLE_MIN){
      leftWindowAngle = WINDOW_ANGLE_MIN;
    }
    leftWindow.write(leftWindowAngle);
    
    //Right Window 개폐
    if(RightControl == 1 && rightWindowAngle < WINDOW_ANGLE_MAX){
      rightWindowAngle = WINDOW_ANGLE_MAX;
    }
    else if(RightControl == 0 && rightWindowAngle > WINDOW_ANGLE_MIN){
      rightWindowAngle = WINDOW_ANGLE_MIN;
    }
    rightWindow.write(rightWindowAngle);

    //Heater Control On Off
    if(digitalRead(HEATER_PIN) == HIGH && heater == 0){
      relayControl(HEATER_PIN, heater);
    }
    else if(digitalRead(HEATER_PIN) == LOW && heater == 1){
      relayControl(HEATER_PIN, heater);
    }
  }
  //자동 제어
  else if(manualControl == 1){
    
  }
}
