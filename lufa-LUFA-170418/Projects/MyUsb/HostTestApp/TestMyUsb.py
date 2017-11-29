"""
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
"""

"""
    My Usb device demo host test script. This script will send a command to turn ON/OFF
    LEDs on the board and also sends a command to READ temperature.

    Requires the pyUSB library (http://sourceforge.net/projects/pyusb/).
"""

import sys
from time import sleep
import usb.core
import usb.util
import argparse
import random

# MyUsb HID device VID and PID
# device_vid = 0x03EB
# device_pid = 0x206C
device_vid = 0x03DB
device_pid = 0x430C

device_in_ep = 3
device_out_ep = 4


def get_vendor_device_handle():
    dev_handle = usb.core.find(idVendor=device_vid, idProduct=device_pid)
    return dev_handle


def write(device, packet):
    device.write(usb.util.ENDPOINT_OUT | device_out_ep, packet, 10000)


def read(device):
    packet = device.read(usb.util.ENDPOINT_IN | device_in_ep, 8, 10000)[0:5]
    return packet
def main():

    vendor_device = get_vendor_device_handle()

    if vendor_device is None:
        print("No valid Vendor device found.")
        sys.exit(1)

    vendor_device.set_configuration()

    p = 0

    while 1:
        data = [(p >> 3) & 1, (p >> 2) & 1, (p >> 1) & 1, (p >> 0) & 1]
        print('Sending LED pattern: ', data)
        write(vendor_device, data)
        print ("Reading LED status and Temp")
        packet = read(vendor_device)
        for v in range(len(packet)):
          if v == (len(packet) - 1):
            print('Temp(C): {0:d}'.format(packet[v]))
          else:
            print('LED{0:d}:{1:2d}'.format(v + 1, packet[v]))
        sleep(1)
        p = (p + 1) % 16

if __name__ == '__main__':
    main()
