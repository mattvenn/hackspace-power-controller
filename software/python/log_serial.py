#!/usr/bin/python
import serial
import logging

log = logging.getLogger('')

# setup logging
log.setLevel(logging.DEBUG)
log_format = logging.Formatter("%(asctime)s - %(levelname)-8s - %(message)s")

ch = logging.StreamHandler()
ch.setFormatter(log_format)
log.addHandler(ch)

fh = logging.FileHandler('serial.log')

serial_port=serial.Serial()
serial_port.port='/dev/ttyACM0'
serial_port.baudrate=9600
serial_port.open()

try:
    while True:
        response = serial_port.readline()
        response = response.strip()
        log.info(response)
finally:
    log.shutdown()
