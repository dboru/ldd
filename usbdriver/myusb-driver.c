#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
// #define BULK_EP_OUT 0x01
// #define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 8

#define MY_USB_VENDOR_ID  0x03DB
#define MY_USB_PRODUCT_ID 0x430C
#define TIMEOUT 5000

// static struct usb_device *device;
// static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];


/* table of devices that work with this driver */
static const struct usb_device_id myusb_table[] = {
    { USB_DEVICE(MY_USB_VENDOR_ID, MY_USB_PRODUCT_ID) },
    { }                 /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, myusb_table);

// #define to_myusb_dev(d) container_of(d, struct usb_myusb, kref)
// static struct usb_driver myusb_driver;

struct usb_myusb {
    struct usb_device   *udev;      /* usb device for this device */
    struct usb_interface    *interface; /* interface for this device */
    unsigned char       minor;      /* minor value */
    unsigned char *     bulk_in_buffer; /* the buffer to in data */
    size_t          bulk_in_size;   /* the size of the in buffer */
    __u8            bulk_in_add;    /* bulk in endpoint address */
    __u8            bulk_out_add;   /* bulk out endpoint address */
    struct kref     kref;       /* module references counter */
};



static void set_bulk_address (struct usb_myusb *dev, struct usb_interface *interface)
{
    struct usb_endpoint_descriptor *endpoint;
    struct usb_host_interface *iface_desc;
    int i;

    iface_desc = interface->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;

        /* check for bulk endpoint */
        if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
                == USB_ENDPOINT_XFER_BULK) {

            /* bulk in */
            if (endpoint->bEndpointAddress & USB_DIR_IN) {
                dev->bulk_in_add = endpoint->bEndpointAddress;
                dev->bulk_in_size = endpoint->wMaxPacketSize;
                dev->bulk_in_buffer = kmalloc(dev->bulk_in_size, GFP_KERNEL);
                if (!dev->bulk_in_buffer)
                    printk("Could not allocate bulk buffer\n");
            }

            /* bulk out */
            else
                dev->bulk_out_add = endpoint->bEndpointAddress;
        }
    }
}

static int myusb_open(struct inode *i, struct file *f)
{
    return 0;
}
static int myusb_close(struct inode *i, struct file *f)
{
    return 0;
}
static ssize_t myusb_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{

    struct usb_myusb *dev;
    int retval;
    int read_cnt;
    dev = f->private_data;
    /* Read the data from the bulk endpoint */
    retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_add),
                          bulk_buf, MAX_PKT_SIZE, &read_cnt, TIMEOUT);
    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
    {
        return -EFAULT;
    }

    return MIN(cnt, read_cnt);
}
static ssize_t myusb_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
    struct usb_myusb *dev;
    int retval;
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }
    dev = f->private_data;
    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_add),
                          bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, TIMEOUT);
    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }

    return wrote_cnt;
}

static struct file_operations fops =
{
    .open = myusb_open,
    .release = myusb_close,
    .read = myusb_read,
    .write = myusb_write,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver myusb_class = {
    .name =     "myusb%d",
    .fops =     &fops,
    .minor_base =   USB_SKEL_MINOR_BASE,
};



static int myusb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
    struct usb_myusb *dev;

    /* allocate memory for our device state and initialize it */
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;


    dev->udev = usb_get_dev(interface_to_usbdev(interface));
    dev->interface = interface;

    // device = interface_to_usbdev(interface);

    set_bulk_address (dev, interface);


    // class.name = "usb/myusb%d";
    // class.fops = &fops;
    if ((retval = usb_register_dev(interface, &myusb_class)) < 0)
    {
        /* Something prevented us from registering this driver */
        err("Not able to get a minor for this device.");
    }
    else
    {
        // printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
        /* let the user know what node this device is now attached to */
        dev_info(&interface->dev,
                 "USB myusb device now attached to USBMyusb-%d",
                 interface->minor);
    }

    return retval;
}

static void myusb_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &myusb_class);
}


static struct usb_driver myusb_driver =
{
    .name = "myusb",
    .probe = myusb_probe,
    .disconnect = myusb_disconnect,
    .id_table = myusb_table,
};

static int __init myusb_init(void)
{
    int result;

    /* Register this driver with the USB subsystem */
    if ((result = usb_register(&myusb_driver)))
    {
        err("usb_register failed. Error number %d", result);
    }
    return result;
}

static void __exit myusb_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&myusb_driver);
}

module_init(myusb_init);
module_exit(myusb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dejene");
MODULE_DESCRIPTION("USB driver");