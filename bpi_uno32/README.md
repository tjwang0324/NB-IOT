# 使用bpi uno32 透過wifi 編譯MicroPythone

## 認識uno32

本產品採用ESP-WROOM-32為核心，支持藍牙，BLE等通訊方式。
可使用原生的SDK進行編譯，也支援Arduino IDE與Platfrom IO等多種開發平台。

<img src="https://i.imgur.com/kPkmtRV.jpg" width="40%" hight="40%"/>
<img src="https://i.imgur.com/LbD82TR.jpg" width="40%" hight="40%"/>


| 名稱 | 說明 |
|:----:|:---:|
|  CPU      |  Xtensa®32-bitLX6單/雙核處理器  |
|  ROM        |  448KB  |
|  SRAM       |  520KB  |
|  Flash      |  4MB（Onboard），最多4個Flash / SRAM，每個Flash最大16MB  |
|  Power      |  5V@1AmicroUSBpoweror 5.5mm 12VDCport  |
|  GPIO       |  12-bits SAR ADC 18 channel, 2*8-bit D/A converter, 4*SPI, 2*I2S, 2*I2C, 3*UART, Host SD/eMMC/SDIO, Slave SDIO/SPI  |
|  Wi-Fi      |  802.11 b/g/n/e/i 802.11 n \ (2.4GHz 150Mbps\ ) 802.11 e \ (Quality of Service\ )  |
|  Bluetooth  |  BT4.2 & BLE  |
|  Buzzer     |  Passive buzzer  |
|  LEDs       |  RGB LED/POWER LED/Receive LED/Transmit LED  |
|  Sizes      |  68mm*53mm  |


## 驅動程式安裝

先將bpi uno32 連接電腦。

![alt tag](https://i.imgur.com/rCSia4Q.jpg)

開啟裝置管理員查看是否有安裝過驅動 路徑：控制台>系統及安全性>系統>裝置管理員，如果已經安裝過驅動就會如以下畫面所顯示。

![alt tag](https://i.imgur.com/Zhr7TMj.png)

連接埠的位置會顯示USB-SERIAL CH340的文字。若沒有出現，或是出現驚嘆號，表示 沒有安裝過驅動或是驅動沒有安裝成功

驅動ch340網址(<http://wch.cn/download/CH341SER_ZIP.html>)。

載完解壓縮之後依照路徑 CH341SER > CH341SER > SETUP.exe 找到檔案之後進行安裝。

![alt tag](https://i.imgur.com/yrDSKaa.png)

安裝完畢之後就會如上方圖片中裝置管理員所示，出現連接埠下方的訊息。

## 燒錄韌體(使用Linux或是Windows系統下的子系統，以下使用win10示範)


打開“控制台” >> “程式集” >> “開啟或變更 Windows 功能”

![alt tag](https://i.imgur.com/UgpvkJO.png?1)

找到“適用於 Linux 的 Windows 子系統”，將選項打勾

![alt tag](https://i.imgur.com/liX0Czw.png)

接下來選擇“立即重新開機”，不重新開機設定不會生效

![alt tag](https://i.imgur.com/X1pZNfJ.png)

重新開機完後，打開“開始功能選單”，點選“Microsoft Store"或是搜尋“Microsoft Store"

![alt tag](https://i.imgur.com/tfoJr8k.png?1)

搜尋“Ubuntu”

![alt tag](https://i.imgur.com/OfADHsA.png?1)

點選第一個沒有任何數字的“Ubuntu”

![alt tag](https://i.imgur.com/ZnnU4UG.png?1)

接著按取得，下載“Ubuntu”

![alt tag](https://i.imgur.com/sMsOGTV.png?1)

這邊會問你要不要登入，可以選擇登入windows既有的微軟帳號，或是自己重新申請一個

![alt tag](https://i.imgur.com/nXaXpXs.png?1)

安裝完成之後，在”開始功能選單“中可以看到有"Ubuntu"，打開"Ubuntu"

![alt tag](https://i.imgur.com/JgVgu2i.png?1)

打開之後系統會先做基本的設定與詹莊，之後系統會要求你設定使用者帳密

![alt tag](https://i.imgur.com/ZyCKMS0.png?1)

看到這個畫面就表示設定成功，就可以關掉視窗

![alt tag](https://i.imgur.com/RZT36qx.png?1)

打開“開始功能表”點選“Windows Power Shell”

接下來的安裝都是藉由此軟體操作

![alt tag](https://i.imgur.com/lz9Tc9V.png?2)

打開之後輸入指令```bash```

![alt tag](https://i.imgur.com/8UcKiV5.png?1)

輸入剛剛設定的帳號密碼，輸入完成登入後，依序執行以下幾行指令

```sudo su```

輸入剛剛設定的密碼

如果有看到Do you want to continue? [Y/n] 請打y 就是yes的意思

```
apt-get install software-properties-common
apt-add-repository universe
apt-get update
apt-get install python-pip
pip install esptool
```

指令輸入後會安裝一些套件，安裝完成，接著要把windows的comport映射到linux子系統去

輸入以下指令其中*代表剛剛查看到COM的數字

```
apt-get install rdesktop
rdesktop -r comport:COM*=/dev/ttyS*
ln -s /dev/ttyS*  /dev/ttyUSB0
chmod 766 /dev/ttyUSB0
```

完成之後，就可以開始燒路韌體

到最上面tool資料夾裡的"firmware0829.bin"下載後，放到桌面

輸入```cd Desktop/```

執行```esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z --flash_mode dio --flash_freq 40m 0x1000 firmware0829.bin```

執行完之後結果會呈現這樣，這樣就安裝完成了

![alt tag](https://i.imgur.com/hwea4di.png?1)

## 使用ESPFlashDownloadTool 燒錄韌體

到最上面tool資料夾下載“FLASH_DOWNLOAD_TOOLS_V3.6.2”，執行“ESPFlashDownloadTool_v3.6.2.exe”

看到畫面後點選“ESP32 DownloadTool”

![alt tag](https://i.imgur.com/AOT553e.png)

將原本已經勾選的地方取消，接著按下圈起來的三個點

![alt tag](https://i.imgur.com/QOkvkYM.png?1)

選擇剛剛下載的"firmware0829.bin"，按下確定

![alt tag](https://i.imgur.com/1Sgo3pp.png)

回到剛剛的畫面，然後按照上面的步驟做，將第一個選項勾起來，確認一下2,3,4,5資訊有沒有錯誤

接著選擇裝置的COM，確定Baud是115200後就可以按下START

![alt tag](https://i.imgur.com/sDWaxJi.png)

完成後會出現下面的畫面

![alt tag](https://i.imgur.com/1lJiuBx.png?1)

接下來拿出Android手機，開google play 搜尋“esp8266 smartconfig”，選擇第一個

![alt tag](https://i.imgur.com/zh8qrWG.png?1)

點選”安裝“

![alt tag](https://i.imgur.com/Nhaw22H.png?1)

安裝後啟動軟體

![alt tag](https://i.imgur.com/Iz2UzQU.png)

之後將你的手機連接至附近的wifi分享器(要可以輸入帳密的，網頁登入的那種不行)，在輸入框中輸入wifi分享器的密碼

記住帳號之後，點選"CONFIRM"

![alt tag](https://i.imgur.com/z85AGb4.png)

有順利連接到，就會出現此畫面，紅色框框裡是uno32在區網的ＩＰ，若沒有起出現，需多嘗試幾次

![alt tag](https://i.imgur.com/hOBVf1t.png?1)

接下來下載tool裡的putty，下載，安裝後打開

打開之後依照圖去設定，點選“Serial”，輸入裝置的COMPORT，speed是115200，設定完後點選“Open”

![alt tag](https://i.imgur.com/oLuVY4F.png)

進入後輸入```import webrepl_setup```若沒有動靜，需將uno32重新啟動

![alt tag](https://i.imgur.com/WonCXaZ.png?1)

輸入後會跳出選項，選"e"

![alt tag](https://i.imgur.com/oecJvKG.png?1)

接著要設定一組，esp32使用遠端編譯時會用的密碼

![alt tag](https://i.imgur.com/XV052N7.png?1)

設定完後，打開google chrome瀏覽器<http://micropython.org/webrepl/>

![alt tag](https://i.imgur.com/cR4YvqI.png?1)

將原本的IP改成剛剛用手機查詢到的IP，點擊Connected

![alt tag](https://i.imgur.com/w1sl5e7.png?1)

連接之後輸入密碼，接著就可以開始編譯了

![alt tag](https://i.imgur.com/BMwQDdN.png?1)

可使用以下程式碼進行測試

```
from machine import Pin
led = Pin(27,Pin.OUT)
led.value(1)
```
![alt tag](https://i.imgur.com/hgYCUHO.png?1)

成果會如下圖中所示

![alt tag](https://i.imgur.com/YMvKPaX.jpg)


有些事項需注意，電腦要跟uno32在同一個區網內，否則無法使用
第二次之後使用就是uno32上電之後，在用手機查看IP，在進行連線