#include <Arduino.h>
#include <SPI.h>
#include <SPIFlash.h>

// ~/.platformio/packages/framework-arduinoststm32/STM32F1/variants/generic_stm32f103r8/board.cpp
//
#define BC95_LED1   PC3
#define BC95_LED2   PC2
#define BC95_LED3   PC1
#define BC95_LED4   PC0
/*
#define RS485_RO	PA3
#define RS485_RE	PB8
#define RS485_DE	PB9
#define RS485_DI	PA2
*/
/*
#define BOARD_SPI2_NSS_PIN      PB12
#define BOARD_SPI2_SCK_PIN      PB13
#define BOARD_SPI2_MISO_PIN     PB14
#define BOARD_SPI2_MOSI_PIN     PB15
*/
//#define FLASH_SS	BOARD_SPI2_NSS_PIN
#define FLASH_SS    BOARD_SPI1_NSS_PIN
#define FLASH_ID    0XEF40

#define	SOFT_SERIAL_TX  PB0
#define	SOFT_SERIAL_RX  PB1

SPIFlash flash(FLASH_SS, FLASH_ID);
//SoftSerialSTM32 mySerial(SOFT_SERIAL_RX, SOFT_SERIAL_TX); // RX, TX
//SoftSerialInt mySerial(SOFT_SERIAL_RX, SOFT_SERIAL_TX, 2);

uint8_t LEDS[] = {BC95_LED1,BC95_LED2,BC95_LED3,BC95_LED4};
unsigned long counter = 0;
unsigned long LedCounter = 0;
unsigned long lastTime;
/*
DEFINE_HWSERIAL(Serial, 1);                                                                              
DEFINE_HWSERIAL(Serial1, 2);
DEFINE_HWSERIAL(Serial2, 3);
DEFINE_HWSERIAL_UART(Serial3, 4);
DEFINE_HWSERIAL_UART(Serial4, 5);
*/
#if 0
String BC95_CMDS[] = {
    "\r\nAT+NBAND=8\r\n",
    "\r\nAT+CGMM\r\n",
    "\r\nAT+CGSN=1\r\n",
    "\r\nAT+CGATT=1\r\n",
    "\r\nAT+CGATT?\r\n",
    "\r\nAT+CEREG=1\r\n",
    "\r\nAT+CEREG?\r\n",
    "\r\nAT+CSCON?\r\n",
    "\r\nAT+CSQ\r\n",
//    "\r\nAT+CCLK\r\n",
    "\r\nAT+NSOCR=DGRAM,17,8888,1\r\n",
    "\0"
};
#else // TWM
String BC95_CMDS[] = {
    "\r\nAT+NBAND=28\r\n",
    "\r\nAT+NBAND?\r\n",
    "\r\nAT+CGMM\r\n",
    "\r\nAT+CGSN=1\r\n",
    "\r\nAT+CGATT=1\r\n",
    "\r\nAT+CGATT?\r\n",
    "\r\nAT+CEREG=1\r\n",
    "\r\nAT+CEREG?\r\n",
    "\r\nAT+CSCON?\r\n",
    "\r\nAT+CSQ\r\n",
//    "\r\nAT+CCLK\r\n",
    "\r\nAT+NSOCR=DGRAM,17,8888,1\r\n",
    "\0"
    "\r\nAT+CGDCONT=1,\"IP\",\"twm.nbiot\"\r\nr" //台哥大不能連線,再使用的AT指令
};
#endif
String BC95_Buffer = "";
bool RESET_BC95 = true;
#define BC95_RESET_PIN	PA0
bool BC95_READY = false;
bool ATCMD_OVER = false;

int	BC95_CMD_STEP = 0;

bool Flash_Ready = false;
bool BC95_Connected = false;
int socketId = -1;

#define UDP_SERVER  "120.119.77.20"
#define UDP_PORT    60001

bool scan_flash = false;
#define FLASH_MAX_SIZE  8*1024*1024
unsigned long flash_addr = 0;
unsigned long pkgCounter = 0;
unsigned long errorRetry = 0;

void setup() {
	Serial1.begin(115200);
	Serial1.println("STM32 begin ..."+String(F_CPU));

	Serial2.begin(9600);

    afio_remap(AFIO_REMAP_USART3_PARTIAL);
    gpio_set_mode(GPIOC, 10, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOC, 11, GPIO_INPUT_FLOATING);
	gpio_set_mode(GPIOC, 12, GPIO_AF_OUTPUT_PP); 

	Serial3.begin(9600);
	Serial3.println("Hello, world?");

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

void restart(){
	BC95_CMD_STEP = 0;
	BC95_READY = false;
	ATCMD_OVER = false;
	BC95_Connected = false;
	errorRetry = 0;
	pkgCounter = 0;
	Serial1.println("Restarting");
	Serial2.print("\r\nAT+NRB\r\n");
}

void loop() {
//	Serial1.print(__func__ + " : " +String(__LINE__));

	if(RESET_BC95) {
		pinMode(BC95_RESET_PIN, OUTPUT);
		digitalWrite(BC95_RESET_PIN, HIGH);
		delay(10);
		digitalWrite(BC95_RESET_PIN, LOW);
		RESET_BC95 = false;
	}

	if (Serial2.available() > 0) {
		int incomingByte = Serial2.read();
		BC95_Buffer.concat((char)incomingByte);
        
//		Serial1.print(__func__ + " : " +String(__LINE__));
#if 0
            if(BC95_Connected) {
                Serial1.print(BC95_Buffer);
			}
#endif

		if(BC95_Buffer.endsWith("\r\n")) {
			if(BC95_Buffer.length() != 2) {
			    if(BC95_Buffer.indexOf("Neul")>=0) {
		    	}

			    if(BC95_Buffer == "OK\r\n") {
                    errorRetry = 0;
                    if(BC95_CMDS[BC95_CMD_STEP] == "\0") {
                        if(BC95_Connected) {
                            pkgCounter++;
                        }
/*
				        Serial1.print("Cmds finished ");
            	        BC95_Connected = true;
*/
				    } else {
                        if(BC95_CMD_STEP == 0 && BC95_READY == false) {	
				            BC95_READY = true;
                        } else {
                            if(BC95_CMD_STEP == 3) {
                                delay(500);
                            }
				   	        BC95_CMD_STEP++;
                        }
				    }
                    ATCMD_OVER = true;
			    } else if(BC95_CMD_STEP == 10) {
                    socketId = BC95_Buffer.toInt();
			        Serial1.println(String(BC95_CMD_STEP)+" Socket ID : " + String(socketId));
                } else if(BC95_Buffer == "ERROR\r\n") {
                    Serial1.println("ERROR : " + String(errorRetry));
                    errorRetry++;
                    if(!BC95_Connected)
                        delay(1000);
                    ATCMD_OVER = true;
                }
				if(errorRetry >= 10){
					Serial2.print("\r\nAT+CSQ\r\n");
					Serial1.println(BC95_Buffer);
					if(BC95_Buffer == "+CSQ:99,99\r\n"){
						Serial1.println("Error is over 10 times, restart now!");
						restart();
					}
				}
            }
            BC95_Buffer = "";
        }
	}

	if(BC95_READY && ATCMD_OVER && BC95_Connected == false) {
	    if(BC95_CMDS[BC95_CMD_STEP] == "\0") {
		    Serial1.println("Cmds finished ");
            BC95_Connected = true;
/*
            BC95_READY = false;
            ATCMD_OVER = false;
            BC95_Connected == true;
*/
		} else {
            Serial1.print("BC95 Cmds " + String(BC95_CMD_STEP) +  " : " + String(BC95_CMDS[BC95_CMD_STEP]).substring(2));
            Serial2.print(BC95_CMDS[BC95_CMD_STEP]);
            ATCMD_OVER = false;
		}
	}

	if (Serial3.available() > 0) {
		int incomingByte = Serial3.read();
		Serial3.print("Serial 3 Recevied : ");
		Serial3.println(incomingByte, HEX);
	}

	if (Serial1.available() > 0) {
		int incomingByte = Serial1.read();
		//Serial1.print("Serial1 Recevied : ");
		//Serial1.println(incomingByte, HEX);
        Serial2.print((char)incomingByte);
      
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
                scan_flash = true;
			}
		}
	}

    if(scan_flash) {
        uint8_t chr = flash.readByte(flash_addr);
        if(chr == 0x55) {
            Serial1.println(String(flash_addr) + " : " + String(chr,HEX));
        } else {
            flash.writeByte(flash_addr, 0x55);
            chr = flash.readByte(flash_addr);
            if(chr != 0x55) {
                Serial1.println(String(flash_addr) + " : " + String(chr,HEX));
            }
        }

		for(int i=0;i<sizeof(LEDS);i++){
			if(i == LedCounter % sizeof(LEDS)) {
				digitalWrite(LEDS[i], LOW);
			} else {
				digitalWrite(LEDS[i], HIGH);
			}
		}

		LedCounter++;

        flash_addr++;
        if(flash_addr >= FLASH_MAX_SIZE){
            scan_flash = false;
            flash_addr = 0;
        }
    }

	if(millis() - lastTime >= 1000) {

        if(BC95_Connected == true){
			//Serial2.print("\r\nAT+CSQ\r\n");
			//BC95_Buffer.concat(Serial2.read());
			//Serial1.print(BC95_Buffer);
			//BC95_Buffer = "";
//            Serial2.print("\r\nAT+NSOST="+String(socketId)+","+String(UDP_SERVER)+","+String(UDP_PORT)+",2,"+String(LedCounter)+"\r\n");
            String at_cmds =  String("\r\nAT+NSOST="+String(socketId)+","+String(UDP_SERVER)+","+String(UDP_PORT)+",");
            String at_data = String(pkgCounter);
            String at_datalen = String(at_data.length());

            at_cmds.concat(at_datalen);
            at_cmds.concat(',');

            for(int i = 0 ;i < at_data.length();i++) {
                at_cmds.concat(String((uint8_t)at_data.charAt(i),HEX));
            }
		    at_cmds.concat('\r');
		    at_cmds.concat('\n');
            Serial2.print(at_cmds);
//            Serial2.print("\r\nAT+NSOST=1,120.119.77.20,60001,4,54455354\r\n");
//            Serial1.print("\r\nAT+NSOST=1,120.119.77.20,60001,3,535455\r\n");
            Serial1.println("Sended : "+ String(pkgCounter) + " ");
        }
/*
		for(int i=0;i<sizeof(LEDS);i++){
			if(i == LedCounter % sizeof(LEDS)) {
				digitalWrite(LEDS[i], LOW);
			} else {
				digitalWrite(LEDS[i], HIGH);
			}
		}

		LedCounter++;
*/
		lastTime = millis();
	}
}
