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

#define FAN_SPEED_MAX 255
#define FAN_SPEED_MID 128
#define FAN_SPEED_MIN 0

Servo leftWindow;
Servo rightWindow;

// 와이파이 ID 및 Password 설정
const String ssid = "PI_AP";
const String password = "12345678";

// 서버 IP 설정 및 Port 설정
const String ip = "192.168.1.1";
const String port = "80";

//수동 및 자동 제어, 0이면 수동제어, 1이면 자동제어
// 토양수분, 온도1, 온도2, 습도1, 습도2, 조도,
// 수동자동, 팬상태, 팬스피드, 왼쪽개폐기, 오른쪽개폐기, 열풍기
// Receive ALL Data
const uint8_t sensingDataCount = 7; //getTargetTmp 추가
const uint8_t controlDataCount = 6; 
const uint8_t allDataCount = sensingDataCount + controlDataCount;
String receiveType[allDataCount] = {"grd","tmp1","tmp2","hum1","hum2","lux","getTargetTmp",
                                    "getManualControl","getFanState","getFanSpeed",
                                    "getLeftWindow","getRightWindow","getHeaterState"};
int16_t receiveValue[allDataCount] = {0,0,0,0,0,0,0,1,0,0,0,0,0};

//fan 시작 시간 측정
uint64_t startFanSpeedControl;
uint64_t endFanSpeedControl;
const uint16_t waitFanSpeedControl = 3000;

// 개폐기 초기 열림 각도 : 최대값으로 설정
int8_t leftWindowAngle = WINDOW_ANGLE_MAX;
int8_t rightWindowAngle = WINDOW_ANGLE_MAX;

//목표 온도 //토양습도
uint8_t targetTemp; 
uint8_t targetMoist = 40;
const uint64_t waitTime = 5000;

//개폐기 시간 측정
uint64_t leftStartWindowTime = millis();
uint64_t leftEndWindowTime = millis();
uint64_t rightStartWindowTime = millis();
uint64_t rightEndWindowTime = millis();

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
void moterControl(uint8_t moterStatus, uint8_t pin_In_1, uint8_t pin_In_2){
  //정방향 작동시 입력값 1, 역방향 -1, 나머지 값 멈춤
  if(moterStatus == 1){
    Serial.print(F("\r\n === Moter forward === \r\n "));
    digitalWrite(pin_In_1, HIGH);
    digitalWrite(pin_In_2, LOW);
  }
  else if(moterStatus == -1){
    Serial.print(F("\r\n === Moter reverse === \r\n "));
    digitalWrite(pin_In_1, LOW);
    digitalWrite(pin_In_2, HIGH);
  }
  else {
    Serial.print(F("\r\n === Moter Off === \r\n"));
    digitalWrite(pin_In_1, LOW);
    digitalWrite(pin_In_2, LOW);
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
  uint8_t oldFanSpeed = 0;
  uint8_t fanState,fanSpeed,LeftControl,RightControl,heater;
  //자동제어시 펌프상태 저장변수
  uint8_t pumpState;
  // 토양습도, 온도1, 온도2, 습도1, 습도2 값 저장받을 변수
  uint8_t grdData,tmp1Data,tmp2Data,hum1Data,hum2Data,luxData;
  int i,j;
  String url;

  //센싱 값 입력 받기
  ///////////////////////////////////////////////////////////////////////////////
  for(i=0; i<allDataCount; i++){
    url = "GET /"+ receiveType[i] +" HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
    receiveData(receiveType[i], url);
  }
  ///////////////////////////////////////////////////////////////////////////////

  // 전달 받은 센싱값 및 제어값 가져오기
  for(i=0; i<allDataCount; i++){
    if(receiveType[i] == "grd"){
      grdData = receiveValue[i];
    }
    else if(receiveType[i] == "tmp1"){
      tmp1Data = receiveValue[i];
    }
    else if(receiveType[i] == "tmp2"){
      tmp2Data = receiveValue[i];
    }
    else if(receiveType[i] == "hum1"){
      hum1Data = receiveValue[i];
    }
    else if(receiveType[i] == "hum2"){
      hum2Data = receiveValue[i];
    }
    else if(receiveType[i] == "lux"){
      luxData = receiveValue[i];
    }
    else if(receiveType[i] == "getTargetTmp"){
      targetTemp = receiveValue[i];
    }
    else if(receiveType[i] == "getManualControl"){
      manualControl = receiveValue[i];
    }
    else if(receiveType[i] == "getFanState"){
      fanState = receiveValue[i];
    }
    else if(receiveType[i] == "getFanSpeed"){
      fanSpeed = receiveValue[i];
    }
    else if(receiveType[i] == "getLeftWindow"){
      LeftControl = receiveValue[i];
    }
    else if(receiveType[i] == "getRightWindow"){
      RightControl = receiveValue[i];
    }
    else if(receiveType[i] == "getHeaterState"){
      heater = receiveValue[i];
    }
    else {
      Serial.print(F("\r\n\r\n === err === \r\n\r\n"));
    }
  }

  // 제어할 data 및 종료 가져오기
  for(i=sensingDataCount; i<allDataCount; i++){
    if(receiveType[i] == receiveType[sensingDataCount]){
      fanState = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount+1]){
      fanSpeed = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount+2]){
      LeftControl = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount+3]){
      RightControl = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount+4]){
      heater = receiveValue[i];
    }
  }

  // 센서값 가져오기
  for(i=0; i<sensingDataCount; i++){
    if(receiveType[i] == receiveType[sensingDataCount-7]){
      grdData = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-6]){
      tmp1Data = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-5]){
      tmp2Data = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-4]){
      hum1Data = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-3]){
      hum2Data = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-2]){
      luxData = receiveValue[i];
    }
    else if(receiveType[i] == receiveType[sensingDataCount-1]){
      targetTemp = receiveValue[i];
    }
  }


  //수동 제어
  if(manualControl == 0){
    //Fan Control
    if(fanState == 0){
      //fan 정지
      if(digitalRead(FANCONTROL_IN_1) == LOW){
        //현재 fan 정지 상태
        Serial.print(F("\r\n\r\n === 이미 정지 === \r\n\r\n"));
      }
      else {
        //가동중이면 정지
        moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
      }
    }
    else if(fanState == 1){
      //fan 가동
      if(digitalRead(FANCONTROL_IN_1) == HIGH){
        //가동중 속도 조절
        if(oldFanSpeed != fanSpeed){
          if((endFanSpeedControl - startFanSpeedControl) > waitFanSpeedControl){
            if(fanSpeed < FAN_SPEED_MIN){
              //Fan Speed 최소값 설정(80)
              fanSpeed = FAN_SPEED_MIN;
              Serial.print(F("\r\n\r\n === Speed MIN === \r\n\r\n"));
            }
            else if (fanSpeed > FAN_SPEED_MAX){
              fanSpeed = FAN_SPEED_MAX;
              Serial.print(F("\r\n\r\n === Speed MAX === \r\n\r\n"));
            }
            else {
              Serial.print(F("\r\n\r\n === Speed START === \r\n\r\n"));
            }
            analogWrite(FANCONTROL_SPEED, fanSpeed);
          }
          else {
            endFanSpeedControl = millis();
          }
        }
        else {
          Serial.print(F("\r\n\r\n === 이미 가동 === \r\n\r\n"));
        }
      }
      else {
        //정지 -> 가동
        moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
        analogWrite(FANCONTROL_SPEED, FAN_SPEED_MAX);
        oldFanSpeed = FAN_SPEED_MAX;
        startFanSpeedControl = millis();
        endFanSpeedControl = millis();
      }
    }

    // Left Window 개폐
    if(LeftControl == 0){
      //close window control
      if(leftWindowAngle == WINDOW_ANGLE_MIN){
        Serial.print(F("\r\n\r\n === 이미 닫혀 있음 === \r\n\r\n"));
      }
      else {
        //안되면 while 사용해야됨 
        if((leftEndWindowTime - leftStartWindowTime) > (WINDOW_ANGLE_MAX *10)){
          leftStartWindowTime = millis();
          leftWindowAngle = WINDOW_ANGLE_MIN;
          leftWindow.write(leftWindowAngle);
        }
        else {
          Serial.print(F("\r\n\r\n === 작동 대기중 === \r\n\r\n"));
          leftEndWindowTime = millis();
        }
      }
    }
    else if(LeftControl == 1){
      //open window control
      if(leftWindowAngle == WINDOW_ANGLE_MAX){
        Serial.print(F("\r\n\r\n === 이미 열려 있음 === \r\n\r\n"));
      }
      else {
        if((leftEndWindowTime - leftStartWindowTime) > (WINDOW_ANGLE_MAX *10)){
          leftStartWindowTime = millis();
          leftWindowAngle = WINDOW_ANGLE_MAX;
          leftWindow.write(leftWindowAngle);
        }
        else {
          Serial.print(F("\r\n\r\n === 작동 대기중 === \r\n\r\n"));
          leftEndWindowTime = millis();
        }
      }
    }
    else {
      Serial.print(F("\r\n === LeftWindow err, return value err === \r\n"));
    }
    
    //Right Window 개폐
    if(RightControl == 0){
      //close window control
      if(rightWindowAngle == WINDOW_ANGLE_MIN){
        Serial.print(F("\r\n\r\n === 이미 닫혀 있음 === \r\n\r\n"));
      }
      else {
        //안되면 while 사용해야됨 
        if((rightEndWindowTime - rightStartWindowTime) > (WINDOW_ANGLE_MAX *10)){
          rightStartWindowTime = millis();
          rightWindowAngle = WINDOW_ANGLE_MIN;
          rightWindow.write(rightWindowAngle);
        }
        else {
          Serial.print(F("\r\n\r\n === 작동 대기중 === \r\n\r\n"));
          rightEndWindowTime = millis();
        }
      }
    }
    else if(RightControl == 1){
      //open window control
      if(rightWindowAngle == WINDOW_ANGLE_MAX){
        Serial.print(F("\r\n\r\n === 이미 열려 있음 === \r\n\r\n"));
      }
      else {
        if((rightEndWindowTime - rightStartWindowTime) > (WINDOW_ANGLE_MAX *10)){
          rightStartWindowTime = millis();
          rightWindowAngle = WINDOW_ANGLE_MAX;
          rightWindow.write(rightWindowAngle);
        }
        else {
          Serial.print(F("\r\n\r\n === 작동 대기중 === \r\n\r\n"));
          rightEndWindowTime = millis();
        }
      }
    }
    else {
      Serial.print(F("\r\n === RightWindow err, return value err === \r\n"));
    }

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
    uint8_t tempAvg;
    tempAvg = (tmp1Data + tmp2Data)/2;
    
    //Fan Control(Auto)
    //온도 앞 뒤 차이가 5도 이상이면 팬을 가동하여 공기 순환
    if(abs(tmp1Data - tmp2Data) >= 5) {
      fanState = 1;
      analogWrite(FANCONTROL_SPEED,FAN_SPEED_MAX);
      moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
    } 
    else if (abs(tmp1Data - tmp2Data) >= 3) {
      fanState = 1;
      analogWrite(FANCONTROL_SPEED,FAN_SPEED_MID);
      moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);  
    }
    else {
      fanState = 0;
      moterControl(fanState, FANCONTROL_IN_1, FANCONTROL_IN_2);
    }

    //히터(Auto)
    //목표온도가 현재온도와 3도이상 차이날 경우 히터 On 
    if(targetTemp - tempAvg >= 3) {
      heater = 1;
      relayControl(HEATER_PIN, heater);
    } 
    else {
      heater = 0;
      relayControl(HEATER_PIN, heater);
    }
    
    //개폐기(Auto)
    //목표온도가 현재온도와 3도이상 차이날 경우 개폐기 On 
    if(tempAvg - targetTemp >= 3) {
      leftWindowAngle = WINDOW_ANGLE_MAX;
      rightWindowAngle = WINDOW_ANGLE_MAX;
    } 
    else {
      leftWindowAngle = WINDOW_ANGLE_MIN;
      rightWindowAngle = WINDOW_ANGLE_MIN;
    }
  }
  
  //토양습도센서(Auto)
  //목표습도보다 낮은 경우 워터펌프 가동
  if(grdData <= targetMoist) {
    pumpState = 1;
    relayControl(PUMP_PIN, pumpState); 
  } 
  else {
    pumpState = 0;
    relayControl(PUMP_PIN, pumpState); 
  }  
}
