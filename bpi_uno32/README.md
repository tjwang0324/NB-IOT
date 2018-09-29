# 使用bpi uno32 透過wifi 編譯MicroPythone

## 前言

bpi uno32 是一塊具有wifi與藍芽功能的模組

![alt tag](https://i.imgur.com/Fg4Xywh.jpg)

|硬體規格|
|CPU｜Xtensa®32-bitLX6單/雙核處理器｜
|ROM|448KB|
|SRAM|520KB|
|Flash|4MB（Onboard），最多4個Flash / SRAM，每個Flash最大16MB|
|Power|5V@1AmicroUSBpoweror 5.5mm 12VDCport|
|GPIO|12-bits SAR ADC 18 channel, 2\*8-bit D/A converter, 4*SPI, 2*I2S, 2*I2C, 3*UART, Host SD/eMMC/SDIO, Slave SDIO/SPI|
|Wi-Fi|802.11 b/g/n/e/i 802.11 n\(2.4GHz 150Mbps\) 802.11 e\(Quality of Service\)|
|Bluetooth|BT4.2 & BLE|
|Buzzer|Passive buzzer|
|LEDs|RGB LED/POWER LED/Receive LED/Transmit LED|
|Sizes|68mm*53mm|


同時也支援Arduino IDE與Platfrom IO等多種開發平台，這篇主要是分享使用MicroPython 開發bpi uno32。

## 驅動程式安裝

先將bpi uno32 連接電腦。
![alt tag](https://i.imgur.com/Dr5PJF5.jpg)

接著去裝置管理員查看是否有安裝過驅動 路徑：控制台>系統及安全性>系統>裝置管理員，如果有安裝就會像以下畫面。
![alt tag](https://i.imgur.com/Zhr7TMj.png)

連接埠的位置會顯示USB-SERIAL CH340的文字，若沒有出現可以下載ch340的驅動，網址(<http://wch.cn/download/CH341SER_ZIP.html>)。

下載完解壓縮之後依照路徑 CH341SER > CH341SER > SETUP.exe 找到檔案之後進行安裝。
![alt tag](https://i.imgur.com/yrDSKaa.png)

安裝完畢之後就會如上面裝置管理員所示，出現連接埠下方的訊息。

## 開發環境

要的開發平台是使用這一套uPyCraft(<http://docs.dfrobot.com.cn/upycraft/>)，bpi uno32的 wiki 上也是使用這一套進行程式的編寫。
![alt tag](https://i.imgur.com/8cUkd07.png)

下載完成之後到(<http://micropython.org/download#esp32>)下載最新版本的韌體。
![alt tag](https://i.imgur.com/olaE3rm.png)

兩者都下載完之後打開uPyCraft。

打開之後會出現這個對話框，意思是詢問要不要安裝文字套件，點選”OK”。不安裝會沒辦法使用軟體並且每次打開都會詢問一次。
![alt tag](https://i.imgur.com/1veEmN4.png)

進入畫面之後會再跳出一個視窗詢問要不要更新，我個人是選否，更新之後一些比較重要的功能會無法使用。
![alt tag](https://i.imgur.com/a08Kp0s.png)

選項選完之後第一步先選”Tools>Serial>(CH340的COM PORT)”。
![alt tag](https://i.imgur.com/gRPAIhF.png)

之後會跳出這個視窗按照圖片上的選項選完之後，在Firmware Choose那邊選擇第二個，檔案選擇前面說明中下載的esp32的韌體。
![alt tag](https://i.imgur.com/EyeNrhi.png)

等狀態條跑完後，就可以看看有沒有辦法能與板子連接。
![alt tag](https://i.imgur.com/sWp9RoF.png)

出現這樣的箭頭就表示連接成功，並且剛剛鎖鏈圖示的地方也會變成現在圖中的狀態。這樣就可以開始codeing了。example裡面有一些程式可以試試看，這邊示範其中幾個。
![alt tag](https://i.imgur.com/waFp6a5.png)

## LED on off

首先一開始先new一個檔案。
![alt tag](https://i.imgur.com/IGmYSr3.png)

打開之後可以把example的code貼上來。
![alt tag](https://i.imgur.com/BgmfIV4.png)

貼上之後，先儲存。
![alt tag](https://i.imgur.com/zHRkPpy.png)

選擇程式碼存放的資料夾。
![alt tag](https://i.imgur.com/LTKmSu4.png)

接著皆可以開始執行了，圈圈1的功能是compiler先檢查有沒有什麼語法上的錯誤，如果沒有就可以點選圈圈2上傳，這個範例是hello.py的code會讓LED跑亮滅迴圈。
上傳成功之後會顯示的訊息，並且這個LED(pin18)會亮滅
![alt tag](https://i.imgur.com/UtsIlLT.png)
![alt tag](https://imgur.com/a/uL1vy5D)

## 基礎算法

打開example的leapyear.py，compiler後上傳，上傳後在”>>>”後面輸入```cmd
is_leap_year(int(input("input a leap year ")))```
，之後就可以輸入你想判斷的年份
![alt tag](https://i.imgur.com/3H2uCJV.png)

可以看到 2004 is leap year，說明 2004 年是閏年。
可以看到 1999 not is leap year，說明 1999 年不是閏年。
可以看到 2000 is leap year，說明 2000 年是閏年。
你也可以繼續輸入更多，讓它回答你哪一年是閏年。


什麼是斐波那契数列？它指的是，有這樣一個數列 0, 1, 1, 2, 3, 5, 8, 13，特别指出：第0項是0，第1項是第一個1。从第三項開始，每一項都等於前兩項之和。

打開example的fablist.py，compiler後上傳，上傳後在”>>>”後面輸入```cmd
printfablist(int(input('please input a number:')))```
，接著可以自己輸入驗證看看
![alt tag](https://i.imgur.com/xU9aDeC.png)
