# T2311_RFM69_Modem

https://github.com/infrapale/T2512_RFM69_Modem_Library.git

*******************************************************************************
                  -------------------
                  |                 |
    <---UART----->|    RFM69_Modem  |<-----RFM69 433 MHz --------->
                  |                 |
                  -------------------
                       |
                       <--------->  Library Object Interface 

*******************************************************************************
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all

*******************************************************************************
Library Object Interface:
- Rfm69Modem(RH_RF69 *rf69p, char mod_tag, char mod_addr, uint8_t pin_rfm69_rst,uint8_t pin_led);
- static void led_on(uint32_t duration_ms);
- void initialize(uint8_t key[]);
- void modem_task(void);
- void radiate(char *buff);
- void radiate_node_json(char *buff);
- bool msg_is_avail(void);
- void receive(char *buff, uint8_t max_len, bool clr_avail);
- void receive_decode(char *buff, uint8_t max_len, bool clr_avail);
- int16_t get_last_rssi(void);

#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h
RH_RF69         rf69(RFM69_CS, RFM69_INT);
Rfm69Modem      rfm69_modem(&rf69, MY_MODULE_TAG, MY_MODULE_ADDR, RFM69_RST, PIN_LED_ONBOARD );
modem_data_st   modem_data = {MY_MODULE_TAG, MY_MODULE_ADDR};
  
setup()
. . .
    rfm69_modem.initialize(key);
    rfm69_modem.radiate(__APP__);
. . .

Usage Example:
  #define BUFF_LEN   80
  char mbuff[BUFF_LEN];
  . . .
  if(rfm69_modem.msg_is_avail())
  {
        rfm69_modem.receive(mbuff, BUFF_LEN, true);
        Serial.println(mbuff);
        delay(500);
        rfm69_modem.radiate_node_json("<R1X1J1:Dock;T_bmp1;9.1;->");
        //rfm69_modem.radiate("OK");
   }


*******************************************************************************
UART Command Syntax

    rfm_send_msg_st *rx_msg = &send_msg; 
    Frame: <R1F1O2:L>\r\n
            \\\\\\\\______ value, message (optional)
             \\\\\\\______ action set: '=' get: '?' reply: ':'
              \\\\\\______ index
               \\\\\______ function
                \\\\______ sender addr
                 \\\______ sender tag
                  \\______ module_addr target
                   \______ module_tag target

*******************************************************************************

    module_tag  and sender tag identifies the type of module
      R Radio
      Y Relay
      K Keypad
      # All modules
    module_addr and sender addr
      0-9
      # All modules
    Radio Functions
      A Is messages available
      R Read Raw Message
      D Read Decoded Message
      T Transmit Raw Message
      J Send Desoced Message as json
      I Get last RSSI
      S Set parameter
    Index
      0-9
      0 is also used when N/A
    Action
      set: '=' 
      get: '?' 
      reply: ':'
    Value
      Total max 61 characters for RFM69
      '*'  Not applicable or relevant

*******************************************************************************

    Examples:
      Check if we have available messages:  
          <R1X1A0?*>  
          <X1R1A0:1>    1 message is available   
      Get Last RSSI:  
          <R1X1I0?*>  
          <X1R1I0:-34>    1 message is available   
      Read raw messages:      
          <R1X1R0?*>   
          <X1R1R0:{"Z":"MH1","S":"RKOK1","V":"T","R":"-"}>
      Read decoded messages:  
          <R1X1D0?*>
          <X1R1D0:OD_1;Temp;23.1;->
          <X1R1D0:*>    No message available
      Transmit Raw Message
          <R1X1T0={"Z":"MH1","S":"RKOK1","V":"T","R":"-"}>
          +-+-+-  {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
      Send Decoded Message as JSON
          <R1X1J0=RMH1;RKOK1;T;->
          +-+-+-  {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
      Set Parameter
          <R1X1S0=1>  Set transparent mode 
          <R1X1S0=0>  Set polling mode mode 


*******************************************************************************

        ******** UART ***************** Transmit Raw ********* Radio ********************
                                          --------
         <#X1T:Hello World>\n             |      |
        >-------------------------------->|      | Hello World
                                          |      |-------------------------------------->
                                          |      |
        <---------------------------------|      |
                                          |      |<-------------------------------------
                                          --------
      
        ******** UART ***************** Transmit Node ********* Radio ********************
                                          --------
         <#X1N:RMH1;RKOK1;T;->\n          |      |
        >-------------------------------->|      | {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
                                          |      |-------------------------------------->
                                          |      |
        <---------------------------------|      |
                                          |      |<-------------------------------------
                                          --------
  
      ******** UART *************** Check Radio Data ********* Radio ********************
                                        --------
      <#X1A:>\n                         |      |
      >-------------------------------->|      | 
      <#X1a:0>\n                        |      |
      <---------------------------------|      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                        |      |<-------------------------------------
      <#X1A:>\n                         |      |
      >-------------------------------->|      | 
      <#X1a:1>\n                        |      |
      <---------------------------------|      | 
                                        |      |
      '                                 --------
    
      ******** UART ************ Read Radio Raw Data ********* Radio ********************
       '                                --------
                                        |      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                        |      |<-------------------------------------
      <#X1R:>\n                         |      |
      >-------------------------------->|      | 
      <#X1r:{"Z":"OD_1","S":"Temp",     |      |
      "V":23.1,"R":"-"}>                |      |
      <---------------------------------|      | 
                                        |      |
      '                                 --------
    
      ******** UART ************ Read Radio Node Data ********* Radio ********************
       '                                --------
                                        |      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                        |      |<-------------------------------------
      <#X1O:>\n                         |      |
      >-------------------------------->|      | 
      <#X1o:OD_1;Temp;23.1;->\n         |      |
      <---------------------------------|      | 
                                        |      |
       '                                --------
---
#UART Commands
- UART_CMD_TRANSMIT_RAW   = 'T',
- UART_CMD_TRANSMIT_NODE  = 'N',
- UART_CMD_GET_AVAIL      = 'A',
- UART_CMD_READ_RAW       = 'R',
- UART_CMD_READ_NODE      = 'O' 

#UART Replies
- UART_REPLY_AVAILABLE    = 'a',
- UART_REPLY_READ_RAW     = 'r',
- UART_REPLY_READ_NODE    = 'o' 

*******************************************************************************
  Sensor Radio Message:   {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                          {"Z":"Dock","S":"T_dht22","V":"8.7","R":"-"}
  Relay Radio Message     {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
  Sensor Node Rx Mesage:  <#X1N:OD1;Temp;25.0;->
  Relay Node Rx Mesage:   <#X1N:RMH1;RKOK1;T;->

  Relay Mesage      <#R12=x>   x:  0=off, 1=on, T=toggle

*******************************************************************************



