#include <Arduino.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <STM32ADC.h>

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
unsigned long LedCounter = 0;                                                                            
unsigned long lastTime;

uint8 pin = PB0;

STM32ADC myADC(ADC1);

uint16_t dataPoint;

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
       Serial1.println("Flash Init OK!");
   } else {
       Flash_Ready = false;
       Serial1.print("Flash Init FAIL!");
       Serial1.println(flash.readDeviceId(),HEX);
   }

   pinMode(pin,INPUT_ANALOG);
 
   myADC.setSampleRate(ADC_SMPR_1_5);//set the Sample Rate
   myADC.setScanMode();              //set the ADC in Scan mode. 
   myADC.setPins(&pin, 1);           //set how many and which pins to convert.
   myADC.setContinuous();            //set the ADC in continuous mode.
   
   myADC.setDMA(&dataPoint, 1, (DMA_MINC_MODE | DMA_CIRC_MODE), NULL);

   myADC.startConversion();

   for(int i=0;i<sizeof(LEDS);i++){
       pinMode(LEDS[i], OUTPUT);
       digitalWrite(LEDS[i], HIGH);
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
		Serial1.print("Serial1 Recevied : ");
		Serial1.println(incomingByte, HEX);

		if(Flash_Ready) {
			if(incomingByte == 'a') {
				Serial1.print("Erasing Flash chip ");
				flash.chipErase();
				while(flash.busy()) {
					Serial1.print(".");
					delay(100);
				}
				Serial1.println("DONE");
			}
			if(incomingByte == 0x31) {
				char test[] = "this is a test\0";
				for(int i = 0;i<=sizeof(test);i++) {
					flash.writeByte(i,test[i]);
				}
				Serial1.println("Write Flash Data : \"" + String(test) + "\" Done");
			}
			if(incomingByte == 0x32) {
				String strFlash = "";
				uint8_t chr = 0;
				counter = 0;
				while(true) {
					chr = flash.readByte(counter);
					if(chr == 0xff || chr == 0x0)
						break;
					strFlash.concat((char)chr);
					counter++;
				}
				Serial1.print("Flash Data : ");
				if(strFlash.length() == 0) {
					Serial1.println("(null)");
				} else {
					Serial1.println(strFlash);
				}
			}
			if(incomingByte == 0x33) {
				Serial1.println("ADC Value : " + String(dataPoint));
			}
		}
	}

   //RS485
   if (Serial3.available() > 0) {
        int incomingByte = Serial3.read();
        Serial1.print("Serial 3 Recevied : ");
        Serial1.println(incomingByte, HEX);
    }

   if(millis() - lastTime >= 1000) {

        for(int i=0;i<sizeof(LEDS);i++){
             if(i == LedCounter % sizeof(LEDS)) {
		digitalWrite(LEDS[i], LOW);
	     } else {
	        digitalWrite(LEDS[i], HIGH);
             }
        }
	lastTime = millis();
	LedCounter++;
   }
}
