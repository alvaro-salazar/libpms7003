/*
 * The MIT License
 *
 * Copyright 2019 Alvaro Salazar <alvaro@denkitronik.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   libpms7003.h
 * Author: Alvaro Salazar <alvaro@denkitronik.com>
 *
 * Created on 30 de abril de 2019, 10:21 PM
 */

#ifndef LIBPMS7003_H
#define LIBPMS7003_H

#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>
#define ERROR 0  ///<
#define OK    1  ///<
    /**
     * This union represents the registers sent by the Plantower PMS7003 sensor: 
     * (See the datasheet [1] for more information)
     */
    /// PMS7003 Registers
    union Pms {
        uint8_t buffer[32]; ///< Buffer received with the data that contain all the registers

        struct {
            uint16_t header; ///< 0x424d: No error.\n 1: There is an error on the communication with the sensor	
            uint16_t length; ///< Bytes to be sent to the Raspberry Pi
            uint16_t pm1_cf; ///< PM1.0 concentration unit ug/m3(CF=1, standard particle)
            uint16_t pm25_cf; ///< PM2.5 concentration unit ug/m3(CF=1, standard particle)
            uint16_t pm10_cf; ///< PM10 concentration unit ug/m3(CF=1, standard particle)
            uint16_t pm1_atm; ///< PM1.0 concentration unit * ug/m3(under atmospheric environment)
            uint16_t pm25_atm; ///< PM2.5 concentration unit * ug/m3(under atmospheric environment)
            uint16_t pm10_atm; ///< PM10 concentration unit * ug/m3(under atmospheric environment)
            uint16_t num_part_03; ///< number of particles with diameter beyond 0.3 um in 0.1 L of air.
            uint16_t num_part_05; ///< number of particles with diameter beyond 0.5 um in 0.1 L of air.
            uint16_t num_part_1; ///< number of particles with diameter beyond 1.0 um in 0.1 L of air.
            uint16_t num_part_25; ///< number of particles with diameter beyond 2.5 um in 0.1 L of air.
            uint16_t num_part_5; ///< number of particles with diameter beyond 5.0 um in 0.1 L of air.
            uint16_t num_part_10; ///< number of particles with diameter beyond 10 um in 0.1 L of air.
            uint16_t reserved; ///< Reserved.
            uint16_t check_code; ///< Check code= summation from buffer[0] to buffer[29]. //@todo Implement a mechanism to check errors.
        } reg;
    };
    union Pms sensorPMS;        ///< Data registers received from PMS7003
    char * getSerialNumberCPU();
    void printJson();
    int readPMS();

#ifdef __cplusplus
}
#endif

#endif /* LIBPMS7003_H */

