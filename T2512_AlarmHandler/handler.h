#ifndef __HANDLER_H__
#define __HANDLER_H__
#define FIELD_LEN   8
#define NBR_OF_NODES  4

typedef enum
{
    NODE_TYPE_UNDEF = 0,
    NODE_TYPE_PIR,
    NODE_TYPE_DOOR,
    NODE_TYPE_TEMPERATURE,
    NODE_TYPE_NBR_OF,
} node_type_et;

typedef struct
{
    char  zone[FIELD_LEN];
    char  item[FIELD_LEN];
    char  value[FIELD_LEN];
} node_field_values_st;

typedef struct
{
    uint8_t state;
    uint32_t alarm_timeout;
    uint32_t wait_timeout;
} node_state_machine_st;

typedef struct
{
    node_field_values_st fields;
    node_type_et type;
    //char  value[FIELD_LEN];
    int16_t rssi;
    uint32_t last_update;
    node_state_machine_st sm; 
} node_st;

typedef struct
{
    node_st node;
    uint8_t node_index;
    uint8_t alarm_level;
    uint8_t prev_alarm_level;
    uint32_t timeout;
} handler_ctrl_st;



void handler_initialize(void);

bool handler_parse_msg(char *msg, int16_t rssi );

void handler_process_node(void);

void handler_debug_print(void);

#endif