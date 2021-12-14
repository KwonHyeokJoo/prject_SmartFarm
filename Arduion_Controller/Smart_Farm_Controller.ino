#define MOTERPIN_IN_1 22
#define MOTERPIN_IN_2 24
#define MOTER_SPEED_1 7

#define RELAY_PIN_1 2
#define RELAY_PIN_2 3

void setup() {
  Serial.begin(9600);

  Serial.println(F("===== START ====="));
  
  pinMode(MOTERPIN_IN_1, OUTPUT);
  pinMode(MOTERPIN_IN_2, OUTPUT);
  pinMode(MOTER_SPEED_1, OUTPUT);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
}

void loop() {
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
