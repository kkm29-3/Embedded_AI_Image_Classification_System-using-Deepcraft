#ifndef _USB_CAMERA_TASK_H_
#define _USB_CAMERA_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Header Files
*******************************************************************************/
#include <stdio.h>

/* FreeRTOS header file */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "cyabs_rtos.h"
#include "cyabs_rtos_impl.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* YUYV 422 encoding - 2 bytes per pixel */

#define CAMERA_BUFFER_SIZE ((CAMERA_WIDTH) * (CAMERA_HEIGHT) * 2) 
#define FORMAT USBH_VIDEO_VS_FORMAT_UNCOMPRESSED
/* Configurable FPS Logitech C920e */
#define FRAME_INTERVAL_1 (1000000)
/* 30 FPS 0.3 MP cam */ 
#define FRAME_INTERVAL_2 (333332) 
/* 30 FPS 2 MP cam */
#define FRAME_INTERVAL (333333) 

#define MAX_VIDEO_INTERFACES 4

#define TASK_PRIO_USBH_MAIN         (configMAX_PRIORITIES - 2)
#define TASK_PRIO_USBH_ISR          (configMAX_PRIORITIES - 1)

#define LOGI_TECH_C920_VID  0x046D
/* Logitech C920 */
#define LOGI_TECH_C920_PID  0x08E5
/* Logitech C920e */
#define LOGI_TECH_C920e_PID 0x08B6            

/* 0.3MP HBVCAM */
#define HBV_CAM_0P3_VID  0x058F         
#define HBV_CAM_0P3_PID  0x5608

/* 2MP HBVCAM */
#define HBV_CAM_2P0_VID  0x05A3         
#define HBV_CAM_2P0_PID  0x2B01

extern bool logitech_camera_enabled;
extern bool point3mp_camera_enabled;
extern bool Camera_not_supported;

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct video_buffer
{
    uint32_t num_bytes; /* Number of video data bytes in the buffer. */ 
    uint8_t buff_ready; /* Buffer ready flag */ 
} video_buffer_t;

/*******************************************************************************
* Global Variables
*******************************************************************************/
extern video_buffer_t    _image_buff[];
extern uint8_t          last_buffer;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _USB_CAMERA_TASK_H_ */
