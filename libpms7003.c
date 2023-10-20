/**
 *  Linux Raspbian (Raspberry PI) Shared Library for Plantower PMS7003
 *  Digital universal particle concentration sensor
 *  @version 1.0.0
 *  @author Alvaro Salazar <alvaro@denkitronik.com>
 *  http://www.denkitronik.com
 *
 */

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
 * File:   main.c
 * Author: Alvaro Salazar <alvaro@denkitronik.com>
 * @file
 * @brief This is a test application for libpms7003
 * Created on 10/03/2019 , 12:47 AM
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringSerial.h> //@todo We don't want to depend of any 3rd party libraries to read the serial port
#include <stdint.h>
#include <libpms7003.h>

#define MAX_TRIES 3  //Maximum number of tries of getting the CPU serial number
#define STOP 5       //Stop trying to get the CPU serial number
//extern union Pms sensorPMS;


/**
 * Method to read the sensor PMS7003
 * @param device full path to the serial port device.\n Raspberry PI 3: /dev/ttyS0\n Raspberry PI 2 or earlier: /dev/ttyAMA0
 * @return Error code. Valid values are: ERROR, OK
 */
int readPMS(const char * device) {
    uint16_t checksum = 0;
    int fd; //File descriptor
    unsigned char i; //Variable used to iterate over the buffer 
    if ((fd = serialOpen(device, 9600)) < 0) { //We will open the ttyAMA0 Serial port (*see Raspberry Pi documentation)
        //fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno)); //If we have any problem using the serial port (unplugged?)
        return ERROR;
    }

    // We start waiting for 0x42 and then 0x4d to know when we start receiving data in this order: buffer[1], buffer[0]
    do {
        while ((sensorPMS.buffer[1] = serialGetchar(fd)) != 0x42);
    } while ((sensorPMS.buffer[0] = serialGetchar(fd)) != 0x4d);
    // We received a start data, now we will save them in this order: buffer[3], buffer[2], buffer[5], buffer[4], buffer[7]...
    // in order to avoid having the data stored in BIG ENDIAN, we need them in LITTLE ENDIAN (this is an ARM arch)
    for (i = 2; i < 31; i++) {
        sensorPMS.buffer[i + 1] = serialGetchar(fd);
        sensorPMS.buffer[i] = serialGetchar(fd);
        i++;
    }
    serialClose(fd);
    // Let's check if the data length is 28
    if (sensorPMS.reg.length == 28) {

        // We will calculate the checksum (adding each byte in the buffer from 0 to 29 index)
        for (i = 0; i < 30; i++) {
            checksum += sensorPMS.buffer[i];
        }
        // Let's check if the checksum is OK.     
        if (checksum == sensorPMS.reg.check_code) {
            return OK;
        } else {
            return ERROR;
        }
    } else {
        return ERROR; // Error, the data lenght is not 28
    }
}

/**
 * Method to get the CPU serial number
 * @return a char * with the CPU serial number
 */
char * getSerialNumberCPU() {
    static char serial[18];
    FILE *fp;
    fp = popen("sudo cat /proc/cpuinfo | grep Serial | cut -d ' ' -f 2", "r");
    if (fp != NULL) {
        fgets(serial, sizeof (serial) - 1, fp);
    }
    pclose(fp);
    return serial;
}

/**
 * Method to print in JSON format the data sensor stored in sensorPMS
 */
void printJson() {
    char json[250];
    int a = 5;
    int b = 10;
    int times = 0;
    char * result;
    setbuf(stdout, NULL); //We wont give any buffering to stout (printf() function) in order to print the message immediately.
    // This block of code tries to get the CPU serial number, return "not available id" if it couldnt get the serial (4 tries).
    do {
        result = getSerialNumberCPU();
        if (result[0] == '\0') {
            if (times > MAX_TRIES) {
                strcpy(result, "not available id");
                times = STOP;
            } else {
                times++;
            }
        } else {
            times = STOP;
        }
    } while (times < STOP);

    sprintf(json,
            "{\"serial_cpu\":\"%s\",\"pm1\":%d,\"pm25\":%d,\"pm10\":%d,\"num_part_03\":%d,\"num_part_05\":%d,\"num_part_1\":%d,\"num_part_25\":%d,\"num_part_5\":%d,\"num_part_10\":%d}",
            result,
            sensorPMS.reg.pm1_atm,
            sensorPMS.reg.pm25_atm,
            sensorPMS.reg.pm10_atm,
            sensorPMS.reg.num_part_03,
            sensorPMS.reg.num_part_05,
            sensorPMS.reg.num_part_1,
            sensorPMS.reg.num_part_25,
            sensorPMS.reg.num_part_5,
            sensorPMS.reg.num_part_10);
    printf("%s", json);
    fflush(stdout); //@todo check if this fflush is necessary
}