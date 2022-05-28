/**
 * @file bme280.cpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-05-20
 */

#include "bme280.hpp"
#include <cstdio>

enum _sensor_addresses
{
    BME280_MEASUREMENT_PRESS_MSB_ADDR   = 0xF7,
    BME280_MEASUREMENT_PRESS_LSB_ADDR   = 0xF8,
    BME280_MEASUREMENT_PRESS_XLSB_ADDR  = 0xF9,
    BME280_MEASUREMENT_TEMP_MSB_ADDR    = 0xFA,
    BME280_MEASUREMENT_TEMP_LSB_ADDR    = 0xFB,
    BME280_MEASUREMENT_TEMP_XLSB_ADDR   = 0xFC,
    BME280_MEASUREMENT_HUM_MSB_ADDR	    = 0xFD,
    BME280_MEASUREMENT_HUM_LSB_ADDR	    = 0xFE,

    BME280_CHIP_ID_ADDR		            = 0xD0,
    BME280_RESET_ADDR		            = 0xE0,
    BME280_CALIB_DATA_00_25_ADDR	    = 0x88,
    BME280_CALIB_DATA_26_41_ADDR	    = 0xE1,
    BME280_STATUS_ADDR		            = 0xF3,
    BME280_CTRL_HUM_ADDR	            = 0xF2,
    BME280_CTRL_MEAS_ADDR	            = 0xF4,
    BME280_CONFIG_ADDR		            = 0xF5,

    BME280_CALIBRATION_DIG_T1_ADDR      = 0x88,
    BME280_CALIBRATION_DIG_T2_ADDR      = 0x8A,
    BME280_CALIBRATION_DIG_T3_ADDR      = 0x8C,
    BME280_CALIBRATION_DIG_P1_ADDR      = 0x8E,
    BME280_CALIBRATION_DIG_P2_ADDR      = 0x90,
    BME280_CALIBRATION_DIG_P3_ADDR      = 0x92,
    BME280_CALIBRATION_DIG_P4_ADDR      = 0x94,
    BME280_CALIBRATION_DIG_P5_ADDR      = 0x96,
    BME280_CALIBRATION_DIG_P6_ADDR      = 0x98,
    BME280_CALIBRATION_DIG_P7_ADDR      = 0x9A,
    BME280_CALIBRATION_DIG_P8_ADDR      = 0x9C,
    BME280_CALIBRATION_DIG_P9_ADDR      = 0x9E,
    BME280_CALIBRATION_DIG_H1_ADDR      = 0xA1,
    BME280_CALIBRATION_DIG_H2_ADDR      = 0xE1,
    BME280_CALIBRATION_DIG_H3_ADDR      = 0xE3,
    BME280_CALIBRATION_DIG_H4_ADDR      = 0xE4,
    BME280_CALIBRATION_DIG_H5_ADDR      = 0xE5,
    BME280_CALIBRATION_DIG_H6_ADDR      = 0xE7,
};


namespace BME280 {

    bme_status bme280::_readCompensationData(void)
    {
        uint8_t temp[32] = {0};
        bme_status status;

        status = this->_read(BME280_CALIBRATION_DIG_T1_ADDR, (uint8_t*) &temp[0], 24);
        if (status != STATUS_OK) 
        {
            return status;
        }
        status = this->_read(BME280_CALIBRATION_DIG_H1_ADDR, (uint8_t*) &temp[24], 1);
        if (status != STATUS_OK) 
        {
            return status;
        }
        status = this->_read(BME280_CALIBRATION_DIG_H2_ADDR, (uint8_t*) &temp[25], 7);
        if (status != STATUS_OK) 
        {
            return status;
        }

        this->sensor_compensation_data.t1 = (((uint16_t) (temp[1]<< 8)) | ((uint16_t) temp[0]));
        this->sensor_compensation_data.t2 = (((uint16_t) (temp[3]<< 8)) | ((uint16_t) temp[2]));
        this->sensor_compensation_data.t3 = (((uint16_t) (temp[5]<< 8)) | ((uint16_t) temp[4]));

        this->sensor_compensation_data.p1 = (((uint16_t) (temp[7] << 8)) | ((uint16_t) temp[6]));
        this->sensor_compensation_data.p2 = (((uint16_t) (temp[9] << 8)) | ((uint16_t) temp[8]));
        this->sensor_compensation_data.p3 = (((uint16_t) (temp[11] << 8)) | ( (uint16_t) temp[10]));
        this->sensor_compensation_data.p4 = (((uint16_t) (temp[13] << 8)) | ( (uint16_t) temp[12]));
        this->sensor_compensation_data.p5 = (((uint16_t) (temp[15] << 8)) | ( (uint16_t) temp[14]));
        this->sensor_compensation_data.p6 = (((uint16_t) (temp[17] << 8)) | ( (uint16_t) temp[16]));
        this->sensor_compensation_data.p7 = (((uint16_t) (temp[19] << 8)) | ( (uint16_t) temp[18]));
        this->sensor_compensation_data.p8 = (((uint16_t) (temp[21] << 8)) | ( (uint16_t) temp[20]));
        this->sensor_compensation_data.p9 = (((uint16_t) (temp[23] << 8)) | ( (uint16_t) temp[22]));

        this->sensor_compensation_data.h1 = temp[24];
        this->sensor_compensation_data.h2 = (((uint16_t) (temp[26] << 8)) | ((uint16_t) temp[25]));
        this->sensor_compensation_data.h3 = temp[27];
        this->sensor_compensation_data.h4 = (((uint16_t) (temp[28] << 4)) | ((uint16_t) (temp[29] & 0x0F)));
        this->sensor_compensation_data.h5 = (((uint16_t) (temp[29] >> 4)) | ((uint16_t) (temp[30] << 4)));
        this->sensor_compensation_data.h6 = temp[31];
        
        return STATUS_OK;
    }

    bme_status bme280::_forceMeasurement(void)
    {
        bme_status status;
        uint8_t reg;

        status = this->_read(BME280_CTRL_MEAS_ADDR, &reg, 1);
        if(status != STATUS_OK)
        {
            return status;
        }
        reg &= ~(0x03);
        reg |= MODE_FORCED;
        status = this->_write(BME280_CTRL_MEAS_ADDR, &reg, 1);
        if(status != STATUS_OK)
        {
            return status;
        }

        // wait for end of measurement
        while (1)
        {
            status = this->_read(BME280_CTRL_MEAS_ADDR, &reg, 1);
            if(status != STATUS_OK)
            {
                return status;
            }
            reg &= 0x03;
            if(reg == MODE_SLEEP)
            {
                break;;
            }
        }

        return STATUS_OK;
    }

    bme_status bme280::_readRawData(void)
    {
        uint8_t temp[8];
        bme_status status;
        
        // check mode of the sensor
        if(this->sensor_work_mode == MODE_FORCED)
        {
            status = _forceMeasurement();
            if(status != STATUS_OK)
            {
                return status;
            }
        }

        status = this->_read(BME280_MEASUREMENT_PRESS_MSB_ADDR, (uint8_t*) &temp, 8);
        if (status != STATUS_OK)
        {
            return status;
        }

        this->raw_press = (((uint32_t) temp[0] << 16) | ((uint32_t) temp[1] << 8) | temp[2]);
        this->raw_temp = (((uint32_t) temp[3] << 16) | ((uint32_t) temp[4] << 8) | temp[5]);
        this->raw_hum = (((uint32_t) temp[6] << 8) | temp[7]);

        return STATUS_OK;
    }

    void bme280::_compensate_temperature(void)
    {
        uint32_t var1, var2;

        this->raw_temp = this->raw_temp >> 4;

        var1 = ((((this->raw_temp >> 3) - ((int32_t) this->sensor_compensation_data.t1 <<1))) *
                ((int32_t)this->sensor_compensation_data.t2)) >> 11;
        var2 = (((((this->raw_temp >> 4)- ((int32_t) this->sensor_compensation_data.t1)) *
                ((this->raw_temp >> 4) - ((int32_t) this->sensor_compensation_data.t1))) >> 12) *
                ((int32_t)this->sensor_compensation_data.t3)) >> 14;
        // t_fine carries global temperature for compensation for humidity and pressure
        this->t_fine = var1 + var2;
        // return value in DegC
        this->temperature = ((this->t_fine * 5 + 128) >> 8) / 100;
    }

    void bme280::_compensate_pressure(void)
    {
        int64_t val1, val2, p;

        this->raw_press = this->raw_press >> 4;

        val1 = ((int64_t) this->t_fine) - 128000;
        val2 = val1 * val1 * (int64_t) this->sensor_compensation_data.p6;
        val2 = val2 + ((val1 * (int64_t) this->sensor_compensation_data.p5) << 17);
        val2 = val2 + (((int64_t) this->sensor_compensation_data.p4) << 35);
        val1 = ((val1 * val1 * (int64_t) this->sensor_compensation_data.p3) >> 8) + 
            ((val1 *(int64_t )this->sensor_compensation_data.p2) << 12);
        val1 = (((((int64_t)1) << 47) + val1)) * ((int64_t) this->sensor_compensation_data.p1)  >> 33;

        if (val1 == 0)
        {
            // avoid division by 0
            this->pressure = 0;
            return;
        }

        p = 1048576 - this->raw_press;
        p = (((p << 31) - val2) * 3125) / val1;
        val1 = (((int64_t) this->sensor_compensation_data.p9) * (p >> 13) * (p >> 13)) >> 25;
        val2 = (((int64_t) this->sensor_compensation_data.p8) * p) >> 19;

        p = ((p + val1 + val2) >> 8) + (((int64_t) this->sensor_compensation_data.p7) << 4);
        
        // returning value in hPa
	    this->pressure = (int32_t) p / 256000.0;
    }   

    void bme280::_compensate_humidity(void)
    {
        int32_t val1;

        val1 = (this->t_fine - ((int32_t)76800));
        val1 = (((((raw_hum << 14) - (((int32_t)this->sensor_compensation_data.h4) << 20) -
                (((int32_t)this->sensor_compensation_data.h5)* val1)) + ((int32_t)16384)) >> 15) *
                (((((((val1 * ((int32_t)this->sensor_compensation_data.h6)) >> 10) *
                (((val1 * ((int32_t)this->sensor_compensation_data.h3)) >> 11) +
                ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                ((int32_t)this->sensor_compensation_data.h2) + 8192) >> 14));
        val1 = (val1 - (((((val1 >> 15) * (val1 >> 15))  >> 7) * ((int32_t)this->sensor_compensation_data.h1)) >> 4));
        val1 = (val1 < 0) ? 0 : val1;
        val1 = (val1 > 419430400) ? 419430400 : val1;
        // get value in 32 bit unsigned integer in Q22.10 format (22 integer and 10 fractional bits)
        float h = (val1 >> 12);
        // convert value to final % RH float value
        this->humidity = h / 1024.0;
    }

    bme280::bme280(bme_status (*read_fp)(uint8_t, uint8_t *, uint32_t), bme_status (*write_fp)(uint8_t, uint8_t *, uint32_t), void (*delay_fp)(uint32_t))
    {
        this->_read = read_fp;
        this->_write = write_fp;
        this->_delay = delay_fp;
    }
    
    bme280::~bme280()
    {
        // perform soft reset
        uint8_t data = 0xB6;
        this->_write(BME280_RESET_ADDR, &data, 1);
    }

    bme_status bme280::bme280_Init(meas_oversampling temp_oversampling, meas_oversampling hum_oversampling, 
        meas_oversampling press_oversampling,working_mode mode, standby_time s_time, filter_coeffitiens filter)
    {
        if ((temp_oversampling >= OVERSAMPLING_TOP) || (hum_oversampling >= OVERSAMPLING_TOP) || (press_oversampling >= OVERSAMPLING_TOP) || 
            (mode >= MODE_TOP) || ( s_time >= STANDBY_TOP) || (filter >= FILTER_TOP))
        {
            return STATUS_ERROR;
        }

        bme_status status = STATUS_ERROR;

        this->temperature_oversampling = temp_oversampling;
        this->humidity_oversampling = hum_oversampling;
        this->pressure_oversampling = press_oversampling;
        this->sensor_work_mode = mode;
        this->sensor_standby_time = s_time;
        this->sensor_selected_filter = filter;

        // perform soft reset
        uint8_t data = 0xB6;
        status = this->_write(BME280_RESET_ADDR, &data, 1);

        // wait for end of callibration
        status = this->_read(BME280_STATUS_ADDR, &data, 1);
        if (status != STATUS_OK) { return status; }

        while ((data & 0x01) != 0)
        {
            this->_delay(10);
            this->_read(BME280_STATUS_ADDR, &data, 1);
            if (status != STATUS_OK) { return status; }
        }
        
        // reading parameters for data compensation
        _readCompensationData();

        if (mode == MODE_NORMAL)
        {
            // setting standby time, filter and disable 3-wire SPI
            data = ((((uint8_t)(s_time) << 5) | ((uint8_t)(filter) << 2)) & 0xFC);
        }

        status = this->_read(BME280_CTRL_HUM_ADDR, &data, 1);
        if (status != STATUS_OK) { return status; }

        data |= (uint8_t) hum_oversampling;
        status = this->_write(BME280_CTRL_HUM_ADDR, &data, 1);
        if (status != STATUS_OK) { return status; }

        data = (((uint8_t)(temp_oversampling) << 5) | ((uint8_t)(press_oversampling) << 2) | (uint8_t) mode);
        status = this->_write(BME280_CTRL_MEAS_ADDR, &data, 1);
        
        return status;
    }

    bme_status bme280::bme280_ReadTemperature()
    {
        bme_status status;

        status = _readRawData();
        if (status != STATUS_OK)
        {
            return status;
        }

        // if measurement was skipped return -100
        if(this->raw_temp == 0x800000)
        {
            this->temperature = -100.0f;
            return STATUS_OK;
        }
        
        _compensate_temperature();
        return STATUS_OK;
    }

    bme_status bme280::bme280_ReadPressure()
    {
        bme_status status;

        status = _readRawData();
        if (status != STATUS_OK)
        {
            return status;
        }

        // if measurement was skipped return -100
        if(this->raw_press == 0x800000)
        {
            this->pressure = -100.0f;
            return STATUS_OK;
        }

        // get current t_fine for compenastion
        _compensate_temperature();
        _compensate_pressure();

        return STATUS_OK;
    }

    bme_status bme280::bme280_ReadHumidity()
    {
        bme_status status;

        status = _readRawData();
        if (status != STATUS_OK)
        {
            return status;
        }

        // if measurement was skipped return -100
        if(this->raw_hum == 0x8000)
        {
            this->humidity = -100.0f;
            return STATUS_OK;
        }

        // get current t_fine for compenastion
        _compensate_temperature();
        _compensate_humidity();

        return STATUS_OK;
    }

    bme_status bme280::bme280_ReadMeasurements()
    {
        bme_status status;

        status = _readRawData();
        if (status != STATUS_OK)
        {
            return status;
        }

        // if tempearture measurement was skipped return -100
        if(this->raw_temp == 0x800000)
        {
            this->temperature = -100.0f;
        }
        else
        {
            _compensate_temperature();
        }

        // if tempearture measurement was skipped return -100
        if(this->raw_press == 0x800000)
        {
            this->pressure = -100.0f;
        }
        else
        {
            _compensate_pressure();
        }

        // if humidity measurement was skipped return -100
        if(this->raw_hum == 0x8000)
        {
            this->humidity = -100.0f;
        }
        else
        {
            _compensate_humidity();
        }

        return STATUS_OK;
    }
}