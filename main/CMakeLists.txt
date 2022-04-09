set(srcs "../src/RTC1302.c" "../src/Ultrasonicsensor.c" "../src/main_functions.cc" "../src/AnalogSensors.c" 
    "../src/model.cc" "../src/wificonnection.c" "../src/HiveMQBroker.c" "../src/ConfigurationHelper.cc")

idf_component_register(SRCS "${srcs}" "main.c"
                    INCLUDE_DIRS "." "../include")
