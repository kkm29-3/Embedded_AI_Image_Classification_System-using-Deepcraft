#ifndef _PROJ_CM55_INFERENCE_TASK_H_
#define _PROJ_CM55_INFERENCE_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <math.h>

/*******************************************************************************
* Macros
*******************************************************************************/
#define NUM_CLASSES             1001
#define MAX_CLASS_LEN           100
#define MAX_PREDICTIONS         5
/* Default USB camera input dimensions. */ 
#define CAMERA_WIDTH            320     
#define CAMERA_HEIGHT           240
/* Model input dimensions, hard coded for now to help on LCD graphics maintain an image buffer of model size. */
#define IMAGE_WIDTH             224
#define IMAGE_HEIGHT            224
/* If USB webcam stream is sharding, skip some frames (inference every FRAMES_TO_SKIP frames) */
#define FRAMES_TO_SKIP          2        
#define FRAMES_TO_SKIP_LOGITECH 4

/* Object Detection Configuration. */ 
/* Scaling factor */      
#define HALF(x)                 ((x) * 0.5f)
/* Rounding factor */    
#define RND_F2I_FACTOR          0.5f     

#ifndef max
    #define max(a, b)   ((a) > (b) ? (a) : (b))
    #define min(a, b)   ((a) < (b) ? (a) : (b))
#endif

/******************************************************************************
 * Global Variables - struct
 *****************************************************************************/
/* Final output variables */ 
typedef struct {
    int32_t     count;
    int16_t     bbox_int16[MAX_PREDICTIONS * 4];
    float       conf[MAX_PREDICTIONS];
    uint8_t     class_id[MAX_PREDICTIONS];
    char        class_string[MAX_PREDICTIONS][MAX_CLASS_LEN];
} prediction_od_t;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
int16_t get_best_class(const float *cls, size_t size, float *max_cls_val);


#if defined(__cplusplus)
}
#endif /* __cplusplus */


#endif /* _PROJ_CM55_INFERENCE_TASK_H_ */
