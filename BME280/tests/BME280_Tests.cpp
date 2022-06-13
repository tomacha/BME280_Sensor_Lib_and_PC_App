#include "bme280.hpp"

#include <gtest/gtest.h>

namespace BME280 {
    void sensDelay(uint32_t ms)
    {
        return;
    }
};

TEST(Tests, bme280_Init_args){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_OK, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_TOP,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_TOP,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_TOP,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_TOP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_TOP));
}

TEST(Tests, bme280_Init_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
}

TEST(Tests, bme280_Init_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
}

TEST(Tests, bme280_ReadTemperature_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadTemperature());
}

TEST(Tests, bme280_ReadTemperature_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadTemperature());
}