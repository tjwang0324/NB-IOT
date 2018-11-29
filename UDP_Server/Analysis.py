location = input("請輸入檔案名稱(.txt) >>> ")
fin = open("Datas/%s"% location,'r')
count_loss = 0 ## 抓丟包
count_line = 0 ## 讀取行數
total_delay_time = 0 ## 總延遲時間
tmp_delay_time = 0 ## 暫存延遲時間
lines = fin.readline() ## 讀取一行
num = int(lines[21]) ## 秒數從3開始記錄

count = 0 ## debug

try:
	while True:
	    count_line += 1
	    if tmp_delay_time != 0:
	    	if lines[21] == '0':
	    		total_delay_time += abs((int(lines[21:23])+60) - tmp_delay_time)
	    	else:
	    		total_delay_time += abs((int(lines[21:23])) - tmp_delay_time)
	    if lines[21] == '0':
	    	tmp_delay_time = int(lines[21:23])+60
	    else:
	    	tmp_delay_time = int(lines[21:23])

	    if lines[21] != str(num): ## lines[] 設定: 22代表計算每1秒，21代表計算每10秒
	    	count_loss += 1
	    num+=1
	    if num == 5: ## num == 9 代表計算每1秒， 6 代表計算每10秒
	        num = 0 
	    ## debug
	    '''
	    print("total_delay_time: ", total_delay_time)
	    print("tmp_delay_time: ", tmp_delay_time)
	    print("讀取行數: %d 行"% count_line)

	    print(lines[21], "\n")
	    count += 1
	    if count == 10:
	        break
	    '''    
	    lines = fin.readline()

except IndexError:
	print("讀取檔案到結尾，分析完畢。\n")
finally:
	fin.close()
	print("丟包次數為: %d 次"% count_loss)
	print("成功傳送並接收資料的次數為: %d 次"% (count_line-1))
	print("總延遲時間: %d 秒"% total_delay_time)
	print("平均每傳送一次資料延遲: %.2f 秒 "% (total_delay_time/count_line));