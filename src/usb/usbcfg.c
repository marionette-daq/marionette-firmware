/*! \file usbcfg.c
 * \defgroup usb_descriptor  USB Descriptor
 * @{
 */

/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

#include <string.h>

#if STM32_USB_USE_OTG2
# define USBD_PERIPHERIAL   USBD2
#else
# warning "USB needs to be configured for OTG2/ULPI HS"
# define USBD_PERIPHERIAL   USBD1
#endif

#define USB_MAX_PACKET_SIZE                    512
#define USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE     SERIAL_USB_BUFFERS_SIZE
#define USB_CDC_INTERUPT_INTERVAL              0x07 // 2**(x-1) * 125e-6 = 8ms

/*see www.usb.org/developers/whitepapers/iadclasscode_r10.pdf*/
#define MULTI_FUNCTION_DEVICE_CLASS       0xEF
#define MULTI_FUNCTION_SUB_CLASS          0x02
#define MULTI_FUNCTION_DEVICE_PROTOCOL    0x01

// The following VID/PID pair belongs to APDM Inc. with permission to use on the Marionette project temporarily
#define MARIONETTE_APDM_USB_VID           0x224F
#define MARIONETTE_APDM_USB_PID           0xff00

/*
 * Endpoints to be used for USBD.
 */
#define USBD_DATA_REQUEST_SDU1_EP1           1
#define USBD_DATA_AVAILABLE_SDU1_EP1         1
#define USBD_INTERRUPT_REQUEST_SDU1_EP2      2

#define USBD_DATA_REQUEST_SDU2_EP3           3
#define USBD_DATA_AVAILABLE_SDU2_EP3         3
#define USBD_INTERRUPT_REQUEST_SDU2_EP4      4

SerialUSBDriver SDU1;
SerialUSBDriver SDU2;

/*!
 * USB Device Descriptor.
 */
static const uint8_t vcom_device_descriptor_data[18] = {
  USB_DESC_DEVICE       (0x0200,                          /* bcdUSB(2.0).                     */
                         MULTI_FUNCTION_DEVICE_CLASS,     /* bDeviceClass (CDC).              */
                         MULTI_FUNCTION_SUB_CLASS,        /* bDeviceSubClass.                 */
                         MULTI_FUNCTION_DEVICE_PROTOCOL,  /* bDeviceProtocol.                 */
                         0x0040,                          /* bMaxPacketSize.                  */
                         MARIONETTE_APDM_USB_VID,         /* idVendor (APDM).                 */
                         MARIONETTE_APDM_USB_PID,         /* idProduct.                       */
                         0x0200,                          /* bcdDevice.                       */
                         1,                               /* iManufacturer.                   */
                         2,                               /* iProduct.                        */
                         3,                               /* iSerialNumber.                   */
                         1)                               /* bNumConfigurations.              */
};

/*! Configuration Descriptor tree for a CDC.*/
static const uint8_t vcom_configuration_descriptor_data[141] = {
  /* Configuration Descriptor.*/
  USB_DESC_CONFIGURATION(141,            /* wTotalLength.                    */ // FIXME make sure this is correct
                         0x04,          /* bNumInterfaces.                  */
                         0x01,          /* bConfigurationValue.             */
                         0,             /* iConfiguration.                  */
                         0xC0,          /* bmAttributes (self powered).     */ // FIXME change to bus powered, 0x80
                         50),           /* bMaxPower (100mA).               */ // FIXME up this to 500mA, 0xFA


  /* Interface Associateion Desriptor (IAD) */
  0x08, /*Length of IAD */ \
  USB_DESCRIPTOR_INTERFACE_ASSOCIATION, \
  0x00, /* bFirstInterface */ \
  0x02, /* bInterfaceCount */ \
  CDC_COMMUNICATION_INTERFACE_CLASS, /* bFunctionClass */ \
  CDC_ABSTRACT_CONTROL_MODEL, /* bFunctionSubClass */ \
  0x01, /* bFunctionProcotol */ \
  0x00, /* iInterface */ \

  /* Interface Descriptor CDC0.*/
  USB_DESC_INTERFACE    (0x00,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x01,          /* bNumEndpoints.                   */
                         0x02,          /* bInterfaceClass (Communications
                                           Interface Class, CDC section
                                           4.2).                            */
                         0x02,          /* bInterfaceSubClass (Abstract
                                         Control Model, CDC section 4.3).   */
                         0x01,          /* bInterfaceProtocol (AT commands,
                                           CDC section 4.4).                */
                         4),            /* iInterface.                      */
  /* Header Functional Descriptor (CDC section 5.2.3).*/
  USB_DESC_BYTE         (5),            /* bLength.                         */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x00),         /* bDescriptorSubtype (Header
                                           Functional Descriptor.           */
  USB_DESC_BCD          (0x0110),       /* bcdCDC.                          */
  /* Call Management Functional Descriptor. */
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x01),         /* bDescriptorSubtype (Call Management
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x00),         /* bmCapabilities (D0+D1).          */
  USB_DESC_BYTE         (0x01),         /* bDataInterface.                  */
  /* ACM Functional Descriptor.*/
  USB_DESC_BYTE         (4),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x02),         /* bDescriptorSubtype (Abstract
                                           Control Management Descriptor).  */
  USB_DESC_BYTE         (0x02),         /* bmCapabilities.                  */
  /* Union Functional Descriptor.*/
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x06),         /* bDescriptorSubtype (Union
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x00),         /* bMasterInterface (Communication
                                           Class Interface).                */
  USB_DESC_BYTE         (0x01),         /* bSlaveInterface0 (Data Class
                                           Interface).                      */
  /* Endpoint 2 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_INTERRUPT_REQUEST_SDU1_EP2|0x80,
                         0x03,          /* bmAttributes (Interrupt).        */
                         0x0008,        /* wMaxPacketSize.                  */
                         USB_CDC_INTERUPT_INTERVAL),         /* bInterval.                       */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x01,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x02,          /* bNumEndpoints.                   */
                         0x0A,          /* bInterfaceClass (Data Class
                                           Interface, CDC section 4.5).     */
                         0x00,          /* bInterfaceSubClass (CDC section
                                           4.6).                            */
                         0x00,          /* bInterfaceProtocol (CDC section
                                           4.7).                            */
                         0x00),         /* iInterface.                      */
  /* Endpoint 3 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_DATA_AVAILABLE_SDU1_EP1,       /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,        /* wMaxPacketSize.                  */
                         0x00),         /* bInterval.                       */
  /* Endpoint 1 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_DATA_REQUEST_SDU1_EP1|0x80,    /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,        /* wMaxPacketSize.                  */
                         0x00),         /* bInterval.                       */
  
  /* Interface Associateion Desriptor (IAD) */
  0x08, /*Length of IAD */ \
  USB_DESCRIPTOR_INTERFACE_ASSOCIATION, \
  0x02, /* bFirstInterface */ \
  0x02, /* bInterfaceCount */ \
  CDC_COMMUNICATION_INTERFACE_CLASS, /* bFunctionClass */ \
  CDC_ABSTRACT_CONTROL_MODEL, /* bFunctionSubClass */ \
  0x01, /* bFunctionProcotol */ \
  0x00, /* iInterface */ \

  /* Interface Descriptor CDC1.*/
  USB_DESC_INTERFACE    (0x02,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x01,          /* bNumEndpoints.                   */
                         0x02,          /* bInterfaceClass (Communications
                                           Interface Class, CDC section
                                           4.2).                            */
                         0x02,          /* bInterfaceSubClass (Abstract
                                         Control Model, CDC section 4.3).   */
                         0x01,          /* bInterfaceProtocol (AT commands,
                                           CDC section 4.4).                */
                         5),            /* iInterface.                      */
  /* Header Functional Descriptor (CDC section 5.2.3).*/
  USB_DESC_BYTE         (5),            /* bLength.                         */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x00),         /* bDescriptorSubtype (Header
                                           Functional Descriptor.           */
  USB_DESC_BCD          (0x0110),       /* bcdCDC.                          */
  /* Call Management Functional Descriptor. */
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x01),         /* bDescriptorSubtype (Call Management
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x00),         /* bmCapabilities (D0+D1).          */
  USB_DESC_BYTE         (0x01),         /* bDataInterface.                  */
  /* ACM Functional Descriptor.*/
  USB_DESC_BYTE         (4),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x02),         /* bDescriptorSubtype (Abstract
                                           Control Management Descriptor).  */
  USB_DESC_BYTE         (0x02),         /* bmCapabilities.                  */
  /* Union Functional Descriptor.*/
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x06),         /* bDescriptorSubtype (Union
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x02),         /* bMasterInterface (Communication
                                           Class Interface).                */
  USB_DESC_BYTE         (0x03),         /* bSlaveInterface0 (Data Class
                                           Interface).                      */
  /* Endpoint 2 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_INTERRUPT_REQUEST_SDU2_EP4|0x80,
                         0x03,          /* bmAttributes (Interrupt).        */
                         0x0008,        /* wMaxPacketSize.                  */
                         USB_CDC_INTERUPT_INTERVAL),         /* bInterval.                       */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x03,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x02,          /* bNumEndpoints.                   */
                         0x0A,          /* bInterfaceClass (Data Class
                                           Interface, CDC section 4.5).     */
                         0x00,          /* bInterfaceSubClass (CDC section
                                           4.6).                            */
                         0x00,          /* bInterfaceProtocol (CDC section
                                           4.7).                            */
                         0x00),         /* iInterface.                      */
  /* Endpoint 3 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_DATA_AVAILABLE_SDU2_EP3,       /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,        /* wMaxPacketSize.                  */
                         0x00),         /* bInterval.                       */
  /* Endpoint 1 Descriptor.*/
  USB_DESC_ENDPOINT     (USBD_DATA_REQUEST_SDU2_EP3|0x80,    /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,        /* wMaxPacketSize.                  */
                         0x00)          /* bInterval.                       */
};

/*!
 * Device Descriptor wrapper.
 */
static const USBDescriptor vcom_device_descriptor = {
  sizeof vcom_device_descriptor_data,
  vcom_device_descriptor_data
};

/*
 * Configuration Descriptor wrapper.
 */
static const USBDescriptor vcom_configuration_descriptor = {
  sizeof vcom_configuration_descriptor_data,
  vcom_configuration_descriptor_data
};

/*
 * U.S. English language identifier.
 */
static const uint8_t vcom_string0[] = {
  USB_DESC_BYTE(4),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  USB_DESC_WORD(0x0409)                 /* wLANGID (U.S. English).          */
};

/*
 * Vendor string.
 */
static const uint8_t vcom_string1[] = {
  USB_DESC_BYTE(10),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'A', 0, 'P', 0, 'D', 0, 'M', 0
};

/*
 * Device Description string.
 */
static const uint8_t vcom_string2[] = {
  USB_DESC_BYTE(22),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'M', 0, 'a', 0, 'r', 0, 'i', 0, 'o', 0, 'n', 0, 'e', 0, 't', 0,
  't', 0, 'e', 0
};

/*
 * Serial Number string.
 */
static uint8_t vcom_string3[] = {
  USB_DESC_BYTE(50),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
  '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
  '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
};

/* Control Interface String. */
static const uint8_t vcom_string4[] = {
  USB_DESC_BYTE(16),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'C', 0, 'o', 0, 'n', 0, 't', 0, 'r', 0, 'o', 0, 'l', 0
};

/* Data Interface String. */
static const uint8_t vcom_string5[] = {
  USB_DESC_BYTE(10),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'D', 0, 'a', 0, 't', 0, 'a', 0
};


/*
 * Strings wrappers array.
 */
static const USBDescriptor vcom_strings[] = {
  {sizeof vcom_string0, vcom_string0},
  {sizeof vcom_string1, vcom_string1},
  {sizeof vcom_string2, vcom_string2},
  {sizeof vcom_string3, vcom_string3},
  {sizeof vcom_string4, vcom_string4},
  {sizeof vcom_string5, vcom_string5}
};

/*
 * Handles the GET_DESCRIPTOR callback. All required descriptors must be
 * handled here.
 */
static const USBDescriptor *get_descriptor(USBDriver *usbp,
                                           uint8_t dtype,
                                           uint8_t dindex,
                                           uint16_t lang) {

  (void)usbp;
  (void)lang;
  switch (dtype) {
  case USB_DESCRIPTOR_DEVICE:
    return &vcom_device_descriptor;
  case USB_DESCRIPTOR_CONFIGURATION:
    return &vcom_configuration_descriptor;
  case USB_DESCRIPTOR_STRING:
    if (dindex < 6)
      return &vcom_strings[dindex];
  }
  return NULL;
}

/**
 * @brief   IN EP1 state.
 */
static USBInEndpointState ep1instate;

/**
 * @brief   OUT EP1 state.
 */
static USBOutEndpointState ep1outstate;

/**
 * @brief   EP1 initialization structure (both IN and OUT).
 */
static const USBEndpointConfig ep1config = {
  USB_EP_MODE_TYPE_BULK,
  NULL,
  sduDataTransmitted,
  sduDataReceived,
  USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,
  USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,
  &ep1instate,
  &ep1outstate,
  2,
  NULL
};

/**
 * @brief   IN EP2 state.
 */
static USBInEndpointState ep2instate;

/**
 * @brief   EP2 initialization structure (IN only).
 */
static const USBEndpointConfig ep2config = {
  USB_EP_MODE_TYPE_INTR,
  NULL,
  sduInterruptTransmitted,
  NULL,
  0x0010,
  0x0000,
  &ep2instate,
  NULL,
  1,
  NULL
};

/**
 * @brief   IN EP3 state.
 */
static USBInEndpointState ep3instate;

/**
 * @brief   OUT EP3 state.
 */
static USBOutEndpointState ep3outstate;

/**
 * @brief   EP3 initialization structure (both IN and OUT).
 */
static const USBEndpointConfig ep3config = {
  USB_EP_MODE_TYPE_BULK,
  NULL,
  sduDataTransmitted,
  sduDataReceived,
  USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,
  USB_CDC_DESCRIPTOR_MAX_PACKET_SIZE,
  &ep3instate,
  &ep3outstate,
  2,
  NULL
};

/**
 * @brief   IN EP4 state.
 */
static USBInEndpointState ep4instate;

/**
 * @brief   EP4 initialization structure (IN only).
 */
static const USBEndpointConfig ep4config = {
  USB_EP_MODE_TYPE_INTR,
  NULL,
  sduInterruptTransmitted,
  NULL,
  0x0010,
  0x0000,
  &ep4instate,
  NULL,
  1,
  NULL
};
/*
 * Handles the USB driver global events.
 */
static void usb_event(USBDriver *usbp, usbevent_t event) {

  switch (event) {
  case USB_EVENT_RESET:
    return;
  case USB_EVENT_ADDRESS:
    return;
  case USB_EVENT_CONFIGURED:
    chSysLockFromISR();

    if(usbp->state == USB_ACTIVE)
    {
      /* Enables the endpoints specified into the configuration.
         Note, this callback is invoked from an ISR so I-Class functions
         must be used.*/
      usbInitEndpointI(usbp, USBD_DATA_REQUEST_SDU1_EP1, &ep1config);
      usbInitEndpointI(usbp, USBD_INTERRUPT_REQUEST_SDU1_EP2, &ep2config);

      usbInitEndpointI(usbp, USBD_DATA_REQUEST_SDU2_EP3, &ep3config);
      usbInitEndpointI(usbp, USBD_INTERRUPT_REQUEST_SDU2_EP4, &ep4config);

      /* Resetting the state of the CDC subsystem.*/
      sduConfigureHookI(&SDU1);
      sduConfigureHookI(&SDU2);
    }
    else if(usbp->state == USB_SELECTED)
    {
      usbDisableEndpointsI(usbp);
    }

    chSysUnlockFromISR();
    return;
  case USB_EVENT_SUSPEND:
    chSysLockFromISR();
    /* Disconnection event on suspend.*/
    sduDisconnectI(&SDU1);
    sduDisconnectI(&SDU2);
    chSysUnlockFromISR();    
    return;
  case USB_EVENT_WAKEUP:
    return;
  case USB_EVENT_STALLED:
    return;
  }
  return;
}

char to_hex_char(uint8_t v) {
	v = (v & 0x0F);

	if (v < 10) {
		return ('0' + v);
	} else {
		return ('A' + (v - 10));
	}
}

void usb_set_serial_strings(const uint32_t high, const uint32_t mid, const uint32_t low) {
	uint8_t src[sizeof(high) + sizeof(mid) + sizeof(low)];
	memcpy(&src[0], &high, sizeof(high));
	memcpy(&src[sizeof(high)], &mid, sizeof(mid));
	memcpy(&src[sizeof(high) + sizeof(mid)], &low, sizeof(low));

	uint32_t src_idx = 0;
	uint32_t dest_idx = 2;
	for (; dest_idx < sizeof(vcom_string3); dest_idx += 2) {
		vcom_string3[dest_idx] = to_hex_char(src[src_idx]);
		dest_idx += 2;
		if( dest_idx < sizeof(vcom_string3) ) {
			vcom_string3[dest_idx] = to_hex_char(src[src_idx] >> 4);
		}
		src_idx++;
	}
}

/*
 * Handling messages not implemented in the default handler nor in the
 * SerialUSB handler.
 */
static bool requests_hook(USBDriver *usbp) {

  if (((usbp->setup[0] & USB_RTYPE_RECIPIENT_MASK) == USB_RTYPE_RECIPIENT_INTERFACE) &&
      (usbp->setup[1] == USB_REQ_SET_INTERFACE)) {
    usbSetupTransfer(usbp, NULL, 0, NULL);
    return true;
  }
  return sduRequestsHook(usbp);
}

/*
 * Handles the USB driver global events.
 */
static void sof_handler(USBDriver *usbp) {
  (void)usbp;

  osalSysLockFromISR();
  sduSOFHookI(&SDU1);
  sduSOFHookI(&SDU2);
  osalSysUnlockFromISR();
}

/*
 * USB driver configuration.
 */
const USBConfig usbcfg = {
  usb_event,
  get_descriptor,
  requests_hook,
  sof_handler
};

/*!
 * Serial over USB driver configuration.
 */

const SerialUSBConfig serusbcfg  = {
  &USBD_PERIPHERIAL,
  USBD_DATA_REQUEST_SDU1_EP1,
  USBD_DATA_AVAILABLE_SDU1_EP1,
  USBD_INTERRUPT_REQUEST_SDU1_EP2
};

/*!
 * Serial over USB driver configuration.
 */

const SerialUSBConfig serusbcfg2 = {
  &USBD_PERIPHERIAL,
  USBD_DATA_REQUEST_SDU2_EP3,
  USBD_DATA_AVAILABLE_SDU2_EP3,
  USBD_INTERRUPT_REQUEST_SDU2_EP4
};



//! @}



