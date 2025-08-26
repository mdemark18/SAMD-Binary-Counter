#include "cdc_acm_usb.h"
#include "hal_usb_device.h"

static uint8_t cdc_acm_rx_buffer[64];
static uint32_t cdc_acm_rx_count = 0;

static bool cdc_acm_setup_callback(const uint8_t ep, const uint8_t *req)
{
    if (req[0] & 0x80){
        if (req[1] == 0x06){
            if(req[3] == 0x01) {
                struct usb_d_transfer transfer = {
                    .ep = ep,
                    .buf = (uint8_t*)device_descriptor,
                    .size = sizeof(device_descriptor)
                };

                if(usb_d_ep_transfer(&transfer) != 0) return false;

                return true;
            }
            else if(req[3] == 0x02){
                struct usb_d_transfer transfer = {
                    .ep = ep,
                    .buf = (uint8_t*)config_descriptor,
                    .size = sizeof(config_descriptor)
                };

                if(usb_d_ep_transfer(&transfer) != 0) return false;

                return true;
            }
        }
    }
    // Handle other requests...
    return false;
}

static void cdc_acm_xfer_callback(const uint8_t ep, const enum usb_xfer_code code, void *param)
{
    if (ep == CDC_ACM_EP_OUT && code == USB_TRANS_DONE) {
        cdc_acm_rx_count = *(uint32_t*)param;
    }
}

void cdc_acm_init(void)
{
    int32_t status;
    status = usb_d_init();
    usb_d_register_callback(USB_D_CB_SOF, NULL);
    usb_d_register_callback(USB_D_CB_EVENT, NULL);
    
    status = usb_d_ep0_init(64);
    status = usb_d_ep_init(CDC_ACM_EP_IN, USB_EP_XTYPE_BULK, 64);
    status = usb_d_ep_init(CDC_ACM_EP_OUT, USB_EP_XTYPE_BULK, 64);
    
    status = usb_d_ep_enable(CDC_ACM_EP_IN);
    status = usb_d_ep_enable(CDC_ACM_EP_OUT);

    
    usb_d_ep_register_callback(0, USB_D_EP_CB_SETUP, (FUNC_PTR)cdc_acm_setup_callback);
    usb_d_ep_register_callback(CDC_ACM_EP_IN, USB_D_EP_CB_XFER, (FUNC_PTR)cdc_acm_xfer_callback);
    usb_d_ep_register_callback(CDC_ACM_EP_OUT, USB_D_EP_CB_XFER, (FUNC_PTR)cdc_acm_xfer_callback);
    
    status = usb_d_enable();
    usb_d_attach();
}

void cdc_acm_task(void)
{
    uint8_t buffer[64];
    uint32_t received = cdc_acm_read_data(buffer, sizeof(buffer));
    
    if (received > 0) {
        cdc_acm_send_data(buffer, received);
    }
}


bool cdc_acm_send_data(const uint8_t* data, uint32_t length)
{
    struct usb_d_transfer xfer = {
        .ep = CDC_ACM_EP_IN,
        .buf = (uint8_t*)data,
        .size = length
    };
    return (usb_d_ep_transfer(&xfer) == 0);
}

uint32_t cdc_acm_read_data(uint8_t* buffer, uint32_t max_length)
{
    uint32_t read_count = (cdc_acm_rx_count < max_length) ? cdc_acm_rx_count : max_length;
    for (uint32_t i = 0; i < read_count; i++) {
        buffer[i] = cdc_acm_rx_buffer[i];
    }
    cdc_acm_rx_count = 0;
    
    struct usb_d_transfer xfer = {
        .ep = CDC_ACM_EP_OUT,
        .buf = cdc_acm_rx_buffer,
        .size = sizeof(cdc_acm_rx_buffer)
    };
    usb_d_ep_transfer(&xfer);
    
    return read_count;
}
