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


void handler_initialize(void);

bool handler_parse_msg(char *msg, int16_t rssi );

//void handler_process_event(event_radio_msg_st ev):

void handler_debug_print(void);

#endif