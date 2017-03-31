

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"
#include<string.h>

uint8 T_VALUE = 32; //温度上限 ，自行修改
uint8 H_VALUE = 55; //湿度上限 ，自行修改
uint8 C_VALUE = 3; //烟雾浓度报警 ，自行修改 只能改为 1/2/3其中一个
/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8 count=0;
uint8 TEM,HUM,CO2,ADDR;
uint8 A_CMD1[]="EAE10000";
uint8 A_CMD2[]="EAE20000";
uint8 A_CMD3[]="EAE30000";
uint8 B_CMD1[]="EBE40000";
uint8 B_CMD2[]="EBE50000";
uint8 B_CMD3[]="EBE60000";
uint8 C_CMD1[]="ECE10000";
uint8 C_CMD2[]="ECE20000";
uint8 C_CMD3[]="ECE30000";
// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
 SAMPLEAPP_PERIODIC_CLUSTERID1,
 SAMPLEAPP_PERIODIC_CLUSTERID2,

};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)


afAddrType_t SampleApp_P2P_DstAddr;      //点播
afAddrType_t SampleApp_Periodic_DstAddr;      //广播
aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_Send_P2P_Message(void);
void callback(uint8 port,uint8 event);
void Delay_MS(uint8 x);
//ms延时函数
void Delay_MS(uint8 x)
{
  uint8 i;
  for(i=0;i<x;i++)
  MicroWait(1000);
}
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();                  //串口初始化
  MT_UartRegisterTaskID(task_id); //注册串口任务
  
  
 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif
  
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit; //点播 
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT; 
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;            //两终端以此地址将测量值发送到协调器
  
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;//广播
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;       //发给终端，此项目协调器不向终端发送数据，所以不用。

  //装载描述符信息，默认。
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // 注册端点描述符
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

 
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      应用层任务轮询函数，所有任务都在这里顺序执行
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        // 按键任务，未使用
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        //接收到无线消息，调用消息解析函数
        case AF_INCOMING_MSG_CMD: 
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // 硬件初始化
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( //(SampleApp_NwkState == DEV_ZB_COORD) ||
              
               //只有温度终端启动1.1s定时循环发送温湿度
               (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            //定时启动任务
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
 //未使用，不得删除 
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_PERIODIC_CLUSTERID1://接收到终端消息，串口发送至PC   
        {
         
         if(pkt->cmd.Data[0]==1)//A号终端
          {
           TEM = pkt->cmd.Data[1]*10+pkt->cmd.Data[2];
           HUM = pkt->cmd.Data[3]*10+pkt->cmd.Data[4];
           CO2 = pkt->cmd.Data[5];
           if(TEM>T_VALUE||HUM>H_VALUE||CO2==C_VALUE) B_CMD2[7] = '1';
           else B_CMD2[7] = '0';             
           AF_DataRequest(   &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID1,
                       8,
                       B_CMD2,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );
           A_CMD1[6] = pkt->cmd.Data[1]+0X30;
           A_CMD1[7] = pkt->cmd.Data[2]+0X30;
           A_CMD2[6] = pkt->cmd.Data[3]+0X30;
           A_CMD2[7] = pkt->cmd.Data[4]+0X30;
           A_CMD3[7] = pkt->cmd.Data[5]+0X30;
           HalUARTWrite(0, A_CMD1, 8); //输出接收到的数据
  //         Delay_MS(60);//延时60ms  延时时间 自行修改
           HalUARTWrite(0, A_CMD2, 8); //输出接收到的数据
   //        Delay_MS(60);//延时60ms  延时时间 自行修改
           HalUARTWrite(0, A_CMD3, 8); //输出接收到的数据
          }
         if(pkt->cmd.Data[0]==2)//C号终端
          {
           TEM = pkt->cmd.Data[1]*10+pkt->cmd.Data[2];
           HUM = pkt->cmd.Data[3]*10+pkt->cmd.Data[4];
           CO2 = pkt->cmd.Data[5];
           if(TEM>T_VALUE||HUM>H_VALUE||CO2==C_VALUE) B_CMD2[7] = '1';
           else B_CMD2[7] = '0';             
           AF_DataRequest(   &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID1,
                       8,
                       B_CMD2,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );
           C_CMD1[6] = pkt->cmd.Data[1]+0X30;
           C_CMD1[7] = pkt->cmd.Data[2]+0X30;
           C_CMD2[6] = pkt->cmd.Data[3]+0X30;
           C_CMD2[7] = pkt->cmd.Data[4]+0X30;
           C_CMD3[7] = pkt->cmd.Data[5]+0X30;
           HalUARTWrite(0, C_CMD1, 8); //输出接收到的数据温度
           Delay_MS(60);//延时60ms  延时时间 自行修改
           HalUARTWrite(0, C_CMD2, 8); //输出接收到的数据湿度
           Delay_MS(60);//延时60ms  延时时间 自行修改
           HalUARTWrite(0, C_CMD3, 8); //输出接收到的数据烟雾
          }
        }    
  }  
}

/*********************************************************************
 * @fn      SampleApp_Send_P2P_Message
 *
 * @brief   point to point.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_Send_P2P_Message( void )                                      
{
}

/*********************************************************************
*********************************************************************/
void callback(uint8 port, uint8 event)
{
  byte cmd[9];
  uint8 count;
  (void)port;
  if (event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
  {
   HalUARTRead(0, cmd, 8);//从串口读取指令
   AF_DataRequest(   &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID1,
                       8,
                       cmd,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );    
  }
}