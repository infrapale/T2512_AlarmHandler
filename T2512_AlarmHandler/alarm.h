#ifndef __ALARM_H__
#define __ALARM_H__

#include "main.h"
#include "Rfm69Modem.h"
#define ALARM_UNITS_NBR_OF   5

typedef enum
{
    HOME_STATE_AWAY = 0,
    HOME_STATE_AT_MAIN,
    HOME_STATE_AT_STUDIO,
    HOME_STATE_AT_PIHA,
    HOME_STATE_AT_BEACH,
    HOME_STATE_NBR_OF
} home_state_et;

typedef enum
{
    ALARM_NO  = 0,
    ALARM_ALL,
    ALARM_VA1,
    ALARM_LA1,
    ALARM_PIHA1,
    ALARM_RANTA1,
    ALARM_GLOCK1,
    ALARM_NBR_OF
}   alarm_unit_et;

typedef enum
{
    ALARM_BM_NO        = 0b00000000,
    ALARM_BM_ALL       = 0b11111111,
    ALARM_BM_VA1       = 0b00000001,
    ALARM_BM_LA1       = 0b00000010,
    ALARM_BM_PIHA1     = 0b00000100,
    ALARM_BM_RANTA1    = 0b00001000,
    ALARM_BM_GLOCK1    = 0b00010000
}   alarm_unit_bm_et;

typedef enum
{
    SOUND_NO   = 0,
    SOUND_1,
    SOUND_2,
    SOUND_3,
    SOUND_4,
    SOUND_5,
    SOUND_NBR_OF
} sound_indx_et;

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

void alarm_raise_event(event_indx_et evindx);

void alarm_clear_event(event_indx_et evindx);

void alarm_clear_all_events(void);

void alarm_debug_print(void);

#endif