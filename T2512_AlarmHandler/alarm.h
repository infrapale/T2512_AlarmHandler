#ifndef __ALARM_H__
#define __ALARM_H__

// <ALARM;A1;3;Piha>
typedef struct
{
    uint8_t     severity;
    char        rem[FIELD_LEN];
    uint16_t     state;
    uint32_t    interval;
    uint32_t    duration;
    uint32_t    timeout;
} alarm_test_msg_st;

void alarm_initialize(Rfm69Modem  *rfm69_modem);

#endif