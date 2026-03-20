#include <math.h>
#include <inttypes.h>
#include <stdint.h>
#include "cybsp.h"
#include "cy_pdl.h"
#include "cyabs_rtos.h"
#include "cyabs_rtos_impl.h"
/* FreeRTOS header file */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "FreeRTOSConfig.h"
#include "stdlib.h"
#include "model.h"
#include "ifx_image_utils.h"
#include "lcd_task.h"
#include "inference_task.h"
#include "ifx_time_utils.h"
#include "mtb_ml_utils.h"
#include "mtb_ml_common.h"
#include "mtb_ml.h"

#include "text.h"
/*******************************************************************************
 * Global Variable
 *******************************************************************************/
/* Performance measure: Buffer wait time */ 
float prep_wait_buf; 
/* Performance measure: YUV422 to BGR565 conversion time */              
float prep_YUV422_to_bgr565; 
/* Performance measure: BGR565 to display time */      
float prep_bgr565_to_disp;   
/* Performance measure: RGB565 to RGB888 conversion time */      
float prep_RGB565_to_RGB888; 
      
#ifdef USE_DVP_CAM
extern bool frame_ready;
#endif

__attribute__((section(".cy_socmem_data")))
/* Final output variable for object detection */
__attribute__((aligned(16))) prediction_od_t prediction;

//for 1001 classes/ images
#define IMAI_DATAOUT_COUNT 1001
float data_out[IMAI_DATAOUT_COUNT] = {0};
/* Inference execution time */
volatile float inference_time = 0;

/* The best class out of all i.e. the class with max value out of all classes */
float max_class_val = 0;

/*******************************************************************************
 * Function Name: get_image
 *
 * Description:
 *   Retrieves the latest image by calling the draw function.
 *
 * Input Arguments:
 *   None
 *
 * Return Value:
 *   uint8_t* - Pointer to the image buffer
 *
 *******************************************************************************/
static float * get_image()
{
    return draw();
}

/*******************************************************************************
* Function Name: get_best_class
*******************************************************************************
*
* Summary:
*  The function calculates the best class out of all available classes.
*
* Parameters:
*  cls  - Pointer to the class array.
*  size - number of classes for the model i.e. size of class array.
*  max_cls_val - pointer to store the the best class out of all i.e. 
*                the max of all classes
*
* Return:
*  max_index - Returns the idex of the best class out of all i.e. 
*              the max of all classes
*
*******************************************************************************/
int16_t get_best_class(const float *cls, size_t size, float *max_cls_val) {
    if (cls == NULL || size == 0) {
        return -1;
    }

    int32_t max_index = 0;   // was int8_t — OVERFLOW BUG
    float max_value = cls[0];

    for (int32_t i = 1; i < (int32_t)size; i++) {  // was int8_t — OVERFLOW BUG
        if (cls[i] > max_value) {
            max_value = cls[i];
            max_index = i;
        }
    }

    *max_cls_val = max_value;
    return (int16_t)max_index;
}

/*******************************************************************************
 * Function Name: cm55_inference_task
 *
 * Description:
 *   Main task for running object detection inference on the CM55 core. Initializes
 *   the object detection model, preprocesses input images, performs inference, and
 *   postprocesses results. Updates performance metrics and signals the graphics
 *   display semaphore.
 *
 * Input Arguments:
 *   void *arg - Task argument (not used)
 *
 * Return Value:
 *   None
 *
 *******************************************************************************/
void cm55_inference_task( void *arg )
{
    int result = IMAI_init();
    volatile float _time_start_prev = 0;
    (void)_time_start_prev;

    /* ADD THIS LINE: We need this to safely access the model's memory */
//        IMAI_api_def *api_def = IMAI_api();

    if(result != 0)
        printf("Failed to initialize the model\r\n");

    while (true)
    {
#ifdef USE_DVP_CAM
        if (frame_ready == true)
#endif
        {
            volatile float _time_start = ifx_time_get_ms_f();
#ifdef USE_DVP_CAM
            frame_ready = false;
#endif

//            /* 1. Get the latest input image */
//            float *image_buf_uint8 = get_image();
//            cy_rtos_delay_milliseconds(1);
//
//            /* 2. Run Inference & Capture Status */
//            volatile float _time_object_det = ifx_time_get_ms_f();
//            IMAI_compute(image_buf_uint8, data_out);

//            /* 1. Get the raw image bytes from the camera */
//			uint8_t *raw_image_bytes = (uint8_t *)get_image();
//			cy_rtos_delay_milliseconds(1);
//
//			/* ------------------------------------------------------------------ */
//			/* CRITICAL ACCURACY FIX: In-Place BGR to RGB Conversion              */
//			/* We swap the colors directly in the camera memory. (ZERO RAM cost!) */
//			/* ------------------------------------------------------------------ */
//			int total_pixels = IMAGE_WIDTH * IMAGE_HEIGHT;
//			for (int i = 0; i < (total_pixels * 3); i += 3) {
//				uint8_t temp_blue = raw_image_bytes[i];          // Save the Blue pixel
//				raw_image_bytes[i] = raw_image_bytes[i + 2];     // Move Red into Blue's spot
//				raw_image_bytes[i + 2] = temp_blue;              // Move Blue into Red's spot
//				// Green (i + 1) stays exactly where it is!
//			}
//			/* ------------------------------------------------------------------ */
//
//			/* 2. Run Inference using the corrected RGB colors */
//			volatile float _time_object_det = ifx_time_get_ms_f();
//
//			// We cast it to (float *) just to keep the compiler happy,
//			// but the AI will read the raw 8-bit RGB integers it expects!
//			IMAI_compute((float *)raw_image_bytes, data_out);


//            /* 1. Get raw camera buffer (assumed BGR uint8, 224x224x3) */
//            uint8_t *raw_image_bytes = (uint8_t *)get_image();
//            cy_rtos_delay_milliseconds(1);
//
//            /* 2. Proper float buffer + RGB + [0,1] normalization */
//            static float input_float[150528];  // 224*224*3
//
//            int total_pixels = IMAGE_WIDTH * IMAGE_HEIGHT;  // must be 224x224!
//            int idx = 0;
//            for (int i = 0; i < total_pixels * 3; i += 3) {
//                uint8_t b = raw_image_bytes[i + 0];
//                uint8_t g = raw_image_bytes[i + 1];
//                uint8_t r = raw_image_bytes[i + 2];
//
//                // BGR → RGB + divide by 255 → [0,1]
//                input_float[idx++] = r / 255.0f;   // Red
//                input_float[idx++] = g / 255.0f;   // Green
//                input_float[idx++] = b / 255.0f;   // Blue
//            }
//
//            /* 3. Run inference */
//            volatile float _time_object_det = ifx_time_get_ms_f();
//            IMAI_compute(input_float, data_out);


            //working part
            /* 1. Get the raw image bytes from the camera (0 to 255) */
			uint8_t *raw_image_bytes = (uint8_t *)get_image();
			cy_rtos_delay_milliseconds(1);

			/* ------------------------------------------------------------------ */
			/* CRITICAL ACCURACY FIX: Range Shift & Color Swap for int8x8 Models  */
			/* ------------------------------------------------------------------ */
			// We cast the exact same memory space to signed int8 to save RAM
			int8_t *signed_image_bytes = (int8_t *)raw_image_bytes;

			int total_pixels = IMAGE_WIDTH * IMAGE_HEIGHT;
			for (int i = 0; i < (total_pixels * 3); i += 3)
			{
				// Read the original unsigned BGR values first so we don't overwrite them
				uint8_t b = raw_image_bytes[i + 0];
				uint8_t g = raw_image_bytes[i + 1];
				uint8_t r = raw_image_bytes[i + 2];

				// Swap to RGB AND subtract 128 to shift from [0, 255] to [-128, 127]
				signed_image_bytes[i + 0] = (int8_t)(r - 128); // Red
				signed_image_bytes[i + 1] = (int8_t)(g - 128); // Green
				signed_image_bytes[i + 2] = (int8_t)(b - 128); // Blue
			}

			/* 2. Run Inference using the signed int8 array */
			volatile float _time_object_det = ifx_time_get_ms_f();

			// We pass the signed bytes. We keep the (float *) cast strictly
			// to stop the compiler from complaining, but the AI will read it as int8!
			IMAI_compute((float *)signed_image_bytes, data_out);

            /* 3. Find the highest probability class (out of 1001) */
            // Note: Ensure get_best_class uses int32_t to avoid overflow!
            int16_t best_class_id = get_best_class(data_out, 1001, &max_class_val);

            /* 4. Copy the matching string from text.h to the prediction struct */
            if (best_class_id >= 0 && best_class_id < 1001) {
                strncpy(prediction.class_string[0], imagenet_labels[best_class_id], MAX_CLASS_LEN - 1);
                prediction.class_string[0][MAX_CLASS_LEN - 1] = '\0'; // Ensure null termination
            } else {
                snprintf(prediction.class_string[0], MAX_CLASS_LEN, "Unknown");
            }

            /* 5. Update prediction output variables */
            prediction.class_id[0] = (uint8_t)(best_class_id & 0xFF);
            prediction.conf[0] = max_class_val;
            prediction.count = 1;

            /* --- TERA TERM CONSOLE OUTPUT --- */
            printf("\r\n--- Frame Processed ---\r\n");
            printf("ID: %d | Conf: %.2f | Label: %s\r\n",
                   best_class_id, max_class_val, prediction.class_string[0]);

            volatile float _time_end = ifx_time_get_ms_f();
            inference_time = _time_end - _time_object_det;

            result = cy_rtos_semaphore_set(&model_semaphore);
            if (CY_RSLT_SUCCESS != result) {
                printf("\r\nModel Semphore set failed\r\n");
            }

            _time_start_prev = _time_start;
        }
    }
}
