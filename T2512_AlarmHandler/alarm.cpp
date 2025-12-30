#include "main.h"
#include "Rfm69Modem.h"
#include "handler.h"
#include "alarm.h"
#include "atask.h"

typedef struct 
{
    Rfm69Modem  *modem;
    uint32_t    timeout;
} alarm_ctrl_st;

extern modem_data_st   modem_data;

void alarm_task(void);

// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st a_handle           = {"Alarm   Task   ", 100,0, 0, 255, 0, 1, alarm_task};


alarm_ctrl_st actrl;
// Alarm Message
// <ALARM;A1;3;Piha>
alarm_test_msg_st test_alarm[4] = 
{
    {.severity=3, .rem="Piha1", .state=0, .interval=44000, .duration=10000, .timeout=0},
    {.severity=3, .rem="Piha2", .state=0, .interval=64000, .duration=8000, .timeout=0},
    {.severity=3, .rem="Tera",  .state=0, .interval=111000, .duration=10000, .timeout=0},
    {.severity=3, .rem="LA",    .state=0, .interval=133000, .duration=10000, .timeout=0},
};

void alarm_initialize(Rfm69Modem  *rfm69_modem)
{
    actrl.modem = rfm69_modem;
    // atask_add_new(&a_handle);
}
// <ALARM;A1;3;Piha>
void alarm_send(uint8_t severity, char *rem )
{
    char msg[40];
    sprintf(msg,"<ALARM;%c%c;%d;%s>",
        modem_data.tag,
        modem_data.addr,
        severity,
        rem);
    Serial.println(msg);
    //actrl.modem->radiate(msg);
}

bool alarm_test_state_machine(alarm_test_msg_st *atp)
{
    bool did_radiate = false;
    switch(atp->state)
    {
        case 0:
            atp->state = 10;
            break;
        case 10:
            atp->state = 20;
            atp->timeout = millis()+atp->interval;
        case 20:
            if (millis() > atp->timeout){
                alarm_send(atp->severity, atp->rem);
                did_radiate = true;
                atp->timeout = millis()+atp->duration;
                atp->state = 30;
            }
            break;
        case 30:
            if (millis() > atp->timeout){
                alarm_send(0, atp->rem);
                did_radiate = true;
                atp->state = 10;
            }
            break;
        case 40:
            atp->state = 10;
            break;
    }
    return did_radiate;
}

void alarm_task(void)
{
    static uint8_t  test_indx = 0;

    switch(a_handle.state)
    {
        case 0:
            a_handle.state = 10;
            break;
        case 10:
            if(test_indx >= 4) test_indx=0;
            if (alarm_test_state_machine(&test_alarm[test_indx])){
                a_handle.state = 20;
                actrl.timeout = millis() + 2000;
            }
            test_indx++;
            break;
        case 20:
            if (millis() > actrl.timeout) a_handle.state = 10;
            break;
        case 30:
            a_handle.state = 10;
            break;

    }
}