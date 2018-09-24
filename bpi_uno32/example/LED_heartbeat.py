from machine import Pin
import time
LED = Pin(18, Pin.OUT)

tick = 0 # counter variable

var = 1
def toggle(LED):
	LED.value(not LED.value())
	
while var == 1:
	if tick <= 3:
		toggle(LED)
	#make sure the value of tick has a 0-9 range
	tick = (tick + 1) % 10
	time.sleep(0.1) 