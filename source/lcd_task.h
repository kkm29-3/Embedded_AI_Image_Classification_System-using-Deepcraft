#ifndef _LCD_TASK_H_
#define _LCD_TASK_H_

/*******************************************************************************
 * Included Headers
 *******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cyabs_rtos.h"
#include "vg_lite.h"
#include "vg_lite_platform.h"
#include "inference_task.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* Number of image buffers */
#define NUM_IMAGE_BUFFERS                   ( 2 )  

#define INFERENCE_TASK_NAME                 ( "CM55 Inference Task" )
#define INFERENCE_TASK_STACK_SIZE           ( 64U * 1024U )
#define INFERENCE_TASK_PRIORITY             ( configMAX_PRIORITIES - 4)
/*******************************************************************************
 * Global Variables
 *******************************************************************************/
/* Model semaphore for synchronization */
extern cy_semaphore_t model_semaphore;  
/* BGR888 integer buffer */
extern uint8_t bgr888_uint8[];           

/*******************************************************************************
 * Local Variables
 *******************************************************************************/
/* Render target buffer */
extern vg_lite_buffer_t *render_target; 
/* USB YUV frame buffers */        
extern vg_lite_buffer_t usb_yuv_frames[];      

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
float *draw(void);
void mirror_image(vg_lite_buffer_t *buffer);
void update_box_data(vg_lite_buffer_t *render_target, prediction_od_t *prediction);
void update_box_data1(vg_lite_buffer_t *render_target, float num);
void VG_LITE_ERROR_EXIT(char *msg, vg_lite_error_t vglite_status);
void VG_switch_frame(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LCD_TASK_H_ */
