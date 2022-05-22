/**
 * @file bme280.hpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-05-20
 * 
 * @brief Library for BME280 sensor
 */

#ifndef _BME280_HPP_
#define _BME280_HPP_

#include <cstdint>

namespace BME280 {

    class bme280
    {
    private:
        struct bme280_compensation_data
        {
            uint16_t t1;
            int16_t t2;
            int16_t t3;

            uint16_t p1;
            int16_t p2;
            int16_t p3;
            int16_t p4;
            int16_t p5;
            int16_t p6;
            int16_t p7;
            int16_t p8;
            int16_t p9;

            uint8_t h1;
            int16_t h2;
            uint8_t h3;
            int16_t h4;
            int16_t h5;
            int8_t h6;

            int32_t t_fine;
        };
        
    public:
        enum bme_status {
            STATUS_OK           = 0,
            STATUS_ERROR        = 1,

            STATUS_TOP
        };

        enum meas_oversampling {
            OVERSAMPLING_NONE   = 0,    // disable specific measurement
            OVERSAMPLING_X1     = 1,
            OVERSAMPLING_X2     = 2,
            OVERSAMPLING_X4     = 3,
            OVERSAMPLING_X8     = 4,
            OVERSAMPLING_X16    = 5,

            OVERSAMPLING_TOP
        };

        enum working_mode {
            MODE_SLEEP          = 0,
            MODE_FORCED         = 1,
            MODE_NORMAL         = 3,

            MODE_TOP
        };

        enum standby_time {
            STANDBY_0_5	        = 0,
            STANDBY_62_5	    = 1,
            STANDBY_125	        = 2,
            STANDBY_250	        = 3,
            STANDBY_500	        = 4,
            STANDBY_1000	    = 5,
            STANDBY_10          = 6,
            STANDBY_20          = 7,

            STANDBY_TOP
        };

        enum filter_coeffitiens {
            FILTER_OFF          = 0,
            FILTER_COEF_2       = 1,
            FILTER_COEF_4       = 2,
            FILTER_COEF_8       = 3,
            FILTER_COEF_16      = 4,

            FILTER_TOP
        };

        /**
         * @brief Construct a new bme280 object
         * 
         * @param [in] read_fp pointer to read function
         * @param [in] write_fp pointer to write function 
         * @param [in] delay_fp pointer to delay function
         */
        bme280(bme_status (*read_fp)(uint8_t, uint8_t *, uint32_t), bme_status (*write_fp)(uint8_t, uint8_t *, uint32_t), void (*delay_fp)(uint32_t));
        
        /**
         * @brief Stop sensor measurements and destroy bme280 object
         */
        ~bme280();

        /**
         * @brief Initialize BME280 sensor
         * 
         * @param [in] temp_oversampling 
         * @param [in] hum_oversampling 
         * @param [in] press_oversampling 
         * @param [in] mode working mode of a sensor 
         * @param [in] s_time 
         * @param [in] filter 
         * @return STATUS_OK if initialization was successful otherwise STATUS_ERROR
         */
        bme_status bme280_Init(meas_oversampling temp_oversampling, meas_oversampling hum_oversampling, 
            meas_oversampling press_oversampling, working_mode mode, standby_time s_time, filter_coeffitiens filter);


        private:
            working_mode sensor_work_mode = MODE_SLEEP;
            standby_time sensor_standby_time = STANDBY_0_5;
            filter_coeffitiens sensor_selected_filter = FILTER_OFF;
            meas_oversampling temperature_oversampling = OVERSAMPLING_NONE;
            meas_oversampling humidity_oversampling = OVERSAMPLING_NONE;
            meas_oversampling pressure_oversampling = OVERSAMPLING_NONE;
            struct bme280_compensation_data sensor_compensation_data = {0};

            bme_status (* _read) (uint8_t reg_addr, uint8_t *data, uint32_t data_len) = nullptr;
            bme_status (* _write) (uint8_t reg_addr, uint8_t *data, uint32_t data_len) = nullptr;
            void (* _delay) (uint32_t ms) = nullptr;
            
            void _readCompensationData(void);
    };
}

#endif