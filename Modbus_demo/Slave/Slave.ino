/*      Slave     */
#include <SoftSerialSTM32.h>
SoftSerialSTM32 Slave(32,34);//TX  RX設定
char val; 
int DE_RE=18;  //DE RE相接 接到arduino pin2
 
void setup() {
  Serial.begin(38400);              //設定序列埠的傳輸速率
  Serial.println("Slave is ready!");//設定完會顯示的字
  Slave.begin(38400);              //設定Master方的傳輸速率
  pinMode(DE_RE,OUTPUT);           //設定DE_RE為OUTPUT
  digitalWrite(DE_RE,LOW);         //設定DE_RE為LOW
}
 
void loop() {
  while (Serial.available()) {      //當序列埠讀取到使用者輸入的資料
    digitalWrite(DE_RE,HIGH);      //DE_RE變成HIGH指定Slave為輸出資料
    val = Serial.read();            //指定Val為你輸入的資料
    Slave.write(val);              //以Slave回應
  } 
 
  digitalWrite(DE_RE,LOW);         //設定DE_RE為LOW
  while(Slave.available()) {       //當序列埠讀取到資料時
    val = Slave.read();            //指定val為輸入的資料
    Serial.write(val);              //在序列埠監控視窗印出資料
  } 
}
