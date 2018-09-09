#include <Arduino.h>
#include <SPI.h>
#include <SPIFlash.h>

// ~/.platformio/packages/framework-arduinoststm32/STM32F1/variants/generic_stm32f103r8/board.cpp

#define BC95_LED1       PC3
#define BC95_LED2       PC2
#define BC95_LED3       PC1
#define BC95_LED4       PC0

#define RS485_RO	PA3
#define RS485_RE	PB8
#define RS485_DE	PB9
#define RS485_DI	PA2
/*
#define BOARD_SPI2_NSS_PIN      PB12
#define BOARD_SPI2_SCK_PIN      PB13
#define BOARD_SPI2_MISO_PIN     PB14
#define BOARD_SPI2_MOSI_PIN     PB15
*/
//#define FLASH_SS	BOARD_SPI2_NSS_PIN
#define FLASH_SS	BOARD_SPI1_NSS_PIN
#define FLASH_ID	0XEF40

SPIFlash flash(FLASH_SS, FLASH_ID);

uint8_t LEDS[] = {BC95_LED1,BC95_LED2,BC95_LED3,BC95_LED4};
unsigned long counter = 0;
unsigned long lastTime;
/*
DEFINE_HWSERIAL(Serial, 1);                                                                              
DEFINE_HWSERIAL(Serial1, 2);
DEFINE_HWSERIAL(Serial2, 3);
DEFINE_HWSERIAL_UART(Serial3, 4);
DEFINE_HWSERIAL_UART(Serial4, 5);
*/

bool Flash_Ready = false;

void setup() {
	Serial1.begin(115200);
	Serial1.println("STM32 begin ...");

	Serial2.begin(9600);
//	Serial3.begin(9600);

	if (flash.initialize())
	{
		Flash_Ready = true;
		Serial1.println("Init OK!");
	} else {
		Flash_Ready = false;
		Serial1.print("Init FAIL!");
		Serial1.println(flash.readDeviceId(),HEX);
	}

	for(int i=0;i<sizeof(LEDS);i++){
		pinMode(LEDS[i], OUTPUT);
		digitalWrite(LEDS[i], HIGH);
	}
}

String BC95_Buffer = "";
bool RESET_BC95 = true;
#define BC95_RESET_PIN	PA0

void loop() {
	if(RESET_BC95) {
		pinMode(BC95_RESET_PIN, OUTPUT);
		digitalWrite(BC95_RESET_PIN, HIGH);
		delay(10);
		digitalWrite(BC95_RESET_PIN, LOW);
//		delay(100);
		RESET_BC95 = false;
	}

	if (Serial2.available() > 0) {
		int incomingByte = Serial2.read();
		BC95_Buffer.concat((char)incomingByte);
		if(BC95_Buffer.endsWith("\r\n")) {
			Serial1.print(BC95_Buffer);
			BC95_Buffer = "";
		}
	}
/*
	if (Serial3.available() > 0) {
		int incomingByte = Serial3.read();
		Serial1.print("Serial3 Recevied : ");
		Serial1.println(incomingByte, HEX);
	}
*/
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
			}
		}
	}

	if(millis() - lastTime >= 1000) {
		for(int i=0;i<sizeof(LEDS);i++){
			if(i == counter % sizeof(LEDS)) {
				digitalWrite(LEDS[i], LOW);
			} else {
				digitalWrite(LEDS[i], HIGH);
			}
		}
/*
		Serial1.print("Time: ");
		lastTime = millis();

		Serial1.print(lastTime);
		Serial1.print("\t");
		
	        Serial1.println(flash.readByte(counter), HEX);                                                            
		counter++;
*/
	}
}
