"""
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
"""

"""
    LUFA Bulk Vendor device demo host test script. This script will send and
    receive a continuous stream of packets to/from to the device, to show
    bidirectional communications.

    Requires the pyUSB library (http://sourceforge.net/projects/pyusb/).
"""

import sys
from time import sleep
import usb.core
import usb.util

# Bulk Vendor HID device VID and PID
import argparse

device_vid = 0x03EB
device_pid = 0x206C
device_in_ep  = 3
device_out_ep = 4


def get_vendor_device_handle():
	dev_handle = usb.core.find(idVendor=device_vid, idProduct=device_pid)
	return dev_handle


def write(device, packet):
    device.write(usb.util.ENDPOINT_OUT | device_out_ep, packet, 1000)
    # device.write(device_out_ep,packet, 1000)
    print("Sent Packet: {0}".format(packet))


def read(device):
    ormask=usb.util.ENDPOINT_IN | device_in_ep;
    packet = device.read(usb.util.ENDPOINT_IN | device_in_ep, 64, 1000)
    print packet
    print("Received Packet: {0}".format(''.join([chr(x) for x in packet])))
    return packet


def main():

    parser = argparse.ArgumentParser('Select command')
    parser.add_argument('-T', '--temp',type=int, help="-T 1 To READ current Temperature",default=0)
    parser.add_argument('-S', '--state',type=int, help="-L [0|1] to Turn ON/OFF",default=0)
    parser.add_argument('-ID', '--id',type=int, help="-ID [1|2|3|4] to Turn ON/OFF",default=1)

    parser.add_argument("-v", "--verbosity", action="count", default=0)
    args = parser.parse_args()
    temp=args.temp
    ledState=args.state
    ledId=args.id;
    message=""
    if args.temp:
      message="cmd%d"%1
    elif args.state and args.id==1 or args.id==2 or args.id==3 or args.id==4:
      message="cmd%d"%args.state%args.id
    else:
      print "Usage: python program.py -T 1 | -S [0|1] -ID [1|2|3|4]"
      exit (0);


    vendor_device = get_vendor_device_handle()


    if vendor_device is None:
        print("No valid Vendor device found.")
        sys.exit(1)

    vendor_device.set_configuration()

    # cfg = vendor_device.get_active_configuration()
    # intf = cfg[(0,0)]
    # ep = usb.util.find_descriptor(intf,custom_match = lambda e:usb.util.endpoint_direction(e.bEndpointAddress) ==usb.util.ENDPOINT_OUT)
    # print usb.util.ENDPOINT_IN
    # # ep.write('Test',0,1000)
    # ep.write('test')



    # print("Connected to device 0x%04X/0x%04X - %s [%s]" %
    #       (vendor_device.idVendor, vendor_device.idProduct,
    #        usb.util.get_string(vendor_device, 255, vendor_device.iProduct),
    #        usb.util.get_string(vendor_device, 255, vendor_device.iManufacturer)))



    x = 0
    while 1:
    	# x = x + 1 % 255
    	write(vendor_device,message)
      # write(vendor_device, "TEST PACKET %d" % x)
    	read(vendor_device)
    	sleep(1)

if __name__ == '__main__':
    main()
