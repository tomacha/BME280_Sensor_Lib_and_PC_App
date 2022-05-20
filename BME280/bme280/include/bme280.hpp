#ifndef _BME280_HPP_
#define _BME280_HPP_

namespace BME280 {

    class bme280
    {
    public:
        enum bme_status {
            STATUS_OK           = 0,
            STATUS_ERROR        = 1,

            STATUS_TOP
        };

        enum meas_oversampling {
            OVERSAMPLING_NONE   = 0,
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
            MODE_NORMAL         = 2,

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

        bme280(bme_status (*read_fp)(uint8_t, uint8_t *, size_t), bme_status (*write_fp)(uint8_t, uint8_t *, size_t), void (*delay_fp)(uint32_t));
        ~bme280();

        private:
            bme_status (* read) (uint8_t reg_addr, uint8_t *data, size_t data_len) = nullptr;
            bme_status (* write) (uint8_t reg_addr, uint8_t *data, size_t data_len) = nullptr;
            void (* delay) (uint32_t ms) = nullptr;
    };
}

#endif