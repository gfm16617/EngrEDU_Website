/*
 * ENGREDU_MT6701.c
 *
 *  Created on: Apr 18, 2026
 *      Author: gfm
 */
//Header files
#include "ENGREDU_MT6701.h"


//Function Definitions
//1- [TESTED]
void MT6701_Init(MT6701_Handle *h, I2C_HandleTypeDef *hi2c){
    h->hi2c = hi2c;
    h->initialized = 0;
}

uint16_t MT6701_ReadRaw(MT6701_Handle *h){
    uint8_t buf[2];

    HAL_I2C_Mem_Read(h->hi2c,
                     MT6701_ADDR << 1,
                     MT6701_REG_03_ANGLE,
                     I2C_MEMADD_SIZE_8BIT,
                     buf,
                     2,
                     100);

    return ((uint16_t)buf[0] << 6) | ((buf[1] >> 2) & 0x3F);
}

/*
//3- i2c Write [REVISE]
HAL_StatusTypeDef MT6701_I2C_Write8(uint8_t reg, uint8_t data)
{
	uint8_t addr = (MT6701_ADDR<<1);
	return HAL_I2C_Mem_Write(&i2cHandler, addr, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}
*/

void MT6701_Update(MT6701_Handle *h, float dt){
    // 1. Read raw
    h->raw = MT6701_ReadRaw(h);

    // 2. Convert to degrees
    h->angle_deg = h->raw * (360.0f / 16384.0f); // absolute 0-360 - debugging

    // 3. Initialization
    if (!h->initialized) {
        h->prev_angle = h->angle_deg;
        h->angle_unwrapped = h->angle_deg;
        h->angle_filtered = h->angle_deg;
        h->velocity_deg_s = 0;
        h->initialized = 1;
        return;
    }

    // 4. Wrap-aware delta
    float delta = h->angle_deg - h->prev_angle;

    if (delta > 180.0f)  delta -= 360.0f;
    if (delta < -180.0f) delta += 360.0f;

    // 5. Unwrap
    h->angle_unwrapped += delta; // continuous - good for tracking motion

    // 6. Velocity (deg/s)
    h->velocity_deg_s = delta / dt;

    // 7. Low-pass filter
    float alpha = 0.2f;
    h->angle_filtered = alpha * h->angle_deg +
                        (1.0f - alpha) * h->angle_filtered;

    h->prev_angle = h->angle_deg;

    // 8. Relative Angle
    float rel = h->angle_deg - h->zero_angle;

    if (rel > 180.0f)  rel -= 360.0f;
    if (rel < -180.0f) rel += 360.0f;

    h->relative_angle = rel; // centered (-180° ↔ +180°) - better for simple control

    // 10. Relative (wrapped 0–360)
    h->relative_angle = h->angle_deg - h->zero_angle;

    if (h->relative_angle < 0) h->relative_angle += 360.0f;
    if (h->relative_angle >= 360.0f) h->relative_angle -= 360.0f;

    // Relative continuous (best for advance control)
    h->relative_unwrapped = h->angle_unwrapped - h->zero_angle;  // continuous relative - advance control
}

void MT6701_SetZero(MT6701_Handle *h)
{
    h->zero_angle = h->angle_deg;  // reference - calibration
}

