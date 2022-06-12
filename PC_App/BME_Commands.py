##
# @author: Krzysztof Półchłopek
# @date: 11.06.2022
#


class BME280():
    ## Dictionaries for initialization command
    __config_working_mode   = {0: "mode_forced", 1: "mode_normal"}
    __config_standby_time   = {0: "stime_0.5ms", 1: "stime_10ms", 2: "stime_20ms", 3: "stime_62.5ms", 
                               4: "stime_125ms", 5: "stime_250ms", 6: "stime_500ms", 7: "stime_1000ms"}
    __config_iir_filter     = {0: "filter_off", 1: "filter_2_coef", 2: "filter_4_coef", 3: "filter_8_coef", 4: "filter_16_coef"}
    __config_meas_select    = {0: "meas_off", 1: "meas_1x", 2: "meas_2x", 3: "meas_3x", 4: "meas_4x", 5: "meas_5x"}
    
    ## The Constructor
    def __init__(self):
        pass

    ## Method for initialization of sensor
    def init(self, data) -> str:
        command = "bme280_initialize " + self.__config_working_mode[data[0]] + " " + \
                self.__config_standby_time[data[1]] + " " + self.__config_iir_filter[data[2]] + \
                " temp: " + self.__config_meas_select[data[3]] + " hum: " + self.__config_meas_select[data[4]] + \
                " press: " + self.__config_meas_select[data[5]] +"\n"
        return command
    
    def deinit(self) -> str:
        command = "sensor_deinitialize\n"
        return command
    
    def recv_mesurements(self) -> str:
        command = "bme280_get_measurements\n"
        return command