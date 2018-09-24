from machine import Pin
import time
led = Pin(18, Pin.OUT) # get a led on gpio 18.
while True:
    print('turn on')
    led.value(1) # turn on
    print('sleep 1s')
    time.sleep(1) # sleep 1s
    print('turn off')
    led.value(0) # turn off
    print('sleep 1s')
    time.sleep(1) # sleep 1s