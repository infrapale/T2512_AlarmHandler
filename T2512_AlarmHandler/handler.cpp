#include "main.h"
#include "handler.h"
#include "atask.h"

node_st node[NBR_OF_NODES] =
{
  {{"Piha","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
  {{"Piha","PIR2","xxxx"}, NODE_TYPE_PIR, 0},
  {{"LA","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
  {{"Tera","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
};

handler_ctrl_st hctrl = {0};

// function prototypes
void handler_task(void);

// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st h_handle           = {"Handler Task   ", 100,0, 0, 255, 0, 1, handler_task};

void handler_initialize(void)
{
    atask_add_new(&h_handle);
}

void handler_print_node(node_st *node)
{
    Serial.printf("Node: %s - %s -%s RSSI: %d  Type: %d updated: %d\n",
        node->fields.zone,
        node->fields.item,
        node->fields.value,
        node->rssi,
        node->type,
        node->last_update);
}

bool handler_parse_msg(char *msg, int16_t rssi )
{
    bool do_continue = true;
    String Msg = msg;
    String Sub;
    int indx1 = 0;
    int indx2 = Msg.indexOf(';');
    hctrl.node.rssi = rssi;

    if (indx2 < 2) do_continue = false;
    if (do_continue) {
        Sub = Msg.substring(indx1,indx2);
        Sub.toCharArray(hctrl.node.fields.zone, FIELD_LEN);
        indx1 = indx2+1;
        indx2 = Msg.indexOf(';',indx1+1);
    }
    if (indx2 < indx1) do_continue = false;
    if (do_continue) {
        Sub = Msg.substring(indx1,indx2);
        Sub.toCharArray(hctrl.node.fields.item, FIELD_LEN);
        indx1 = indx2+1;
        indx2 = Msg.indexOf(';',indx1+1);
    }
    if (indx2 < indx1) do_continue = false;
    if (do_continue) {
        Sub = Msg.substring(indx1,indx2);
        Sub.toCharArray(hctrl.node.fields.value, FIELD_LEN);
        // indx1 = indx2+1;
        // indx2 = Msg.indexOf(';',indx1+1);
    }
    if (do_continue) {
        // handler_print_node(&hctrl.node);
    }    
    else {
        Serial.printf("Message could not be parsed: %s", msg);
    }
    return do_continue;
}

void handler_process_node(void)
{
    uint8_t indx = 0;
    bool    node_found = false;
    String Field;
    int     pos;

    while(!node_found && indx < NBR_OF_NODES)
    {
        Field = node[indx].fields.zone;
        pos = Field.indexOf(hctrl.node.fields.zone);
        if (pos != -1){
            Field = node[indx].fields.item;
            pos = Field.indexOf(hctrl.node.fields.item);
            if (pos != -1){
                node_found = true;
                hctrl.node_index = indx;
            }
        }
        indx++;
    }
    if (node_found){
        Serial.printf("Node was identified, index=%d: ",hctrl.node_index);
        node[hctrl.node_index].last_update = millis();
        strncpy(node[hctrl.node_index].fields.value, hctrl.node.fields.value, FIELD_LEN);
        node[hctrl.node_index].rssi = hctrl.node.rssi;
        handler_print_node(&node[hctrl.node_index]);
    }
    else {
        Serial.print("The node could not be identified: ");
        handler_print_node(&hctrl.node);
    }
}

void handler_node_state_machine(node_st *node)
{
    switch(node->sm.state)
    {
        case 0:
            node->sm.state = 10;
            break;
        case 10:
            if(node->fields.value[0]=='H'){
                node->sm.state = 20;
                node->sm.alarm_timeout = millis() + 5000;
            } 
            break;
        case 20:
            if(millis() > node->sm.alarm_timeout){
                node->sm.wait_timeout = millis() + 30000;
                node->sm.state = 30;
            }
            break;
        case 30:
            if(millis() > node->sm.wait_timeout){
                node->sm.state = 10;
            }
            break;
        case 40:
            node->sm.state = 10;
            break;
        case 50:
            node->sm.state = 10;
            break;
    }
}

void handler_task(void)
{
    switch (h_handle.state)
    {
        case 0:
            
            h_handle.state = 10;
            break;
        case 10:
            h_handle.state = 20;
            break;
        case 0:
            h_handle.state = 10;
            break;
        case 0:
            h_handle.state = 10;
            break;
    }


    for(uint8_t indx = 0; indx < NBR_OF_NODES; indx++)
    {
        handler_node_state_machine(&node[indx]);
        if (node[indx].sm.alarm_timeout > millis()) alarm_is_active = true;        
    }
    
}