// modified from https://github.com/omron-devhub/d6t-2jcieev01-raspberrypi

/* includes */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <time.h>
#include "monitor.h"


/* defines */
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4C  // for D6T-44L-06/06H, D6T-8L-09/09H, for D6T-1A-01/02

#define N_ROW 8
#define N_PIXEL 8

#define N_READ ((N_PIXEL + 1) * 2 + 1)
uint8_t rbuf[N_READ];

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C


/* I2C functions */
/** <!-- i2c_read_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr,
                       uint8_t *data, int length
) {
    int fd = open(I2CDEV, O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 21;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 22; break;
        }
        if (write(fd, &regAddr, 1) != 1) {
            fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
            err = 23; break;
        }
        int count = read(fd, data, length);
        if (count < 0) {
            fprintf(stderr, "Failed to read device(%d): %s\n",
                    count, strerror(errno));
            err = 24; break;
        } else if (count != length) {
            fprintf(stderr, "Short read  from device, expected %d, got %d\n",
                    length, count);
            err = 25; break;
        }
    } while (false);
    close(fd);
    return err;
}

/** <!-- i2c_write_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_write_reg8(uint8_t devAddr,
                        uint8_t *data, int length
) {
    int fd = open(I2CDEV, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 21;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 22; break;
        }
        if (write(fd, data, length) != length) {
            fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
            err = 23; break;
        }
    } while (false);
    close(fd);
    return err;
}

uint8_t calc_crc(uint8_t data) {
    int index;
    uint8_t temp;
    for (index = 0; index < 8; index++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {data ^= 0x07;}
    }
    return data;
}

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
    int i;
    uint8_t crc = calc_crc((D6T_ADDR << 1) | 1);  // I2C Read address (8bit)
    for (i = 0; i < n; i++) {
        crc = calc_crc(buf[i] ^ crc);
    }
    bool ret = crc != buf[n];
    if (ret) {
        fprintf(stderr,
                "PEC check failed: %02X(cal)-%02X(get)\n", crc, buf[n]);
    }
    return ret;
}


/** <!-- conv8us_s16_le {{{1 --> convert a 16bit data from the byte stream.
 */
int16_t conv8us_s16_le(uint8_t* buf, int n) {
    int ret;
    ret = buf[n];
    ret += buf[n + 1] << 8;
    return (int16_t)ret;   // and convert negative.
}


void delay(int msec) {
    struct timespec ts = {.tv_sec = msec / 1000,
                          .tv_nsec = (msec % 1000) * 1000000};
    nanosleep(&ts, NULL);
}


// this sets up the I2C communication with the D6T infrared sensor
void initRemote() {
    

    uint8_t dat1[] = {0x02, 0x00, 0x01, 0xee};
    i2c_write_reg8(D6T_ADDR, dat1, sizeof(dat1));
    uint8_t dat2[] = {0x05, 0x90, 0x3a, 0xb8};
    i2c_write_reg8(D6T_ADDR, dat2, sizeof(dat2));
    uint8_t dat3[] = {0x03, 0x00, 0x03, 0x8b};
    i2c_write_reg8(D6T_ADDR, dat3, sizeof(dat3));
    uint8_t dat4[] = {0x03, 0x00, 0x07, 0x97};
    i2c_write_reg8(D6T_ADDR, dat4, sizeof(dat4));
    uint8_t dat5[] = {0x02, 0x00, 0x00, 0xe9};
    i2c_write_reg8(D6T_ADDR, dat5, sizeof(dat5));
    delay(500);

    
}

// obtain the current temperature values from the D6T
float getRemoteTemp(){

    int i, j;

    float max = -273.15;

    float currTemp;

    int16_t itemp;

    memset(rbuf, 0, N_READ);

    // read the values from the sensor registers into the buffer
    uint32_t ret = i2c_read_reg8(D6T_ADDR, D6T_CMD, rbuf, N_READ);

    // perform CRC check on the data
    bool crcVal = D6T_checkPEC(rbuf, N_READ - 1);

    // 1st data is PTAT measurement (: Proportional To Absolute Temperature)
    //int16_t itemp = conv8us_s16_le(rbuf, 0);

    if(ret==0 && !crcVal)
        // loop temperature pixels of each thermopile measurement
        for (i = 0, j = 2; i < N_PIXEL; i++, j += 2) {

            // obtain current pixel temperature
            itemp = conv8us_s16_le(rbuf, j);

            currTemp = itemp / 10.0f;

            if(currTemp > max)
                max = currTemp;
        }

    return max;
   
}
// vi: ft=c:fdm=marker:et:sw=4:tw=80