#include <Arduino.h>
#include <SPI.h>
#include <SPIFlash.h>

// ~/.platformio/packages/framework-arduinoststm32/STM32F1/variants/generic_stm32f103r8/board.cpp
//
#define BC95_LED1       PC3
#define BC95_LED2       PC2
#define BC95_LED3       PC1
#define BC95_LED4       PC0
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
#define FLASH_SS	BOARD_SPI1_NSS_PIN
#define FLASH_ID	0XEF40

#define	SOFT_SERIAL_TX	PB0
#define	SOFT_SERIAL_RX	PB1

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

char BC95_CMDS[][80] = {
    {"\r\nAT+NBAND=28\r\n"},
	{"\r\nAT+NBAND?\r\n"},
    {"\r\nAT+CGMM\r\n"},
    {"\r\nAT+CGSN=1\r\n"},
    {"\r\nAT+CGATT=1\r\n"},
    {"\r\nAT+CGATT?\r\n"},
    {"\r\nAT+CEREG=1\r\n"},
    {"\r\nAT+CEREG?\r\n"},
    {"\r\nAT+CSCON?\r\n"},
    {"\r\nAT+CSQ\r\n"},
    {"\r\nAT+NSOCR=DGRAM,17,8888,1\r\n"},
	{"\0"},
	//{'\r\nAT+CGDCONT=1,"IP","twm.nbiot"\r\n'}	//台哥大不能連線,再使用的AT指令
};

int BC95_CMD_STEP = 0;
bool Flash_Ready = false;
bool BC95_Connected = false;

void setup() {
	Serial1.begin(115200);
	Serial1.println("STM32 begin ..."+String(F_CPU));

	Serial2.begin(9600);

  	afio_remap(AFIO_REMAP_USART3_PARTIAL);
 	gpio_set_mode(GPIOC, 10, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOC, 11, GPIO_INPUT_FLOATING);
	gpio_set_mode(GPIOC, 12, GPIO_AF_OUTPUT_PP); 

	Serial3.begin(9600);
	Serial3.println("Hello, I'm Serial3!");

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

	if (Serial2.available() > 0) {
		int incomingByte = Serial2.read();
		BC95_Buffer.concat((char)incomingByte);
		if(BC95_Buffer.endsWith("\r\n")) {
			if(BC95_Buffer.indexOf("Neul")>=0) {
				BC95_READY = true;
			}

			if(BC95_Buffer == "OK\r\n") {
				ATCMD_OVER = true;
				
				//----------------------狀態機
				if(BC95_CMDS[BC95_CMD_STEP] == "\0" && !BC95_Connected) {
					Serial1.print("Cmds finished ");
            		BC95_Connected = true;
					BC95_READY = false;
					ATCMD_OVER = false;
				} else {
					Serial2.print(BC95_CMDS[BC95_CMD_STEP]);
					delay(2000);
				    BC95_CMD_STEP++;
				}
				
				
			}
			Serial1.print(BC95_Buffer);
			BC95_Buffer = "";
		}
	}
	if (Serial3.available() > 0) {
		int incomingByte = Serial3.read();
		Serial3.print("Serial 3 Recevied : ");
		Serial3.println(incomingByte, HEX);
	}
	if (Serial1.available() > 0) { 
		//----------------------手動下AT_Command
		int incomingByte = Serial1.read();
		//Serial1.print("Serial1 Recevied : ");
		//Serial1.println(incomingByte, HEX);
        Serial2.print((char)incomingByte);

  /*      
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
		}*/
        
	}

	if(millis() - lastTime >= 1000) {
        if(BC95_Connected == true){
			//----------------------向伺服器傳送資料
			/*
        	Serial2.print("\r\nAT+NSOST=1,120.119.77.20,60001,3,535455\r\n");
			delay(1000);
			Serial2.print("\r\nAT+NSORF=1,80\r\n");	// 接收UDP數據，顯示格式:(SocketID,來源IP,port,收到數據長度,數據(HEX碼))
			Serial2.print("\r\nAT+CSQ\r\n");
			*/
        }
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
