#!/usr/bin/python
import serial
import logging as log

log.basicConfig(level=log.DEBUG,
            format='%(asctime)s %(message)s',
            datefmt='%m-%d %H:%M:%S',
            filename='serial.log',
            filemode='a')

serial_port=serial.Serial()
serial_port.port='/dev/ttyACM0'
serial_port.baudrate=9600
serial_port.open()

try:
    while True:
        response = serial_port.readline()
        response = response.strip()
        print(response)
        log.info(response)
finally:
    print("shutting down nicely")
    log.shutdown()
