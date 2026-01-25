#include "main.h"
#include "Rfm69Modem.h"
#include "handler.h"
#include "alarm.h"
#include "atask.h"

typedef struct 
{
    Rfm69Modem  *modem;
    uint8_t     event_state[EVENT_NBR_OF];
    home_state_et home_state;
    uint32_t    timeout;
} alarm_ctrl_st;

typedef struct
{
    uint8_t sound[EVENT_NBR_OF];
    uint8_t repeat[EVENT_NBR_OF];
    uint8_t alarm_at;
    //uint8_t sound;
} event_action_st;


// ALARM_BM_ALL || ALARM_BM_VA1 || ALARM_BM_LA1 || ALARM_BM_PIHA1 || ALARM_BM_RANTA1 || ALARM_BM_GLOCK1
// SOUND_DEFAULT SOUND_BEEP1 SOUND_RING1 SOUND_RING2 SOUND_BIGBEN SOUND_HORN1 SOUND_GLOCK1, SOUND_GLOCK2

severity_home_st event_action[HOME_STATE_NBR_OF] =
{
    [HOME_STATE_AWAY] = 
    {  // EVENT ->    PIHA1    PIHA2   RANTA1   RANTA2  
        .sound  = { SOUND_5, SOUND_5, SOUND_5, SOUND_5},
        .repeat = {       4,       4,       2,       2},
        .alarm_at = ALARM_BM_ALL 
    },
    [HOME_STATE_AT_MAIN]= 
    {  // EVENT ->    PIHA1    PIHA2   RANTA1   RANTA2  
        .sound  = { SOUND_1, SOUND_1, SOUND_1, SOUND_1},
        .repeat = {       4,       4,       2,       2},
        .alarm_at = ALARM_BM_ALL 
    },
    [HOME_STATE_AT_STUDIO]= 
    {  // EVENT ->    PIHA1    PIHA2   RANTA1   RANTA2  
        .sound  = { SOUND_2, SOUND_2, SOUND_2, SOUND_2},
        .repeat = {       4,       4,       2,       2},
        .alarm_at = ALARM_BM_VA1 || ALARM_BM_LA1 || ALARM_BM_GLOCK1
    },
    [HOME_STATE_AT_PIHA]= 
    {  // EVENT ->    PIHA1    PIHA2   RANTA1   RANTA2  
        .sound  = { SOUND_0, SOUND_0, SOUND_1, SOUND_1},
        .repeat = {       4,       4,       2,       2},
        .alarm_at = ALARM_BM_ALL 
    },
    [HOME_STATE_AT_BEACH]= 
    {  // EVENT ->    PIHA1    PIHA2   RANTA1   RANTA2  
        .sound  = { SOUND_5, SOUND_5, SOUND_0, SOUND_0},
        .repeat = {       4,       4,       2,       2},
        .alarm_at = ALARM_BM_ALL 
    }
};

extern modem_data_st   modem_data;

void alarm_task(void);

// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st a_handle           = {"Alarm   Task   ", 100,0, 0, 255, 0, 1, alarm_task};

char alarm_module_tag[ALARM_NBR_OF][8] =
{   //                     01234567
    [ALARM_NO]          = "No",  
    [ALARM_ALL]         = "All",  
    [ALARM_VA1]         = "VA1",
    [ALARM_LA1]         = "LA1",
    [ALARM_PIHA1]       = "Piha1",
    [ALARM_RANTA1]      = "Ranta1",
    [ALARM_GLOCK1]      = "Glock1",
};


// char  sound_tag[SOUND_NBR_OF][8] =
// {   //                     01234567
//     [SOUND_DEFAULT]     = "Default",
//     [SOUND_BEEP1]       = "Beep1",    
//     [SOUND_RING1]       = "Ring1",
//     [SOUND_RING2]       = "Ring2",
//     [SOUND_BIGBEN]      = "BigBen",
//     [SOUND_HORN1]       = "Horn1",
//     [SOUND_GLOCK1]      = "Glock1",
//     [SOUND_GLOCK2]      = "Glock2"
// };
    
alarm_ctrl_st actrl = {0};

void alarm_initialize(Rfm69Modem  *rfm69_modem)
{
    actrl.modem = rfm69_modem;
    actrl.home_state = HOME_STATE_AWAY;
    atask_add_new(&a_handle);
}
// <ALARM;A1;3;Piha>

void alarm_debug_print(void)
{
    Serial.print("Alarm Event State:");
    for(uint8_t evindx=0; evindx < EVENT_NBR_OF; evindx++) {
        Serial.printf("%d - ",actrl.event_state[evindx]);
    }
    Serial.println(); 
}

void alarm_send(uint8_t alarm_unit_indx, uint8_t sound_indx, uint8_t repeat_nbr)
{
    char msg[40];
    sprintf(msg,"<BELL;%s;%s;%d>",
        alarm_module_tag[alarm_unit_indx],
        sound_tag[sound_indx],
        repeat_nbr
    );

    Serial.println(msg);
    actrl.modem->radiate(msg);
}
void alarm_raise_event(event_indx_et evindx)
{
    if(evindx < EVENT_NBR_OF) actrl.event_state[evindx] = 1;
}
void alarm_clear_event(event_indx_et evindx)
{
     if(evindx < EVENT_NBR_OF) actrl.event_state[evindx] = 0;
}
void alarm_clear_all_events(void)
{
    for(uint8_t evindx=0; evindx < EVENT_NBR_OF; evindx++) actrl.event_state[evindx] = 0;
}

void alarm_task(void)
{
    static uint8_t event_raised = 99;
    static uint8_t alarm_unit_indx = 0;
    uint8_t alarm_unit_bm;
    uint8_t sound_indx;
    uint8_t repeat_nbr;

    switch(a_handle.state)
    {
        case 0:
            a_handle.state = 10;
            break;
         case 10:
            event_raised = 99;
            a_handle.state = 20;
            break;
        case 20:
            for(uint8_t i = 0; i < EVENT_NBR_OF; i++){
                if(actrl.event_state[i] !=0 ) event_raised = i;
            }
            if (event_raised < 99){
                alarm_unit_indx = ALARM_ALL+1;
                //  alarm_send(event_raised);
                a_handle.state = 50;
            }
            break;
        case 50:
            alarm_unit_bm = 1 << alarm_unit_indx;

            if((event_action[actrl.home_state].alarm_at & alarm_unit_bm) != 0){
                sound_indx = event_action[actrl.home_state].sound[event_raised];
                repeat_nbr = event_action[actrl.home_state].repeat[event_raised];
                alarm_send(alarm_unit_indx, sound_indx, repeat_nbr);
                actrl.timeout = millis() + 10000;
                a_handle.state = 100;
            }
            else {
                a_handle.state = 60;
            }
            break;
        case 60:
            if(++alarm_unit_indx < ALARM_NBR_OF) a_handle.state = 50;
            else a_handle.state = 10;
            break;
        case 100:
            if (millis() > actrl.timeout) {
                a_handle.state = 60;
            }
            break ;
        case 200:
            a_handle.state = 10;
            break;

    }
}