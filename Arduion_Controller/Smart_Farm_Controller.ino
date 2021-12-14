#include <Adafruit_ESP8266.h>

#define MOTERPIN_IN_1 22
#define MOTERPIN_IN_2 24
#define MOTER_SPEED_1 7
#define RELAY_PIN_1 2
#define RELAY_PIN_2 3
#define RELAY_PIN_3 4

#define TIMEOUT 10000               // TimeOut 시간 설정

// 와이파이 ID 및 Password 설정
const String ssid = "Pi3-AP";
const String password = "raspberry";

// 서버 IP 설정 및 Port 설정
const String ip = "192.168.1.1";
const String port = "80";

// url 반환 타입
const uint8_t ArrayCount = 6;
char receiveType[ArrayCount] = {"grd","tmp1","tmp2","hum1","hum2","lux"};
int16_t receiveValue[ArrayCount] = {0,0,0,0,0,0};

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

void returnData(String returnType, String url){
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
    returnSensingData(returnType);
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
// SensingData 가져오기
void returnSensingData(String returnType){
  int i;

  for(i=0; i<ArrayCount; i++){
    if(returnType == receiveType[i]){
      receiveValue[i] = returnCommand();
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);

  Serial.println(F("===== START ====="));

  //wifi connection
  connectWifi();
  
  pinMode(MOTERPIN_IN_1, OUTPUT);
  pinMode(MOTERPIN_IN_2, OUTPUT);
  pinMode(MOTER_SPEED_1, OUTPUT);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
}

void loop() {
  int i;
  String url;

  //센싱 값 입력 받기
  ///////////////////////////////////////////////////////////////////////////////
  for(i=0; i<ArrayCount; i++){
    url = "GET /"+ receiveType[i] +" HTTP/1.1\r\nHost: " + ip +"\r\n\r\n";
    returnData(receiveType[i], url);
  }
  ///////////////////////////////////////////////////////////////////////////////

  for(i=0; i<ArrayCount; i++){
    Serial.print(F("\r\n === "));
    Serial.print(receiveType[i]);
    Serial.print(F(" : "));
    Serial.print(receiveValue[i]);
    Serial.print(F(" === \r\n"));
  }

  /*
  uint8_t M_StartStop;
  uint8_t M_Speed;
  uint8_t relayOnOff;

  //Moter Control
  Serial.print(F("\r\n === Moter Start : 1, Stop : 0 === \r\n"));
  M_StartStop = serialInput();
  
  //M_StartStop이 0이면 멈춤으로 speed는 자동으로 0으로 설정
  if(M_StartStop != 0){
    Serial.print(F("\r\n === Moter Speed : 0 ~ 255 === \r\n"));
    M_Speed = serialInput();
  }
  else {
    M_Speed = 0;
  }
  moterControl(M_StartStop, M_Speed);

  //Relay Control
  Serial.print(F("\r\n === first Relay On : 1, Off : 0 === \r\n"));
  relayOnOff = serialInput();
  relayControl(RELAY_PIN_1, relayOnOff);

  Serial.print(F("\r\n === second Relay On : 1, Off : 0 === \r\n"));
  relayOnOff = serialInput();
  relayControl(RELAY_PIN_2, relayOnOff);

  Serial.print(F("\r\n === third Relay On : 1, Off : 0 === \r\n"));
  relayOnOff = serialInput();
  relayControl(RELAY_PIN_3, relayOnOff);
  */
  
}


//Serial 입력 받기
uint8_t serialInput(){
  uint8_t inputData;
  //입력 받을때 까지 대기
  while(Serial.available() <= 0){

  }
  //입력 받으면 inputData에 저장
  if(Serial.available()){
    inputData = Serial.parseInt();
  }
  //입력받은값 return
  return inputData;
}

//Moter Control 함수
void moterControl(uint8_t moterStatus, uint8_t moterSpeed){
  //정방향 작동시 입력값 1, 역방향 -1, 나머지 값 멈춤
  if(moterStatus == 1){
    Serial.print(F("\r\n === Moter On, Speed : "));
    Serial.print(moterSpeed);
    Serial.print(F(" ===\r\n"));
    digitalWrite(MOTERPIN_IN_1, HIGH);
    digitalWrite(MOTERPIN_IN_2, LOW);
    analogWrite(MOTER_SPEED_1, moterSpeed);
  }
  else if(moterStatus == -1){
    digitalWrite(MOTERPIN_IN_1, LOW);
    digitalWrite(MOTERPIN_IN_2, HIGH);
    analogWrite(MOTER_SPEED_1, moterSpeed);
  }
  else {
    Serial.print(F("\r\n === Moter Off === \r\n"));
    digitalWrite(MOTERPIN_IN_1, LOW);
    digitalWrite(MOTERPIN_IN_2, LOW);
  }
}

//2ch Relay의 경우 LOW 가 on, HIGH가 off
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
