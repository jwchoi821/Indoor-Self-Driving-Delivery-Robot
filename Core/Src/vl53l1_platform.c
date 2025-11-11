#include "vl53l1_platform.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c1;

#define I2C_TIMEOUT 100

int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
    uint8_t buffer[2 + count];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    memcpy(&buffer[2], pdata, count);
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 2 + count, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
    uint8_t buffer[2];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 2, I2C_TIMEOUT) != HAL_OK)
        return -1;
    if (HAL_I2C_Master_Receive(&hi2c1, dev, pdata, count, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
    uint8_t buffer[3];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    buffer[2] = data;
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 3, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
    uint8_t buffer[4];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 4, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
    uint8_t buffer[6];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    buffer[2] = (data >> 24) & 0xFF;
    buffer[3] = (data >> 16) & 0xFF;
    buffer[4] = (data >> 8) & 0xFF;
    buffer[5] = data & 0xFF;
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 6, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
    uint8_t buffer[2];
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, buffer, 2, I2C_TIMEOUT) != HAL_OK)
        return -1;
    if (HAL_I2C_Master_Receive(&hi2c1, dev, data, 1, I2C_TIMEOUT) != HAL_OK)
        return -1;
    return 0;
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
    uint8_t buffer[2];
    uint8_t reg[2] = {(index >> 8) & 0xFF, index & 0xFF};
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, reg, 2, I2C_TIMEOUT) != HAL_OK)
        return -1;
    if (HAL_I2C_Master_Receive(&hi2c1, dev, buffer, 2, I2C_TIMEOUT) != HAL_OK)
        return -1;
    *data = (buffer[0] << 8) | buffer[1];
    return 0;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
    uint8_t buffer[4];
    uint8_t reg[2] = {(index >> 8) & 0xFF, index & 0xFF};
    if (HAL_I2C_Master_Transmit(&hi2c1, dev, reg, 2, I2C_TIMEOUT) != HAL_OK)
        return -1;
    if (HAL_I2C_Master_Receive(&hi2c1, dev, buffer, 4, I2C_TIMEOUT) != HAL_OK)
        return -1;
    *data = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    return 0;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
    HAL_Delay(wait_ms);
    return 0;
}
