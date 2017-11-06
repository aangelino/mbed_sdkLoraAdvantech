/**
 * @file main.cpp
 *
 * @brief Lora Node SDK Sample
 *
 * example show how to set/save config and read/send sensor data via LoRa periodically;
 *
 * @author AdvanWISE
*/


#include "mbed.h"
#include "node_api.h"
#include "DebouncedInterrupt.h"

#define NODE_DEBUG(x,args...) node_printf_to_serial(x,##args)

#define NODE_DEEP_SLEEP_MODE_SUPPORT 0  ///< Flag to Enable/Disable deep sleep mode
#define NODE_ACTIVE_PERIOD_IN_SEC 10    ///< Period time to read/send sensor data
#define NODE_ACTIVE_TX_PORT 1          ///< Lora Port to send data



extern Serial debug_serial; ///< Debug serial port
extern Serial m2_serial;    ///< M2 serial port

/*
typedef enum

{
	NODE_STATE_INIT,            ///< Node init state
	NODE_STATE_LOWPOWER = 1,    ///< Node low power state
	NODE_STATE_ACTIVE,          ///< Node active state
	NODE_STATE_TX,              ///< Node tx state
	NODE_STATE_RX_DONE,         ///< Node rx done state
}node_state_t;

struct node_api_ev_rx_done node_rx_done_data;
volatile node_state_t node_state = NODE_STATE_INIT;
static char node_class=1;

static unsigned int  node_sensor_temp_hum=0; ///<Temperature and humidity sensor global
static unsigned int  g_water_counter=0; ///<var counter global
*/
//DigitalOut led(PA_8);
InterruptIn gpio0(PA_8);
//DigitalIn gpio0(PA_8);

//InterruptIn gpio1(PC_8);
//InterruptIn gpio2(PC_7);
//InterruptIn gpio3(PC_5);
//InterruptIn gpio4(PB_0);
//InterruptIn gpio5(PA_3);
//InterruptIn gpio6(PA_2);
//InterruptIn gpio7(PB_6);

//I2C i2c(PC_1, PC_0); ///<i2C define

/** @brief print message via serial
 *
 *  @param format message to print
 *  @returns 0 on success
 */
int node_printf_to_serial(const char * format, ...)
{
	unsigned int i;
	va_list ap;

	char buf[512 + 1];
	memset(buf, 0, 512+1);

	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), (char *)format, ap);
	va_end(ap);

	for(i=0; i < strlen(buf); i++)
	{
		debug_serial.putc(buf[i]);
	}

	return 0;
}

void button_push_isr( void )
{
    NODE_DEBUG("\n\r button interrupt \n\r");
}



int main ()
{
	/*Create sensor thread*/
	//Thread *p_node_sensor_temp_hum_thread, *p_node_water_counter_thread;

	/* Init carrier board, must be first step */
	nodeApiInitCarrierBoard();

	debug_serial.baud(115200);
	m2_serial.baud(115200);

	nodeApiInit();

	//p_node_sensor_temp_hum_thread=new Thread(node_sensor_temp_hum_thread);
	//p_node_water_counter_thread=new Thread(water_counter_thread);

	//node_show_version();

	/*
	 * Init configuration at beginning
	 */
	//node_set_config();

	/* Apply to module */
	//nodeApiApplyCfg();

	/* Start Lora */
	//nodeApiStartLora();

	//node_get_config();

	//Thread::wait(1000);

	/*debounce code*/
	DebouncedInterrupt user_interrupt(PA_8);
	user_interrupt.attach(button_push_isr, IRQ_RISE, 100, true);

	//gpio0.mode(PullUp);
	//gpio0.rise(&Int_gpio0);
	//gpio0.enable_irq();
	/*gpio1.rise(&Int_gpio1);
	gpio2.rise(&Int_gpio2);
	gpio3.rise(&Int_gpio3);
	gpio4.rise(&Int_gpio4);
	gpio5.rise(&Int_gpio5);
	gpio6.rise(&Int_gpio6);
	gpio7.rise(&Int_gpio7);*/

	/*
	 *  Node state loop
	 */
	//node_state_loop();
while(1)
{

		NODE_DEBUG("\n\r into while \n\r");
		Thread::wait(3000);

}
	/*Never reach here*/
	return 0;
}
