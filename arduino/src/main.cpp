#include <Arduino.h>
#include <SPI.h>
#include <SPIFlash.h>

// ~/.platformio/packages/framework-arduinoststm32/STM32F1/variants/generic_stm32f103r8/board.cpp

#define BC95_LED1       PC3
#define BC95_LED2       PC2
#define BC95_LED3       PC1
#define BC95_LED4       PC0

#define RS485_RO    PA3
#define RS485_RE    PB8
#define RS485_DE    PB9
#define RS485_DI    PA2

#define FLASH_SS    BOARD_SPI1_NSS_PIN
#define FLASH_ID    0XEF40

SPIFlash flash(FLASH_SS, FLASH_ID);

uint8_t LEDS[] = {BC95_LED1,BC95_LED2,BC95_LED3,BC95_LED4};
unsigned long counter = 0;
unsigned long lastTime;

bool Flash_Ready = false;

void setup() {
   Serial1.begin(115200);
   Serial2.begin(9600);

   afio_remap(AFIO_REMAP_USART3_PARTIAL);
   gpio_set_mode(GPIOC, 10, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOC, 11, GPIO_INPUT_FLOATING);
    gpio_set_mode(GPIOC, 12, GPIO_AF_OUTPUT_PP);

   Serial3.begin(9600);
   Serial3.println("hello im Serial-3");

   if (flash.initialize())
   {
       Flash_Ready = true;
       Serial1.println("Init OK!");
   } else {
       Flash_Ready = false;
       Serial1.print("Init FAIL!");
       Serial1.println(flash.readDeviceId(),HEX);
   }
}

String BC95_Buffer = "";
bool RESET_BC95 = true;
#define BC95_RESET_PIN  PA0
bool BC95_READY = false;
bool ATCMD_OVER = false;

void loop() {

   if(RESET_BC95) {
       pinMode(BC95_RESET_PIN, OUTPUT);
       digitalWrite(BC95_RESET_PIN, HIGH);
       delay(10);
       digitalWrite(BC95_RESET_PIN, LOW);
       RESET_BC95 = false;
   }

   // BC95
       if (Serial2.available() > 0) {
           int incomingByte = Serial2.read();
           BC95_Buffer.concat((char)incomingByte);
           if(BC95_Buffer.endsWith("\r\n")) {
               if(BC95_Buffer.indexOf("Neul") >= 0){
                   BC95_READY = true;
               }

               if(BC95_Buffer == "OK\r\n"){
                   ATCMD_OVER = true;
               }

               Serial1.print(BC95_Buffer);
               BC95_Buffer = "";
           }

           if(BC95_READY && ATCMD_OVER){
                Serial2.print("\r\nAT+CMEE=1\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CGMM\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CGSN=1\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CGATT=1\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CGATT?\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CGATT?\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CSQ\r\n");
                delay(4000);
                Serial2.print("\r\nAT+COPS?\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CEREG=1\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CEREG?\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CSCON=1\r\n");
                delay(2000);
                Serial2.print("\r\nAT+CSCON?\r\n");
                delay(2000);
                
                //AT+NMSTATUS?
               //AT+NMGS=3,AA11BB
                
               BC95_READY = false;
               ATCMD_OVER = false;
           }
       }

    //UART1
   if (Serial1.available() > 0) {
       int incomingByte = Serial1.read();
        BC95_Buffer.concat((char)incomingByte);
        Serial2.print(BC95_Buffer);
        BC95_Buffer = "";
       //Serial1.print("Serial1 Recevied : ");
       //Serial1.println(incomingByte, HEX);               
        
   }

   //RS485
   if (Serial3.available() > 0) {
        int incomingByte = Serial3.read();
        Serial3.print("Serial 3 Recevied : ");
        Serial3.println(incomingByte, HEX);
    }
}
