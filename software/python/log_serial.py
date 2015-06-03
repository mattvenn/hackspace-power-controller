import serial
import logging as log
from query import get_install_dir

log.basicConfig(level=log.DEBUG,
            format='%(asctime)s %(message)s',
            datefmt='%m-%d %H:%M:%S',
            filename=get_install_dir() + '/serial.log',
            filemode='a')

serial_port=serial.Serial()
serial_port.port='/dev/ttyACM0'
serial_port.baudrate=9600
serial_port.open()

while True:
    response = serial_port.readline()
    response = response.strip()
    print(response)
    log.info(response)
