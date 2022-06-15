#include "bme280.hpp"

#include <gtest/gtest.h>

namespace BME280 {
    bme_status sensRead_measurement_skipped(uint8_t reg_addr, uint8_t *data, uint32_t len){
        if(reg_addr == 244)
        {
            *data = 0xfc;
        }
        else
        {
            data[0] = 0x80;
            data[1] = 0x00;
            data[2] = 0x00;
            data[3] = 0x80;
            data[4] = 0x00;
            data[5] = 0x00;
            data[6] = 0x80;
            data[7] = 0x00;            
        }
        
        return STATUS_OK;
    }
    
    void sensDelay(uint32_t ms)
    {
        return;
    }
};

TEST(BME280_Tests, bme280_Init_args){
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

TEST(BME280_Tests, bme280_Init_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
}

TEST(BME280_Tests, bme280_Init_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF));
}

TEST(BME280_Tests, bme280_ReadTemperature_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadTemperature());
}

TEST(BME280_Tests, bme280_ReadTemperature_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadTemperature());
}

TEST(BME280_Tests, bme280_ReadTemperature_skipped_measurement){
    BME280::bme280 sensor(BME280::sensRead_measurement_skipped, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_OK, sensor.bme280_ReadTemperature());
    EXPECT_EQ(sensor.temperature, -100.0f);
}

TEST(BME280_Tests, bme280_ReadHumidity_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadHumidity());
}

TEST(BME280_Tests, bme280_ReadHumidity_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadHumidity());
}

TEST(BME280_Tests, bme280_ReadHumidity_skipped_measurement){
    BME280::bme280 sensor(BME280::sensRead_measurement_skipped, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_OK, sensor.bme280_ReadHumidity());
    EXPECT_EQ(sensor.humidity, -100.0f);
}

TEST(BME280_Tests, bme280_ReadPressure_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadPressure());
}

TEST(BME280_Tests, bme280_ReadPressure_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadPressure());
}

TEST(BME280_Tests, bme280_ReadPressure_skipped_measurement){
    BME280::bme280 sensor(BME280::sensRead_measurement_skipped, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_OK, sensor.bme280_ReadPressure());
    EXPECT_EQ(sensor.pressure, -100.0f);
}

TEST(BME280_Tests, bme280_ReadMeasurements_write_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadMeasurements());
}

TEST(BME280_Tests, bme280_ReadMeasurements_read_fail){
    BME280::bme280 sensor([](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_ERROR; }, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_ERROR, sensor.bme280_ReadMeasurements());
}

TEST(BME280_Tests, bme280_ReadMeasurements_skipped_measurement){
    BME280::bme280 sensor(BME280::sensRead_measurement_skipped, 
        [](uint8_t reg_addr, uint8_t *data, uint32_t len) { return BME280::STATUS_OK; }, BME280::sensDelay);

    EXPECT_EQ(BME280::STATUS_OK, sensor.bme280_ReadMeasurements());
    EXPECT_EQ(sensor.temperature, -100.0f);
    EXPECT_EQ(sensor.humidity, -100.0f);
    EXPECT_EQ(sensor.pressure, -100.0f);
}