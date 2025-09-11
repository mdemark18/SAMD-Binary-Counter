/* 
 * File:   cdc_acm_usb.h
 * Author: Ethan Schlepp
 *
 * Created on December 13, 2024, 4:51 PM
 */

#ifndef CDC_ACM_USB_H
#define	CDC_ACM_USB_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
#define CDC_ACM_EP_IN  0x81
#define CDC_ACM_EP_OUT 0x02
    
static const uint8_t device_descriptor[] = {
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x00, 0x02, // bcdUSB 2.00
    0x02,       // bDeviceClass (CDC)
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    0x40,       // bMaxPacketSize0 64
    0x00, 0x00, // idVendor
    0x00, 0x00, // idProduct
    0x00, 0x01, // bcdDevice 1.00
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x03,       // iSerialNumber (String Index)
    0x01        // bNumConfigurations 1
};

static const uint8_t config_descriptor[] = {
    // Configuration Descriptor
    0x09,       // bLength: Configuration Descriptor size
    0x02,       // bDescriptorType: Configuration
    0x43, 0x00, // wTotalLength: Bytes returned (43 bytes total)
    0x01,       // bNumInterfaces: 1 interface
    0x01,       // bConfigurationValue: Configuration value
    0x00,       // iConfiguration: Index of string descriptor describing this configuration
    0x80,       // bmAttributes: Self powered
    0x32,       // bMaxPower: Max power consumption (100 mA)

    // Interface Descriptor
    0x09,       // bLength: Interface Descriptor size
    0x04,       // bDescriptorType: Interface
    0x00,       // bInterfaceNumber: Number of this interface
    0x00,       // bAlternateSetting: Alternate setting
    0x02,       // bNumEndpoints: 2 endpoints
    0x02,       // bInterfaceClass: Communications class (CDC)
    0x02,       // bInterfaceSubClass: Abstract Control Model
    0x01,       // bInterfaceProtocol: AT commands (v.25ter)
    0x00,       // iInterface: Index of string descriptor

    // Header Functional Descriptor
    0x05,       // bLength: Size of this descriptor in bytes
    0x24,       // bDescriptorType: CS_INTERFACE
    0x00,       // bDescriptorSubtype: Header Func Desc
    0x10, 0x01, // bcdCDC: Spec release number (1.10)

    // Call Management Functional Descriptor
    0x05,       // bFunctionLength
    0x24,       // bDescriptorType: CS_INTERFACE
    0x01,       // bDescriptorSubtype: Call Management Func Desc
    0x00,       // bmCapabilities: D0+D1
    0x01,       // bDataInterface: 1

    // ACM Functional Descriptor
    0x04,       // bFunctionLength
    0x24,       // bDescriptorType: CS_INTERFACE
    0x02,       // bDescriptorSubtype: Abstract Control Management desc
    0x02,       // bmCapabilities

    // Union Functional Descriptor
    0x05,       // bFunctionLength
    0x24,       // bDescriptorType: CS_INTERFACE
    0x06,       // bDescriptorSubtype: Union func desc
    0x00,       // bMasterInterface: Communication class interface number (Control)
    0x01,       // bSlaveInterface[0]: Data Class Interface number

    // Data Interface Descriptor (for data transfer)
    0x09,       // bLength: Size of this descriptor in bytes
    0x04,       // bDescriptorType: Interface
    0x01,       // bInterfaceNumber: Number of this interface (Data)
    0x00,       // bAlternateSetting: Alternate setting number for this interface.
    0x02,       // bNumEndpoints: Two endpoints used for data transfer.
    0x0A,       // bInterfaceClass: Data class (CDC)
    0x00,       // bInterfaceSubClass:
    0x00,       // bInterfaceProtocol:
    0x00,       // iInterface:

    // Endpoint Descriptor (IN)
    0x07,               // bLength: Endpoint Descriptor size
    0x05,               // bDescriptorType: Endpoint
    CDC_ACM_EP_IN,     // bEndpointAddress: Endpoint Address (IN)
    0x02,               // bmAttributes: Bulk endpoint (2)
    0x40,         // wMaxPacketSize:
    0x00,         //
    0x00                // bInterval:
    
   ,// Endpoint Descriptor (OUT)
   0x07 ,             	//bLength : Endpoint Descriptor size 
  	0X05 ,             	//bDescriptorType : Endpoint 
  	CDC_ACM_EP_OUT ,  	//bEndpointAddress : Endpoint Address (OUT) 
  	0X02 ,             	//bmAttributes : Bulk endpoint 
  	0x40,         // wMaxPacketSize:
    0x00,         //
  	0X00               	//bInterval : 
};


void cdc_acm_init(void);
void cdc_acm_task(void);
bool cdc_acm_send_data(const uint8_t* data, uint32_t length);
uint32_t cdc_acm_read_data(uint8_t* buffer, uint32_t max_length);

#ifdef	__cplusplus
}
#endif

#endif	/* CDC_ACM_USB_H */

