/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Proyecto del curso
Código implementado en el Arduino 
Ciclo: II-2022
*/

// Librerías a utilizar
#include <Mask_recognition_inferencing.h>
#include <Arduino_OV767X.h> 
#include <stdint.h>
#include <stdlib.h>

// Parámetros a utilizar 
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS     160
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS     120

#define DWORD_ALIGN_PTR(a)   ((a & 0x3) ?(((uintptr_t)a + 0x4) & ~(uintptr_t)0x3) : a)

/* Edge Impulse */
class OV7675 : public OV767X {
    public:
        int begin(int resolution, int format, int fps);
        void readFrame(void* buffer);

    private:
        int vsyncPin;
        int hrefPin;
        int pclkPin;
        int xclkPin;

        volatile uint32_t* vsyncPort;
        uint32_t vsyncMask;
        volatile uint32_t* hrefPort;
        uint32_t hrefMask;
        volatile uint32_t* pclkPort;
        uint32_t pclkMask;

        uint16_t width;
        uint16_t height;
        uint8_t bytes_per_pixel;
        uint16_t bytes_per_row;
        uint8_t buf_rows;
        uint16_t buf_size;
        uint8_t resize_height;
        uint8_t *raw_buf;
        void *buf_mem;
        uint8_t *intrp_buf;
        uint8_t *buf_limit;

        void readBuf();
        int allocate_scratch_buffs();
        int deallocate_scratch_buffs();
};

typedef struct {
	size_t width;
	size_t height;
} ei_device_resize_resolutions_t;

/**
 * @brief      
 *
 * @return     
 */
int ei_get_serial_available(void) {
    return Serial.available();
}

/**
 * @brief      
 *
 * @return     
 */
char ei_get_serial_byte(void) {
    return Serial.read();
}

/* Imprimir variables*/
static OV7675 Cam;
static bool is_initialised = false;

static uint8_t *ei_camera_capture_out = NULL;
uint32_t resize_col_sz;
uint32_t resize_row_sz;
bool do_resize = false;
bool do_crop = false;

static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

/* Definición de funciones */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) ;
int calculate_resize_dimensions(uint32_t out_width, uint32_t out_height, uint32_t *resize_col_sz, uint32_t *resize_row_sz, bool *do_resize);
void resizeImage(int srcWidth, int srcHeight, uint8_t *srcImage, int dstWidth, int dstHeight, uint8_t *dstImage, int iBpp);
void cropImage(int srcWidth, int srcHeight, uint8_t *srcImage, int startX, int startY, int dstWidth, int dstHeight, uint8_t *dstImage, int iBpp);

/**
* @brief      Setup para el arduino
*/
void setup()
{

    Serial.begin(115200);
    Serial.println("Edge Impulse Inferencing Demo");

    // resumen de modelo de inferencia (de model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));
    // Pins para los LEDs incoroporados en la placa Arduino NANO 33 BLE Sense
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
}

/**
* @brief      obtener datos y empezar inferencia
*
* @param[in]  
*/
void loop()
{
    bool stop_inferencing = false;

    while(stop_inferencing == false) {
        ei_printf("\nStarting inferencing in 2 seconds...\n");

       
        if (ei_sleep(500) != EI_IMPULSE_OK) {
            break;
        }

        ei_printf("Taking photo...\n");

        if (ei_camera_init() == false) {
            ei_printf("ERR: Failed to initialize image sensor\r\n");
            break;
        }

        // cambiar el tamaño de la dimensiones
        uint32_t resize_col_sz;
        uint32_t resize_row_sz;
        bool do_resize = false;
        int res = calculate_resize_dimensions(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, &resize_col_sz, &resize_row_sz, &do_resize);
        if (res) {
            ei_printf("ERR: Failed to calculate resize dimensions (%d)\r\n", res);
            break;
        }

        void *snapshot_mem = NULL;
        uint8_t *snapshot_buf = NULL;
        snapshot_mem = ei_malloc(resize_col_sz*resize_row_sz*2);
        if(snapshot_mem == NULL) {
            ei_printf("failed to create snapshot_mem\r\n");
            break;
        }
        snapshot_buf = (uint8_t *)DWORD_ALIGN_PTR((uintptr_t)snapshot_mem);

        if (ei_camera_capture(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
            ei_printf("Failed to capture image\r\n");
            if (snapshot_mem) ei_free(snapshot_mem);
            break;
        }

        ei::signal_t signal;
        signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
        signal.get_data = &ei_camera_cutout_get_data;

        // correr el impulse: DSP, red neuronal y el Anomaly algorithm
        ei_impulse_result_t result = { 0 };

        EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, debug_nn);
        if (ei_error != EI_IMPULSE_OK) {
            ei_printf("Failed to run impulse (%d)\n", ei_error);
            ei_free(snapshot_mem);
            break;
        }

        // Imprimir predicciones
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
        bool bb_found = result.bounding_boxes[0].value > 0;
        for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
            auto bb = result.bounding_boxes[ix];
            if (bb.value == 0) {
                continue;
            }

            ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
        }

        if (!bb_found) {
            ei_printf("    No objects found\n");
        }
#else
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label,
                                        result.classification[ix].value);
        }
        if (result.classification[0].value >=0.6){
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDR, LOW);       
      }

      else if (result.classification[1].value >=0.6){
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, HIGH);
        digitalWrite(LEDR, LOW);         
      }

      else{
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDR, HIGH);        
      }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
#endif

        while (ei_get_serial_available() > 0) {
            if (ei_get_serial_byte() == 'b') {
                ei_printf("Inferencing stopped by user\r\n");
                stop_inferencing = true;
            }
        }
        if (snapshot_mem) ei_free(snapshot_mem);
    }
    ei_camera_deinit();
}

/**
 * @brief      Determinar si hay que cambiar las dimensiones
 *
 * @param[in]  out_width     ancho de la imagen de salida
 * @param[in]  out_height    altura de la imagen de salida
 * @param[out] resize_col_sz       puntero a frame buffer's column/width value
 * @param[out] resize_row_sz       puntero a frame buffer's rows/height value
 * @param[out] do_resize     devuelve si hay que redimensionar o no 
 *
 */
int calculate_resize_dimensions(uint32_t out_width, uint32_t out_height, uint32_t *resize_col_sz, uint32_t *resize_row_sz, bool *do_resize)
{
    size_t list_size = 2;
    const ei_device_resize_resolutions_t list[list_size] = { {42,32}, {128,96} };

    // (default) 
    *resize_col_sz = EI_CAMERA_RAW_FRAME_BUFFER_COLS;
    *resize_row_sz = EI_CAMERA_RAW_FRAME_BUFFER_ROWS;
    *do_resize = false;

    for (size_t ix = 0; ix < list_size; ix++) {
        if ((out_width <= list[ix].width) && (out_height <= list[ix].height)) {
            *resize_col_sz = list[ix].width;
            *resize_row_sz = list[ix].height;
            *do_resize = true;
            break;
        }
    }

    return 0;
}

/**
 * @brief   
 *
 * @retval  
 */
bool ei_camera_init(void) {
    if (is_initialised) return true;
    
    if (!Cam.begin(QQVGA, RGB565, 1)) { // VGA 
        ei_printf("ERR: Failed to initialize camera\r\n");
        return false;
    }
    is_initialised = true;

    return true;
}

/**
 * @brief      Parar de streamer los datos del sensor
 */
void ei_camera_deinit(void) {
    if (is_initialised) {
        Cam.end();
        is_initialised = false;
    }
}

/**
 * @brief      Capturar, rescalar y cortar imagenes 
 *
 * @param[in]  img_width     ancho de la imagen de salida
 * @param[in]  img_height    altura de la imagen de salida
 * @param[in]  out_buf       puntero para la salida
 *                           
 *
 * @retval     
 *
 */
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) 
{
    if (!is_initialised) {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }

    if (!out_buf) {
        ei_printf("ERR: invalid parameters\r\n");
        return false;
    }

    // escoger dimensiones para redimensionar
    int res = calculate_resize_dimensions(img_width, img_height, &resize_col_sz, &resize_row_sz, &do_resize);
    if (res) {
        ei_printf("ERR: Failed to calculate resize dimensions (%d)\r\n", res);
        return false;
    }

    if ((img_width != resize_col_sz)
        || (img_height != resize_row_sz)) {
        do_crop = true;
    }

    Cam.readFrame(out_buf); // captura de imágenes y cambia el tamaño

    if (do_crop) {
        uint32_t crop_col_sz;
        uint32_t crop_row_sz;
        uint32_t crop_col_start;
        uint32_t crop_row_start;
        crop_row_start = (resize_row_sz - img_height) / 2;
        crop_col_start = (resize_col_sz - img_width) / 2;
        crop_col_sz = img_width;
        crop_row_sz = img_height;

        
        cropImage(resize_col_sz, resize_row_sz,
                out_buf,
                crop_col_start, crop_row_start,
                crop_col_sz, crop_row_sz,
                out_buf,
                16);
    }

    // The following variables should always be assigned
    // if this routine is to return true
    // cutout values
    //ei_camera_snapshot_is_resized = do_resize;
    //ei_camera_snapshot_is_cropped = do_crop;
    ei_camera_capture_out = out_buf;

    return true;
}

/**
 * @brief      
 *
 * @param[in]   offset       pixel offset of raw buffer
 * @param[in]   length       number of pixels to convert
 * @param[out]  out_buf      pointer to store output image
 */
int ei_camera_cutout_get_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 2; 
    size_t bytes_left = length;
    size_t out_ptr_ix = 0;

    // leer byte por byte 
    while (bytes_left != 0) {
        // convertir el valor a RGB
        uint16_t pixel = (ei_camera_capture_out[pixel_ix] << 8) | ei_camera_capture_out[pixel_ix+1];
        uint8_t r, g, b;
        r = ((pixel >> 11) & 0x1f) << 3;
        g = ((pixel >> 5) & 0x3f) << 2;
        b = (pixel & 0x1f) << 3;

        // converitr a formato out_ptr 
        float pixel_f = (r << 16) + (g << 8) + b;
        out_ptr[out_ptr_ix] = pixel_f;

        // siguiente pixel
        out_ptr_ix++;
        pixel_ix+=2;
        bytes_left--;
    }

    
    return 0;
}


#ifdef __ARM_FEATURE_SIMD32
#include <device.h>
#endif


#define FRAC_BITS 14
#define FRAC_VAL (1<<FRAC_BITS)
#define FRAC_MASK (FRAC_VAL - 1)


// Función para cambiar tamaño de imágen 

void resizeImage(int srcWidth, int srcHeight, uint8_t *srcImage, int dstWidth, int dstHeight, uint8_t *dstImage, int iBpp)
{
    uint32_t src_x_accum, src_y_accum; 
    uint32_t x_frac, nx_frac, y_frac, ny_frac;
    int x, y, ty, tx;

    if (iBpp != 8 && iBpp != 16)
        return;
    src_y_accum = FRAC_VAL/2; 
    const uint32_t src_x_frac = (srcWidth * FRAC_VAL) / dstWidth;
    const uint32_t src_y_frac = (srcHeight * FRAC_VAL) / dstHeight;
    const uint32_t r_mask = 0xf800f800;
    const uint32_t g_mask = 0x07e007e0;
    const uint32_t b_mask = 0x001f001f;
    uint8_t *s, *d;
    uint16_t *s16, *d16;
    uint32_t x_frac2, y_frac2; 
    for (y=0; y < dstHeight; y++) {
        ty = src_y_accum >> FRAC_BITS; 
        y_frac = src_y_accum & FRAC_MASK;
        src_y_accum += src_y_frac;
        ny_frac = FRAC_VAL - y_frac; /
        y_frac2 = ny_frac | (y_frac << 16); 
        s = &srcImage[ty * srcWidth];
        s16 = (uint16_t *)&srcImage[ty * srcWidth * 2];
        d = &dstImage[y * dstWidth];
        d16 = (uint16_t *)&dstImage[y * dstWidth * 2];
        src_x_accum = FRAC_VAL/2; 
        if (iBpp == 8) {
        for (x=0; x < dstWidth; x++) {
            uint32_t tx, p00,p01,p10,p11;
            tx = src_x_accum >> FRAC_BITS;
            x_frac = src_x_accum & FRAC_MASK;
            nx_frac = FRAC_VAL - x_frac; 
            x_frac2 = nx_frac | (x_frac << 16);
            src_x_accum += src_x_frac;
            p00 = s[tx]; p10 = s[tx+1];
            p01 = s[tx+srcWidth]; p11 = s[tx+srcWidth+1];
    #ifdef __ARM_FEATURE_SIMD32
            p00 = __SMLAD(p00 | (p10<<16), x_frac2, FRAC_VAL/2) >> FRAC_BITS; // linea de arriba
            p01 = __SMLAD(p01 | (p11<<16), x_frac2, FRAC_VAL/2) >> FRAC_BITS; // linea de abajo
            p00 = __SMLAD(p00 | (p01<<16), y_frac2, FRAC_VAL/2) >> FRAC_BITS; // combinar
    #else 
            p00 = ((p00 * nx_frac) + (p10 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; // linea de arriba
            p01 = ((p01 * nx_frac) + (p11 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; // linea de abajo
            p00 = ((p00 * ny_frac) + (p01 * y_frac) + FRAC_VAL/2) >> FRAC_BITS; // combinar arriba + abajo
    #endif // Cortex-M4/M7
            *d++ = (uint8_t)p00;
        } 
        } 
        else
        { // RGB565
        for (x=0; x < dstWidth; x++) {
            uint32_t tx, p00,p01,p10,p11;
            uint32_t r00, r01, r10, r11, g00, g01, g10, g11, b00, b01, b10, b11;
            tx = src_x_accum >> FRAC_BITS;
            x_frac = src_x_accum & FRAC_MASK;
            nx_frac = FRAC_VAL - x_frac; 
            x_frac2 = nx_frac | (x_frac << 16);
            src_x_accum += src_x_frac;
            p00 = __builtin_bswap16(s16[tx]); p10 = __builtin_bswap16(s16[tx+1]);
            p01 = __builtin_bswap16(s16[tx+srcWidth]); p11 = __builtin_bswap16(s16[tx+srcWidth+1]);
    #ifdef __ARM_FEATURE_SIMD32
            {
            p00 |= (p10 << 16);
            p01 |= (p11 << 16);
            r00 = (p00 & r_mask) >> 1; g00 = p00 & g_mask; b00 = p00 & b_mask;
            r01 = (p01 & r_mask) >> 1; g01 = p01 & g_mask; b01 = p01 & b_mask;
            r00 = __SMLAD(r00, x_frac2, FRAC_VAL/2) >> FRAC_BITS;
            r01 = __SMLAD(r01, x_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            r00 = __SMLAD(r00 | (r01<<16), y_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            g01 = __SMLAD(g01, x_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            g00 = __SMLAD(g00 | (g01<<16), y_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            b00 = __SMLAD(b00, x_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            b01 = __SMLAD(b01, x_frac2, FRAC_VAL/2) >> FRAC_BITS; 
            b00 = __SMLAD(b00 | (b01<<16), y_frac2, FRAC_VAL/2) >> FRAC_BITS; /
            }
    #else 
            {
            r00 = (p00 & r_mask) >> 1; g00 = p00 & g_mask; b00 = p00 & b_mask;
            r10 = (p10 & r_mask) >> 1; g10 = p10 & g_mask; b10 = p10 & b_mask;
            r01 = (p01 & r_mask) >> 1; g01 = p01 & g_mask; b01 = p01 & b_mask;
            r11 = (p11 & r_mask) >> 1; g11 = p11 & g_mask; b11 = p11 & b_mask;
            r00 = ((r00 * nx_frac) + (r10 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            r01 = ((r01 * nx_frac) + (r11 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            r00 = ((r00 * ny_frac) + (r01 * y_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            g00 = ((g00 * nx_frac) + (g10 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            g01 = ((g01 * nx_frac) + (g11 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            g00 = ((g00 * ny_frac) + (g01 * y_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            b00 = ((b00 * nx_frac) + (b10 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            b01 = ((b01 * nx_frac) + (b11 * x_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            b00 = ((b00 * ny_frac) + (b01 * y_frac) + FRAC_VAL/2) >> FRAC_BITS; 
            }
    #endif // Cortex-M4/M7
            r00 = (r00 << 1) & r_mask;
            g00 = g00 & g_mask;
            b00 = b00 & b_mask;
            p00 = (r00 | g00 | b00); // re combinar componentes de colores
            *d16++ = (uint16_t)__builtin_bswap16(p00); // guardar nuevo pixel 
        } // for x
        } // 16-bpp
    } // for y
} 
//
// Cortar imgágen 

void cropImage(int srcWidth, int srcHeight, uint8_t *srcImage, int startX, int startY, int dstWidth, int dstHeight, uint8_t *dstImage, int iBpp)
{
    uint32_t *s32, *d32;
    int x, y;

    if (startX < 0 || startX >= srcWidth || startY < 0 || startY >= srcHeight || (startX + dstWidth) > srcWidth || (startY + dstHeight) > srcHeight)
       return; // parámetros inválidos
    if (iBpp != 8 && iBpp != 16)
       return;

    if (iBpp == 8) {
      uint8_t *s, *d;
      for (y=0; y<dstHeight; y++) {
        s = &srcImage[srcWidth * (y + startY) + startX];
        d = &dstImage[(dstWidth * y)];
        x = 0;
        if ((intptr_t)s & 3 || (intptr_t)d & 3) { 
          for (; x<dstWidth; x++) {
            *d++ = *s++; // se realiza byte por byte
          }
        } else {
          
          s32 = (uint32_t *)s;
          d32 = (uint32_t *)d;
          for (; x<dstWidth-3; x+= 4) {
            *d32++ = *s32++;
          }
          
          s = (uint8_t *)s32;
          d = (uint8_t *)d32;
          for (; x<dstWidth; x++) {
            *d++ = *s++;
          }
        }
      } // for y
    } // 8-bpp
    else
    {
      uint16_t *s, *d;
      for (y=0; y<dstHeight; y++) {
        s = (uint16_t *)&srcImage[2 * srcWidth * (y + startY) + startX * 2];
        d = (uint16_t *)&dstImage[(dstWidth * y * 2)];
        x = 0;
        if ((intptr_t)s & 2 || (intptr_t)d & 2) { 
          for (; x<dstWidth; x++) {
            *d++ = *s++; // 16 bits cada vez
          }
        } else {
          
          s32 = (uint32_t *)s;
          d32 = (uint32_t *)d;
          for (; x<dstWidth-1; x+= 2) { // mover 2 pixeles cada vez
            *d32++ = *s32++;
          }
      
          s = (uint16_t *)s32;
          d = (uint16_t *)d32;
          for (; x<dstWidth; x++) {
            *d++ = *s++;
          }
        }
      } // for y
    } // 16-bpp case
} /* cropImage() */

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif

// OV767X camera se reescribe la librería
#include <Arduino.h>
#include <Wire.h>

#define digitalPinToBitMask(P) (1 << (digitalPinToPinName(P) % 32))
#define portInputRegister(P) ((P == 0) ? &NRF_P0->IN : &NRF_P1->IN)

//
// OV7675::begin()
//
// Extends the OV767X library function. Some private variables are needed
// to use the OV7675::readFrame function.
//
int OV7675::begin(int resolution, int format, int fps)
{
    pinMode(OV7670_VSYNC, INPUT);
    pinMode(OV7670_HREF, INPUT);
    pinMode(OV7670_PLK, INPUT);
    pinMode(OV7670_XCLK, OUTPUT);

    vsyncPort = portInputRegister(digitalPinToPort(OV7670_VSYNC));
    vsyncMask = digitalPinToBitMask(OV7670_VSYNC);
    hrefPort = portInputRegister(digitalPinToPort(OV7670_HREF));
    hrefMask = digitalPinToBitMask(OV7670_HREF);
    pclkPort = portInputRegister(digitalPinToPort(OV7670_PLK));
    pclkMask = digitalPinToBitMask(OV7670_PLK);

    // driver para usar el tamaño de la imagen completa
    bool ret = OV767X::begin(VGA, format, fps);
    width = OV767X::width(); // ancho completo del sensor
    height = OV767X::height(); // altura completa del sensor
    bytes_per_pixel = OV767X::bytesPerPixel();
    bytes_per_row = width * bytes_per_pixel; // cada pixel es de 2 bytes
    resize_height = 2;

    buf_mem = NULL;
    raw_buf = NULL;
    intrp_buf = NULL;
    //allocate_scratch_buffs();

    return ret;
} /* OV7675::begin() */

int OV7675::allocate_scratch_buffs()
{
    //ei_printf("allocating buffers..\r\n");
    buf_rows = height / resize_row_sz * resize_height;
    buf_size = bytes_per_row * buf_rows;

    buf_mem = ei_malloc(buf_size);
    if(buf_mem == NULL) {
        ei_printf("failed to create buf_mem\r\n");
        return false;
    }
    raw_buf = (uint8_t *)DWORD_ALIGN_PTR((uintptr_t)buf_mem);

    //ei_printf("allocating buffers OK\r\n");
    return 0;
}

int OV7675::deallocate_scratch_buffs()
{
    //ei_printf("deallocating buffers...\r\n");
    ei_free(buf_mem);
    buf_mem = NULL;
    
    //ei_printf("deallocating buffers OK\r\n");
    return 0;
}

//
// OV7675::readFrame()
// Lecutra de cada marco por parte de la cámara
void OV7675::readFrame(void* buffer)
{
    allocate_scratch_buffs();

    uint8_t* out = (uint8_t*)buffer;
    noInterrupts();

    // flanco decreciente indica comienzo del marco
    while ((*vsyncPort & vsyncMask) == 0); // esperar a HIGH
    while ((*vsyncPort & vsyncMask) != 0); // esperar a LOW

    int out_row = 0;
    for (int raw_height = 0; raw_height < height; raw_height += buf_rows) {
        // read in 640xbuf_rows buffer to work with
        readBuf();

        resizeImage(width, buf_rows,
                    raw_buf,
                    resize_col_sz, resize_height,
                    &(out[out_row]),
                    16);
        
        out_row += resize_col_sz * resize_height * bytes_per_pixel; /* resize_col_sz * 2 * 2 */
    }

    interrupts();

    deallocate_scratch_buffs();
} /* OV7675::readFrame() */

//
// OV7675::readBuf()
//

void OV7675::readBuf()
{
    int offset = 0;

    uint32_t ulPin = 33; 
    NRF_GPIO_Type * port;

    port = nrf_gpio_pin_port_decode(&ulPin);

    for (int i = 0; i < buf_rows; i++) {
        // flanco creciente indica comienzo de línea
        while ((*hrefPort & hrefMask) == 0); // esperar a HIGH

        for (int col = 0; col < bytes_per_row; col++) {
            // flanco creciente para cada byte de datos
            while ((*pclkPort & pclkMask) != 0); // esperar a LOW

            uint32_t in = port->IN; // leer bits en paralelo

            in >>= 2; // poner 0 y 1 al final de registro
            in &= 0x3f03; // obtener los 8 bits que interesan
            in |= (in >> 6); // combinar los 6 bits de arriba con los 2 de abajo

            raw_buf[offset++] = in;

            while ((*pclkPort & pclkMask) == 0); // esperar a HIGH
        }

        while ((*hrefPort & hrefMask) != 0); // esperar a LOW
    }
} /* OV7675::readBuf() */
