/**
* @file node_api.h
* @brief Node SDK API 
* 
*
* @author AdavnWISE
*/


#ifndef _NODE_API_H_
#define _NODE_API_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define NODE_API_OK	         	0x0	///< Node API Result: OK
#define NODE_API_INVALID_ARG 	0x1	///< Node API Result: Invalid Argument
#define NODE_API_CFG_DEC_ERROR 		0x2	///< Node API Result: Config Decode Error
#define NODE_API_CFG_LEN_ERROR 		0x3	///< Node API Result: Config Length Error
#define NODE_API_CFG_NULL 		0x4		///< Node API Result: Config NULL
#define NODE_API_CFG_ENC_ERROR 		0x5	///< Node API Result: Config Encode Error


struct node_api_ev_rx_done
{
	unsigned char data_port;///< Node LoRa rx done data port
	unsigned char data_len;///< Node LoRa rx done data len
	unsigned char data[256];///< Node LoRa rx done data 
};

typedef int(*EventTxDoneFP)(void);///< Node LoRa event callback tx done function pointer
typedef int(*EventRxDoneFP)(struct node_api_ev_rx_done*);///< Node LoRa event callback rx done function pointer
typedef int(*EventBeaconFP)(unsigned char);///< Node LoRa event callback beacon function pointer


/** Init Carrier Board
 *
 *  Do this if node module connected with carrier board
 *  @returns NODE_API_OK on success
 */
unsigned short nodeApiInitCarrierBoard();


/** Init
 *
 *  Must do this to Init
 *  @returns NODE_API_OK on success
 */
unsigned short nodeApiInit();



/** Start Lora
 *
 *  Must do this to Start LoRa
 *  @returns NODE_API_OK on success
 */
unsigned short nodeApiStartLora();


/** Send sensor data over LoRa
 *
 *  @param port port number
 *  @param data sensor data buffer
 *  @param data_len length of sensor data
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSendData(unsigned char port, char *data, unsigned short data_len);


/** Get LoRa Join State
 *
 *  @returns 0 if not joined; >0 if joined
 */
int nodeApiJoinState();


/** Get Node current running device class
 *
 *  @returns device class
 */
unsigned char nodeApiDeviceClass();


/** Get Dev EUI
 *
 *  Get Device EUI; length is 16, only consist of "0~9" or "A~F"
 *  @param buf_out Dev EUI if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevEui(char * buf_out, unsigned short buf_len);


/** Get App EUI
 *
 *  Get Application EUI for OTAA mode; length is 16, only consist of "0~9" or "A~F"
 *  @param buf_out App EUI if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetAppEui(char * buf_out, unsigned short buf_len);


/** Get App Key
 *
 *  Get Application Key for OTAA mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_out App Key if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetAppKey(char * buf_out, unsigned short buf_len);


/** Get Dev Addr
 *
 *  Get Device Addr for ABP mode; length is 8, only consist of "0~9" or "A~F"
 *  @param buf_out Dev Addr if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevAddr(char * buf_out, unsigned short buf_len);


/** Get Network Session Key
 *
 *  Get Network session key for ABP mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_out Network Session Key if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetNwkSKey(char * buf_out, unsigned short buf_len);


/** Get App Session Key
 *
 *  Get Application session key for ABP mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_out App Session Key if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetAppSKey(char * buf_out, unsigned short buf_len);


/** Get Dev Act Mode
 *
 *  Get Activation mode(1:OTAA| 2:ABP; Default:2)
 *  @param buf_out Dev Act Mode if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevActMode(char * buf_out, unsigned short buf_len);


/** Get Dev Op Mode
 *
 *  Get Device Operating Mode (1:Advanwise| 2:LoRaWan | 3:MACless; Default:1)
 *  @param buf_out Dev Op Mode if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevOpMode(char * buf_out, unsigned short buf_len);


/** Get Dev Class
 *
 *  Get Device Class (1:classA| 2:classB| 3:classC; Default:1)
 *  @param buf_out Dev Class if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevClass(char * buf_out, unsigned short buf_len);


/** Get Advanwise mode Frequency
 *
 *  Get Advanwise mode frequency
 *  @param buf_out Frequency if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevAdvwiseFreq(char * buf_out, unsigned short buf_len);


/** Get Advanwise mode Data Rate
 *
 *  Get Advanwise mode Date Rate
 *  @param buf_out Data Rate if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevAdvwiseDataRate(char * buf_out, unsigned short buf_len);


/** Get Advanwise mode Tx power
 *
 *  Get Advanwise mode Transmit power
 *  @param buf_out Tx Power if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetDevAdvwiseTxPwr(char * buf_out, unsigned short buf_len);


/** Get Version
 *
 *  Get Version
 *  @param buf_out Version if return NODE_API_OK
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetVersion(char * buf_out, unsigned short buf_len);


/** Set Dev EUI
 *
 *  Set Device EUI; length is 16, only consist of "0~9" or "A~F"
 *  @param buf_in Dev EUI to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevEui(char * buf_in);


/** Set App EUI
 *
 *  Set Application EUI for OTAA mode; length is 16, only consist of "0~9" or "A~F"
 *  @param buf_in App EUI to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetAppEui(char * buf_in);


/** Set App Key
 *
 *  Set Application Key for OTAA mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_in App Key to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetAppKey(char * buf_in);


/** Set Dev Addr
 *
 *  Set Device Addr for ABP mode; length is 8, only consist of "0~9" or "A~F"
 *  @param buf_in Dev Addr to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevAddr(char * buf_in);


/** Set Network Session Key
 *
 *  Set Network session key for ABP mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_in Network Session Key to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetNwkSKey(char * buf_in);


/** Set App Session Key
 *
 *  Set Application session key for ABP mode; length is 32, only consist of "0~9" or "A~F"
 *  @param buf_in App Session Key to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetAppSKey(char * buf_in);


/** Set Dev Act Mode
 *
 *  Set Activation mode(1:OTAA| 2:ABP; Default:2)
 *  @param buf_in Dev Act Mode to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevActMode(char * buf_in);


/** Set Dev Op Mode
 *
 *  Set Device Operating Mode (1:Advanwise| 2:LoRaWan | 3:MACless; Default:1)
 *  @param buf_in Dev Op Mode to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevOpMode(char * buf_in);


/** Set Dev Class
 *
 *  Set Device Class (1:classA| 2:classB| 3:classC; Default:1)
 *  @param buf_in Dev Class to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevClass(char * buf_in);;


/** Set Advanwise mode Frequency
 *
 *  Set Advanwise mode frequency
 *  @param buf_in Frequency to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevAdvwiseFreq(char * buf_in);


/** Set Advanwise mode Data Rate
 *
 *  Set Advanwise mode Date Rate
 *  @param buf_in Data Rate to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevAdvwiseDataRate(char * buf_in);


/** Set Advanwise mode Tx Power
 *
 *  Set Advanwise mode Transmit power
 *  @param buf_in Tx Power to set
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevAdvwiseTxPwr(char * buf_in);


/** Set Deep Sleep mode Wakeup timer
 *
 *  Set Into Deep Sleep mode Wakeup timer with RTC
 *  @param sec wakeup time in second
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetDevSleepRTCWakeup(int sec);


/** Factory Reset
 *
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiFactoryReset();


/** Save Config
 *
 *  Save Config to flash
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSaveCfg();


/** Reboot node
 *
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiReboot();


/** Load Config
 *
 *  Load Config from flash
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiLoadCfg();


/** Apply Config
 *
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiApplyCfg();


/** Get Fuse Dev EUI
 *
 *  Get Fuse Device EUI; length is 16, only consist of "0~9" or "A~F"
 *  @param buf_out Fuse Dev EUI if return 0
 *  @param buf_len length of buf_out 
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiGetFuseDevEui(char * buf_out, unsigned short buf_len);


/** Set Tx done event call back
 *
 *  Set event callback function get TxDone event
 *  @param txdone_cb event callback function
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetTxDoneCb(EventTxDoneFP txdone_cb);

/** Set Rx done event call back
 *
 *  Set event callback function get RxDone event
 *  @param rxdone_cb event callback function
 *  @returns NODE_API_OK on success; others on failed
 */
unsigned short nodeApiSetRxDoneCb(EventRxDoneFP rxdone_cb);


#ifdef __cplusplus
}
#endif

#endif
