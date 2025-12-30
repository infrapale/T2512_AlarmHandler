#ifndef __UART_H__
#define __UART_H__

typedef struct
{
    char to_tag;
    char to_addr;
    char from_tag;
    char from_addr;
    char function;
    char func_indx;
    char action;
    char value;
} uart_msg_st;


#endif