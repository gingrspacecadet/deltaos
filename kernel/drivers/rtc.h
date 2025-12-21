#ifndef DRIVERS_RTC_H
#define DRIVERS_RTC_H

#include <arch/types.h>

typedef struct {
    uint8 second;
    uint8 minute;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint32 year;
} rtc_time_t;

void rtc_init();
void rtc_get_time(rtc_time_t *time);

#endif
