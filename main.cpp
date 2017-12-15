/**
 * Node1
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
#include "pinOutMap_Wise1510.h"
#include "DebouncedInterrupt.h"

#include "stm32l4xx_hal.h"

/***************DEBUG SETTINGS************/
/*GENERATE PWM to debug counter on rising edge*/
#define DPCONTROL_DEBUG_PWM 0
/*set digital out to follow digital in (only debug)*/
#define DPCONTROL_DEBUG_OSCILLOSCOPE 0
/*Printf on rising and falling interrupt*/
#define DPCONTROL_DEBUG_PRINT_INT_RISE 0
#define DPCONTROL_DEBUG_PRINT_INT_FALL 0

/*Printf Rx/Tx done*/
#define DPCONTROL_DEBUG_PRINT_RX_DONE 0
#define DPCONTROL_DEBUG_PRINT_TX_DONE 0
/*Print Info*/
#define DPCONTROL_DEBUG_PRINT_START 1

#define NODE_DEBUG(x,args...) node_printf_to_serial(x,##args)

#define NODE_DEEP_SLEEP_MODE_SUPPORT 0  ///< Flag to Enable/Disable deep sleep mode
#define NODE_ACTIVE_PERIOD_IN_SEC 5    ///< Period time to read/send sensor data
#define NODE_ACTIVE_TX_PORT 1          ///< Lora Port to send data

extern Serial debug_serial; ///< Debug serial port
extern Serial m2_serial;    ///< M2 serial port

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
volatile unsigned int  g_water_counter=0; ///<var counter global

static void MX_GPIO_Init(void);
TIM_HandleTypeDef htim1;
long count=0;

/*Interrupt In*/
InterruptIn intIn_13(ADC_3);

#if DPCONTROL_DEBUG_OSCILLOSCOPE
/*Digital In*/
DigitalIn din_12(ADC_2,PullDown);
#endif

/*Digital Out*/
DigitalOut dout_2(GPIO_2);

/*I2C*/
I2C i2c(I2C0_DATA, I2C0_CLK); ///<i2C define

/*PwmOut*/
PwmOut pwmOut_8(PWM_0);//onda quadra fisicamente in din_12

//#if DPCONTROL_DEBUG_TIMER
/*Timer*/
Timer timer_water_thread;
//#endif

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

void callback_isr13_rise( void )
{
	g_water_counter++;

/*Set Out to follow in (debug only)*/
#if DPCONTROL_DEBUG_OSCILLOSCOPE
	dout_2.write(intIn_13.read());
#endif

#if DPCONTROL_DEBUG_PRINT_INT_RISE
	NODE_DEBUG("\n\r INTO INTERRUPT 13_RISE---> g_water_counter=%d    \n\r",g_water_counter);
#endif
}

void callback_isr13_fall( void )
{
/*Set OUT to follow IN (debug only)*/
#if DPCONTROL_DEBUG_OSCILLOSCOPE

	#if DPCONTROL_DEBUG_PRINT_INT_FALL
		NODE_DEBUG("\n\r INTO INTERRUPT 13_FALL---> g_water_counter=%d    \n\r",g_water_counter);
	#endif

		dout_2.write(intIn_13.read());
#endif
}

/** @brief Temperature and humidity sensor read
 *
 */
static unsigned int hdc1510_sensor(void)
{
	char data_write[3];
	char data_read[4];

	#define HDC1510_REG_TEMP  0x0
	#define HDC1510_ADDR 0x80

	data_write[0]=HDC1510_REG_TEMP;
	i2c.write(HDC1510_ADDR, data_write, 1, 1);
	Thread::wait(50);
	i2c.read(HDC1510_ADDR, data_read, 4, 0);
	float tempval = (float)((data_read[0] << 8 | data_read[1]) * 165.0 / 65536.0 - 40.0);

	/*Temperature*/
	int ss = tempval*100;
	unsigned int yy=0;
	//printf("Temperature: %.2f C\r\n",tempval );
	/*Humidity*/
	float hempval = (float)((data_read[2] << 8 | data_read[3]) * 100.0 / 65536.0);
	yy=hempval*100;
	// printf("Humidity: %.2f %\r\n",hempval);

	return (yy<<16)|ss;

}

/** @brief Temperature and humidity sensor thread
 *
 */
static void node_sensor_temp_hum_thread(void const *args)
{
  while(1)
	{
			Thread::wait(10000);
	    node_sensor_temp_hum=(unsigned int )hdc1510_sensor();
	}

}


/** @brief node tx procedure done
 *
 */
int node_tx_done_cb(void)
{
#if DPCONTROL_DEBUG_PRINT_TX_DONE
	NODE_DEBUG("into node_tx_done_cb -->1.node_state=NODE_STATE_LOWPOWER\r\n");
#endif
	node_state=NODE_STATE_LOWPOWER;
	return 0;
}

/** @brief node got rx data */
int node_rx_done_cb(struct node_api_ev_rx_done *rx_done_data)
{
#if DPCONTROL_DEBUG_PRINT_RX_DONE
	NODE_DEBUG("into node_rx_done_cb\r\n");
#endif
	memset(&node_rx_done_data,0,sizeof(struct node_api_ev_rx_done));
	memcpy(&node_rx_done_data,rx_done_data,sizeof(struct node_api_ev_rx_done));
	node_state=NODE_STATE_RX_DONE;
	return 0;
}

/** @brief An example to show version */
void node_show_version()
{
	char buf_out[256];
	unsigned short ret=NODE_API_OK;

	memset(buf_out, 0, 256);
	ret=nodeApiGetVersion(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("Version=%s\r\n", buf_out);
	}
}

/** @brief An example to set node config */
void node_set_config()
{
	char deveui[32]={};
	char devaddr[16]={};


	if(nodeApiGetFuseDevEui(deveui,16)!=NODE_API_OK)
	{
		NODE_DEBUG("Get fuse DevEui failed\r\n");
		return;
	}

	NODE_DEBUG("\n\r fuse DevEui node1 : %x\n\r",deveui);

	//set ABP mode
	nodeApiSetDevEui(deveui);
	nodeApiSetAppEui("00000000000000ab");										//Set Application EUI for OTAA mode;
	nodeApiSetAppKey("00000000000000000000000000000011");		//Set Application Key for OTAA mode;
	strcpy(devaddr,&deveui[8]);
	nodeApiSetDevAddr(devaddr);															//Set Network session key for ABP mode;
	nodeApiSetNwkSKey("00000000000000000000000000000011");	//Set Network session key for ABP mode;
	nodeApiSetAppSKey("00000000000000000000000000000011");	//Set Application session key for ABP mode;
	nodeApiSetDevActMode("2");														  //Set Activation mode(1:OTAA| 2:ABP; Default:2)
	nodeApiSetDevOpMode("1");																//Set Device Operating Mode (1:Advanwise| 2:LoRaWan | 3:MACless; Default:1)
	nodeApiSetDevClass("3");																//Set Device Class (1:classA| 2:classB| 3:classC; Default:1) N.B class B not implemented yet
	nodeApiSetDevAdvwiseDataRate("4");											//Set Advanwise mode Date Rate
	//nodeApiSetDevAdvwiseFreq("923300000");
	nodeApiSetDevAdvwiseFreq("868500000");//Node 1
	//nodeApiSetDevAdvwiseFreq("869050000");//Node 2
	nodeApiSetDevAdvwiseTxPwr("20");


}

/** @brief An example to get node config */
void node_get_config()
{
	char buf_out[256];
	unsigned short ret=NODE_API_OK;

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevEui(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevEui=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetAppEui(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("AppEui=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevAddr(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevAddr=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetNwkSKey(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("NwkSKey=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetAppSKey(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG( "AppSKey=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevActMode(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevActMode=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevOpMode(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevOpMode=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevClass(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevClass=%s\r\n", buf_out);
		node_class=atoi(buf_out);
	}


	memset(buf_out, 0, 256);
	ret=nodeApiGetDevAdvwiseDataRate(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("AdvwiseDataRate=%s\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevAdvwiseFreq(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevAdvwiseFreq=%sHz\r\n", buf_out);
	}

	memset(buf_out, 0, 256);
	ret=nodeApiGetDevAdvwiseTxPwr(buf_out, 256);
	if(ret==NODE_API_OK)
	{
		NODE_DEBUG("DevAdvwiseTxPwr=%sdBm\r\n", buf_out);
	}

}

/** @brief Read sensor data
 *
 *  A simple sample to generate sensor data, user should implement read sensor data
 *  @param data sensor_data
 *  @returns data_length
 */
unsigned char node_get_sensor_data (char *data)
{
	unsigned char len=0;
	unsigned char sensor_data[32];

	memset(sensor_data,0x0,sizeof(sensor_data));

	sensor_data[len+2]=0x1;//id=1 temperature
	len++;
	sensor_data[len+2]=0x3;// len:3 bytes
	len++;
	sensor_data[len+2]=0x0;//0 is positive, 1 is negative
	len++;
	sensor_data[len+2]=(node_sensor_temp_hum>>8)&0xff;//LSB value
	len++;
	sensor_data[len+2]=node_sensor_temp_hum&0xff;//MSB value
	len++;
	sensor_data[len+2]=0x2;//id=2 humidity
	len++;
	sensor_data[len+2]=0x2;// len:2 bytes
	len++;
	sensor_data[len+2]=(node_sensor_temp_hum>>24)&0xff;//LSB value
	len++;
	sensor_data[len+2]=(node_sensor_temp_hum>>16)&0xff;//MSB value
	len++;
	//mycode water counter
	sensor_data[len+2]=0x3;//id=3 count
	len++;
	sensor_data[len+2]=0x2; // len:2 bytes
	len++;
	sensor_data[len+2]=(g_water_counter>>8)&0xff;
	len++;
	sensor_data[len+2]=g_water_counter&0xff;
	len++;
	//header
	sensor_data[0]=len;
	sensor_data[1]=0xc; //publish
	memcpy(data, sensor_data,len+2);

	return len+2;
}

/** @brief An loop to read and send sensor data via LoRa periodically
 *
 */
void node_state_loop()
{
	static unsigned char join_state=0;/*0:init state, 1:not joined, 2: joined*/

	nodeApiSetTxDoneCb(node_tx_done_cb);
	nodeApiSetRxDoneCb(node_rx_done_cb);

	//NODE_DEBUG("5.node_state=NODE_STATE_LOWPOWER\r\n");
	node_state=NODE_STATE_LOWPOWER;

	while(1)
	{
		if(nodeApiJoinState()==0)
		{
			if(join_state==2)
				NODE_DEBUG("LoRa is not joined.\r\n");

			#if NODE_DEEP_SLEEP_MODE_SUPPORT
			nodeApiSetDevSleepRTCWakeup(1);
			#else
			Thread::wait(1000);
			#endif

			join_state=1;
			continue;
		}
		else
		{
			if(join_state<=1)
			{
				node_class=nodeApiDeviceClass();
				NODE_DEBUG("LoRa Joined.\r\n\r\n");
				//NODE_DEBUG("2.node_state=NODE_STATE_LOWPOWER\r\n");
				node_state=NODE_STATE_LOWPOWER;
			}

			join_state=2;
		}

		switch(node_state)
		{
			case NODE_STATE_LOWPOWER:
			//NODE_DEBUG("NODE_STATE_LOWPOWER\n\r ");
			{
				if(node_class==3)
				{
					//NODE_DEBUG("node_class3\n\r ");
					static int count_NSLP=0;
					#if NODE_DEEP_SLEEP_MODE_SUPPORT
					nodeApiSetDevSleepRTCWakeup(1);
					#else
					Thread::wait(1000);
					#endif

					if(node_state!=NODE_STATE_RX_DONE)
					{
						//NODE_DEBUG("count_NSLP = % d\n\r ",count_NSLP);
						if(count_NSLP%NODE_ACTIVE_PERIOD_IN_SEC==0)
						{
							node_state=NODE_STATE_ACTIVE;
						}
					}
					count_NSLP++;

				}
				else
				{
					NODE_DEBUG("NODE_DEEP_SLEEP_MODE_SUPPORT =%d\n\r ",NODE_DEEP_SLEEP_MODE_SUPPORT);
					#if NODE_DEEP_SLEEP_MODE_SUPPORT
					nodeApiSetDevSleepRTCWakeup(NODE_ACTIVE_PERIOD_IN_SEC);
					#else
					Thread::wait(NODE_ACTIVE_PERIOD_IN_SEC*1000);
					#endif

					if(node_state!=NODE_STATE_RX_DONE)
						node_state=NODE_STATE_ACTIVE;
				}
			}
				break;
			case NODE_STATE_ACTIVE:
			//NODE_DEBUG("NODE_STATE_ACTIVE\n\r ");
			{
				int i=0;
				unsigned char frame_len=0;
				char frame[64]={};


				frame_len=node_get_sensor_data(frame);

				if(frame_len==0)
				{
					NODE_DEBUG("3.node_state=NODE_STATE_LOWPOWER\r\n");
					node_state=NODE_STATE_LOWPOWER;
					break;
				}

				NODE_DEBUG("TX: ");

				for(i=0;i<frame_len;i++)
				{
					NODE_DEBUG("%02x ",frame[i]);
				}

				NODE_DEBUG("\n\r");

				nodeApiSendData(NODE_ACTIVE_TX_PORT, frame, frame_len);

				node_state=NODE_STATE_TX;
			}
				break;
			case NODE_STATE_TX:
				//NODE_DEBUG("NODE_STATE_TX");
				break;
			case NODE_STATE_RX_DONE:
			NODE_DEBUG("NODE_STATE_RX_DONE\n\r ");
			{
				if(node_rx_done_data.data_len!=0)
				{
					int i=0;
					int j=0;
					char print_buf[512];
					memset(print_buf, 0, sizeof(print_buf));

					NODE_DEBUG("RX: ");
					for(i=0;i<node_rx_done_data.data_len;i++)
					{
						NODE_DEBUG("%x ", node_rx_done_data.data[i]);
					}

					NODE_DEBUG("(Length: %d, Port%d)", node_rx_done_data.data_len,node_rx_done_data.data_port);

					NODE_DEBUG("\n\r");

				}
				NODE_DEBUG("4.node_state=NODE_STATE_LOWPOWER\r\n");
				node_state=NODE_STATE_LOWPOWER;
				break;
			}
			default:
			  NODE_DEBUG("default state\n\r ");
				break;

		}
	}
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  //htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
		printf("\r\n _Error_Handler\r\n");
		while(1){};
    //_Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 50;
  if (HAL_TIM_SlaveConfigSynchronization(&htim1, &sSlaveConfig) != HAL_OK)
  {
		printf("\r\n _Error_Handler\r\n");
		while(1){};
    //_Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
		printf("\r\n _Error_Handler\r\n");
		while(1){};
    //_Error_Handler(__FILE__, __LINE__);
  }

}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}


/*@brief Main function*/
int main ()
{
	/*Create sensor thread*/
	Thread *p_node_sensor_temp_hum_thread;

	/* Init carrier board, must be first step */
	nodeApiInitCarrierBoard();

	debug_serial.baud(115200);
	m2_serial.baud(115200);

	nodeApiInit();

	p_node_sensor_temp_hum_thread=new Thread(node_sensor_temp_hum_thread);

	node_show_version();

	/*Init configuration at beginning*/
	node_set_config();

	/* Apply to module */
	nodeApiApplyCfg();

	/* Start Lora */
	nodeApiStartLora();

	node_get_config();

	Thread::wait(1000);

	/*Gpio Settings*/
	intIn_13.mode(PullDown);
	intIn_13.rise(&callback_isr13_rise);

	//intIn_7.mode(PullDown);
	//intIn_7.rise(&callback_isr7);


#if DPCONTROL_DEBUG_OSCILLOSCOPE
	intIn_13.fall(&callback_isr13_fall);
#endif

#if DPCONTROL_DEBUG_PWM
	/*Pwm Settings*/
	pwmOut_8.period_ms(1);      		// Periof(Frequency)  1ms(1kHz)
	pwmOut_8.write(0.50f);      // 50% duty cycle, relative to period
#endif

NODE_DEBUG("\n\rHAL_Init\r\n");
HAL_Init();
NODE_DEBUG("\n\rMX_GPIO_Init\r\n");
MX_GPIO_Init();
NODE_DEBUG("\n\rMX_TIM1_Init\r\n");
MX_TIM1_Init();
NODE_DEBUG("\n\rHAL_TIM_Base_Start\r\n");
HAL_TIM_Base_Start(&htim1);    //Start TIM1 without interrupt
	/*Debounce Settings*/
	//DebouncedInterrupt user_interrupt2(GPIO_2, PullDown);
	//user_interrupt2.attach(button_push_isr2, IRQ_RISE, 10, true);
	//DebouncedInterrupt user_interrupt12(ADC_2/GPIO_12, PullDown);
	//user_interrupt12.attach(button_push_isr12, IRQ_RISE, 10, true);

#if DPCONTROL_DEBUG_PRINT_START
		NODE_DEBUG("\n\rSystemCoreClock=%d\r\n",SystemCoreClock);
		NODE_DEBUG("\n\rStart Node Loop\r\n");
#endif

	/*Node state loop*/
	//node_state_loop();
count = __HAL_TIM_GetCounter(&htim1);
printf("\n\r count=%d\n\r",count);
printf("\n\r Reset Counter \n\r");
/* Reset the Counter Register value */
  TIM1->CNT = 0;
printf("\n\r count=%d\n\r",count);
while(1)
{
	 count = __HAL_TIM_GetCounter(&htim1);    //read TIM2 counter value
	 printf("\n\r count=%d\n\r",count);
	 Thread::wait(1000);
}
	/*Never reach here*/
	return 0;
}
