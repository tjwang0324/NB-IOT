from socket import *
from time import ctime
 
host = '' ## 監聽所有IP
port = 60001 ## 端口必須一樣
bufsize = 1024
addr = (host,port) 
 
udpServer = socket(AF_INET,SOCK_DGRAM) ## 建立socket
udpServer.bind(addr) ## 開始監聽
f = open('Datas/testData.txt', 'a') ## 建立要寫入的資料名稱

count = 0 ## 設定要成功接收幾包

try:
    while True:
        print('等待連接......')
        data,addr = udpServer.recvfrom(bufsize)  ## 接收資料和返回地址
        ## 處理資料
        data  = data.decode(encoding='utf-8')
        data = "時間: %s ,來源IP:%s ,資料:%s\n"%(ctime(),addr,data)
        udpServer.sendto(data.encode(encoding='utf-8'),addr)
        ## 發送資料
        ##print('from and return to :',addr)
        print(data) 
        f.write(data)  

        count += 1
        if count == 10800:
            break    
except KeyboardInterrupt:
    print("例外發生: 使用鍵盤跳出")
    print("存檔中...")
    f.close()
    udpServer.close()
    print("存檔完畢")
finally:
    print("執行完畢，存檔中...")
    f.close()
    udpServer.close()
    print("存檔完畢")



## 例外考慮
'''
    except SystemExit as n:
        print("系統錯誤，應對方法: 將data存檔")
        print("存檔中...")
        f.close()
        udpServer.close()
        print("存檔完畢")
'''