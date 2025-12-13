/*****************************************************************************
T2512_RFM69_Modem 
*******************************************************************************

HW: Adafruit M0 RFM69 Feather or Arduino Pro Mini + RFM69

Send and receive data via UART

*******************************************************************************
https://github.com/infrapale/T2310_RFM69_TxRx
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
*******************************************************************************


*******************************************************************************
Sensor Radio Message:   {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                        {"Z":"Dock","S":"T_dht22","V":"8.7","R":"-"}
Relay Radio Message     {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
Sensor Node Rx Mesage:  <#X1N:OD1;Temp;25.0;->
Relay Node Rx Mesage:   <#X1N:RMH1;RKOK1;T;->

Relay Mesage      <#R12=x>   x:  0=off, 1=on, T=toggle

*******************************************************************************
**/

#include <Arduino.h>
#include "main.h"
#ifdef ADAFRUIT_FEATHER_M0
#include <wdt_samd21.h>
#endif
#ifdef PRO_MINI_RFM69
#include "avr_watchdog.h"
#endif
#include "secrets.h"
#include <RH_RF69.h>
#include "atask.h"
#include "Rfm69Modem.h"
#include "io.h"
#include "handler.h"

//*********************************************************************************************
#define SERIAL_BAUD   9600
#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h
RH_RF69         rf69(RFM69_CS, RFM69_INT);
Rfm69Modem      rfm69_modem(&rf69, MY_MODULE_TAG, MY_MODULE_ADDR, RFM69_RST, PIN_LED_ONBOARD );
modem_data_st   modem_data = {MY_MODULE_TAG, MY_MODULE_ADDR};

#define NBR_TEST_MSG  4
#define LEN_TEST_MSG  32
const char test_msg[NBR_TEST_MSG][LEN_TEST_MSG] =
{  //12345678901234567890123456789012
    "<R1X1J1:Dock;T_bmp1;9.1;->",
    "<R1X1J1:Dock;T_dht22;8.7;->",
    "<R1X1J1:Dock;T_Water;5.3;->",
    "<R1X1J1:Dock;ldr1;0.33;->",
};

void debug_print_task(void);
void run_100ms(void);
void send_test_data_task(void);
void rfm_receive_task(void); 
void modem_task(void);


atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};
atask_st clock_handle              = {"Tick Task      ", 100,0, 0, 255, 0, 1, run_100ms};
atask_st modem_handle              = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
#ifdef SEND_TEST_MSG
atask_st send_test_data_handle     = {"Send Test Task ", 10000,0, 0, 255, 0, 1, send_test_data_task};
#endif

#ifdef PRO_MINI_RFM69
//AVR_Watchdog watchdog(4);
#endif


void initialize_tasks(void)
{
  atask_add_new(&debug_print_handle);
  atask_add_new(&clock_handle);
  atask_add_new(&modem_handle);
  #ifdef SEND_TEST_MSG
  atask_add_new(&send_test_data_handle);
  #endif

}


void setup() 
{
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer
    delay(2000);
    Serial.begin(9600);
    Serial.print(__APP__); Serial.print(F(" Compiled: "));
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.println();

    SerialX.begin(9600);
    atask_initialize();
    io_initialize();
    initialize_tasks();
    handler_initialize();
    uint8_t key[] = RFM69_KEY;
    rfm69_modem.initialize(key);
    rfm69_modem.radiate(__APP__);
    #ifdef ADAFRUIT_FEATHER_M0
    // Initialze WDT with a 2 sec. timeout
    wdt_init ( WDT_CONFIG_PER_16K );
    #endif
    #ifdef PRO_MINI_RFM69
    //watchdog.set_timeout(4);
    #endif
}

#define BUFF_LEN   80
char mbuff[BUFF_LEN];
int16_t rssi;
void loop() 
{

    atask_run();  
    if(rfm69_modem.msg_is_avail())
    {
        // rfm69_modem.get_msg(mbuff, BUFF_LEN, false);
        rfm69_modem.get_msg_decode(mbuff, BUFF_LEN, true);
        rssi = rfm69_modem.get_last_rssi();
        // Serial.print(mbuff); Serial.print(" RSSI: "); Serial.println(rssi);
        if (handler_parse_msg(mbuff,rssi))
        {
            handler_process_node();
        }
        // delay(1000);
        //rfm69_modem.radiate_node_json((char*) "<R1X1J1:Dock;T_bmp1;9.1;->");
        //rfm69_modem.radiate("OK");
    }
}

void modem_task(void)
{
    rfm69_modem.modem_task();
}


void run_100ms(void)
{
    io_run_100ms();
}

void debug_print_task(void)
{
  atask_print_status(true);
  handler_debug_print();
}

#ifdef SEND_TEST_MSG
void send_test_data_task(void)
{
    if  (send_test_data_handle.state >= NBR_TEST_MSG ) send_test_data_handle.state = 0;

    uart_p->rx.str  = test_msg[send_test_data_handle.state];
    uart_p->rx.avail = true;
    send_test_data_handle.state++;
}
#endif

