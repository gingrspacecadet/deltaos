#include <drivers/rtc.h>
#include <arch/amd64/io.h>
#include <kernel/device.h>
#include <lib/io.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

#define RTC_REG_SECOND 0x00
#define RTC_REG_MINUTE 0x02
#define RTC_REG_HOUR   0x04
#define RTC_REG_DAY    0x07
#define RTC_REG_MONTH  0x08
#define RTC_REG_YEAR   0x09
#define RTC_REG_STAT_A 0x0A
#define RTC_REG_STAT_B 0x0B

static uint8 rtc_read_reg(uint8 reg) {
    outb(CMOS_ADDR, reg);
    return inb(CMOS_DATA);
}

static int rtc_is_updating() {
    return rtc_read_reg(RTC_REG_STAT_A) & 0x80;
}

static uint8 bcd2bin(uint8 bcd) {
    return ((bcd / 16) * 10) + (bcd & 0x0F);
}

void rtc_get_time(rtc_time_t *time) {
    while (rtc_is_updating());

    time->second = rtc_read_reg(RTC_REG_SECOND);
    time->minute = rtc_read_reg(RTC_REG_MINUTE);
    time->hour = rtc_read_reg(RTC_REG_HOUR);
    time->day = rtc_read_reg(RTC_REG_DAY);
    time->month = rtc_read_reg(RTC_REG_MONTH);
    time->year = rtc_read_reg(RTC_REG_YEAR);

    uint8 registerB = rtc_read_reg(RTC_REG_STAT_B);

    //convert BCD to binary if necessary
    if (!(registerB & 0x04)) {
        time->second = bcd2bin(time->second);
        time->minute = bcd2bin(time->minute);
        time->hour = ((time->hour & 0x0F) + (((time->hour & 0x70) / 16) * 10)) | (time->hour & 0x80);
        time->day = bcd2bin(time->day);
        time->month = bcd2bin(time->month);
        time->year = bcd2bin(time->year);
    }

    //convert 12h to 24h if necessary
    if (!(registerB & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }

    //calculate full year (assume 21st century for simplicity if not provided)
    time->year += 2000;
}

static struct device rtc_dev;

void rtc_init() {
    rtc_dev.name = "rtc";
    rtc_dev.type = DEV_TIME;
    rtc_dev.subtype = SUBDEV_RTC;
    rtc_dev.ops = NULL; //no standard ops yet as we use rtc_get_time directly for now
    rtc_dev.private = NULL;

    device_register(&rtc_dev);
    puts("[rtc] initialized\n");
}
