/*
 * EF_WLAN.c
 *
 *  Created on: 1 Nov 2016
 *      Author: Mohamed Samy
 *
 *  Description: Source file of Wireless LAN (WLAN) library. Contains its Global variables defintions
 *  and shared functions defintions.
 *
 */

#include "EF_WLAN.h"


/*
 * Wifi module GLOBAL VARIABLES -- Start
 */
uBuffer uBuf;
_u8      BsdBuf_1[BUF_SIZE];
_u8      g_Status = 0;
uint32_t g_ui32SysClock;                      /* System clock shared between the WiFi part and Ethernet part*/
_i16     SockID_sr_1;                         /* The main socket ID*/
_i16     SockID_sr_2[MAX_SOCKET_NUMBER];      /* Array of clients sockets (Clients tablets) */
_u8      gu8APConnectionStatus = DISCONNECTED; /* Access point connection status */
/*
 * GLOBAL VARIABLES -- End
 */

/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */


/*
 * STATIC FUNCTION DEFINITIONS -- End
 */

/*!
    \brief This function configure the SimpleLink device in its default state. It:
           - Sets the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregisters mDNS services
           - Remove all filters

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
 */
_i32 configureSimpleLinkToDefaultState()
{
    SlVersionFull                        ver            = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};
//    _u8           val = 1;        /* Used for DHCP*/
    _u8                configOpt = 0;
    _u8                configLen = 0;
    _u8                power     = 0;
    _i32               retVal    = -1;
    _i32               mode      = -1;
    SlNetCfgIpV4Args_t ipV4;


    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(mode);

    /* If the device is not in station-mode, try configuring it in staion-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything todo Ask: may stuck? */
            while(!IS_IP_ACQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal    = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (_u8 *)(&ver));
    ASSERT_ON_ERROR(retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    //    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    //    ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(62,240,110,198);            // _u32 DNS server address

    /* Define static IP address */
    ipV4.ipV4          = WLAN_IP_ADDRESS;         // _u32 IP address
    ipV4.ipV4Mask      = WLAN_SUBNET_MASK;        // _u32 Subnet mask for this STA/P2P
    ipV4.ipV4Gateway   = WLAN_GATEWAY;            // _u32 Default gateway address
    ipV4.ipV4DnsServer = WLAN_DNS_SERVER;         // _u32 DNS server address
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4);
    sl_Stop(0);
    sl_Start(NULL,NULL,NULL);
    ASSERT_ON_ERROR(retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal    = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power  = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&power);
    ASSERT_ON_ERROR(retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove  all 64 filters (8*8) */
    pal_Memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
            sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(retVal);

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     0 on success, negative error-code on error

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.  //todo Ask: stuck!
 */
_i32 establishConnectionWithAP()
{
    SlSecParams_t secParams = {0};
    _i32          retVal    = 0;

    secParams.Key    = PASSKEY;
    secParams.KeyLen = pal_Strlen(PASSKEY);
    secParams.Type   = SEC_TYPE;
    //todo Ask: what is the name
    retVal = sl_WlanConnect( SSID_NAME, pal_Strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR ( retVal );

    /* Wait */
    while((!IS_CONNECTED(g_Status)) || (!IS_IP_ACQUIRED(g_Status))) { _SlNonOsMainLoopTask(); }

    return SUCCESS;
}

/*!
    \brief Opening a client side socket and sending data

    This function opens a TCP socket and tries to connect to a Server IP_ADDR
    waiting on port PORT_NUM. If the socket connection is successful then the
    function will send 1000 TCP packets to the server.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note

    \warning        A server must be waiting with an open TCP socket on the
                    right port number before calling this function.
                    Otherwise the socket creation will fail.
 */
_i32 BsdTcpClient(_u16 Port)
{
    SlSockAddrIn_t  Addr;

    _u16          idx           = 0;
    _u16          AddrSize      = 0;
    _i16          SockID        = 0;
    _i16          Status        = 0;
    static _u8    u8SockCounter = 0;



    for (idx=0 ; idx < BUF_SIZE ; idx++)
    {
        uBuf.BsdBuf[idx] = (_u8)(idx % 10);
    }

    Addr.sin_family      = SL_AF_INET;
    Addr.sin_port        = sl_Htons((_u16)Port);
    Addr.sin_addr.s_addr = sl_Htonl((_u32)IP_ADDR);
    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    SockID_sr_2[u8SockCounter] = SockID;
    if( SockID < 0 )
    {
        UARTprintf(" [TCP Client] Create socket Error \n\r");
        ASSERT_ON_ERROR(SockID);
    }

    Status = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, AddrSize);
    if( Status < 0 )
    {
        sl_Close(SockID);
        UARTprintf(" [TCP Client]  TCP connection Error \n\r");
        ASSERT_ON_ERROR(Status);
    }

    u8SockCounter++;
    return SUCCESS;
}

/*!
    \brief Opening a server side socket and receiving data

    This function opens a TCP socket in Listen mode and waits for an incoming
    TCP connection. If a socket connection is established then the function
    will try to read 1000 TCP packets from the connected client.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note           This function will wait for an incoming connection till one
                    is established

    \warning
 */
_i32 BsdTcpServer(_u16 Port)
{
    SlSockAddrIn_t      LocalAddr;
    _u16                idx = 0;
    _u16                AddrSize = 0;
    _i16                SockID = 0;
    _i32                Status = 0;
    SlSockNonblocking_t enableOption;
    SlSockKeepalive_t   enableAliveOption;



    for (idx=0 ; idx<BUF_SIZE ; idx++)
    {
        uBuf.BsdBuf[idx] = (_u8)(idx % 10);
    }

    LocalAddr.sin_family      = SL_AF_INET;
    LocalAddr.sin_port        = sl_Htons((_u16)Port);
    LocalAddr.sin_addr.s_addr = 0;

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( SockID < 0 )
    {
        UARTprintf(" [TCP Server] Create socket Error \n\r");
        ASSERT_ON_ERROR(SockID);
    }

    enableAliveOption.KeepaliveEnabled = 0;
    sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_KEEPALIVE, (_u8 *)&enableAliveOption,sizeof(enableAliveOption));

    enableOption.NonblockingEnabled = 1;
    sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_NONBLOCKING, (_u8 *)&enableOption,sizeof(enableOption)); // Enable/disable nonblocking mode

    AddrSize = sizeof(SlSockAddrIn_t);
    Status   = sl_Bind(SockID, (SlSockAddr_t *)&LocalAddr, AddrSize);
    if( Status < 0 )
    {
        sl_Close(SockID);
        UARTprintf(" [TCP Server] Socket address assignment Error \n\r");
        ASSERT_ON_ERROR(Status);
    }

    Status = sl_Listen(SockID, MAX_SOCKET_NUMBER);
    if( Status < 0 )
    {
        sl_Close(SockID);
        UARTprintf(" [TCP Server] Listen Error \n\r");
        ASSERT_ON_ERROR(Status);
    }

    SockID_sr_1 = SockID;

    return SUCCESS;
}

/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
 */
_i32 initializeAppVariables()
{
    g_Status = 0;
    pal_Memset(uBuf.BsdBuf, 0, sizeof(uBuf));

    return SUCCESS;
}

/*!
    \brief This function displays the application's banner

    \param      None

    \return     None
 */
void displayBanner()
{
#ifdef PRINT_VERBOSE
    UARTprintf("\n\r\n\r");
    UARTprintf(" TCP socket application - Version ");
    UARTprintf(APPLICATION_VERSION);
    UARTprintf("\n\r*******************************************************************************\n\r");
#endif
}

/*
 * ASYNCHRONOUS EVENT HANDLERS -- Start
 */
/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" [WLAN EVENT] NULL Pointer Error \n\r");
#endif
        return;
    }

    switch(pWlanEvent->Event)
    {
    case SL_WLAN_CONNECT_EVENT:
    {
        SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

        /*
         * Information about the connected AP (like name, MAC etc) will be
         * available in 'slWlanConnectAsyncResponse_t' - Applications
         * can use it if required
         *
         * slWlanConnectAsyncResponse_t *pEventData = NULL;
         * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
         *
         */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
        slWlanConnectAsyncResponse_t*  pEventData = NULL;

        CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        /* If the user has initiated 'Disconnect' request, 'reason_code' is
         * SL_USER_INITIATED_DISCONNECTION */
        if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
        {
#ifdef PRINT_VERBOSE
            UARTprintf(" Device disconnected from the AP on application's request \n\r");
#endif
        }
        else
        {
#ifdef PRINT_VERBOSE
            UARTprintf(" Device disconnected from the AP on an ERROR..!! \n\r");
#endif
            gu8APConnectionStatus = DISCONNECTED;
        }
    }
    break;

    default:
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" [WLAN EVENT] Unexpected event \n\r");
#endif
    }
    break;
    }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" [NETAPP EVENT] NULL Pointer Error \n\r");
#endif
        return;
    }

    switch(pNetAppEvent->Event)
    {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
    {
        SET_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

        /*
         * Information about the connection (like IP, gateway address etc)
         * will be available in 'SlIpV4AcquiredAsync_t'
         * Applications can use it if required
         *
         * SlIpV4AcquiredAsync_t *pEventData = NULL;
         * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
         *
         */
    }
    break;

    default:
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" [NETAPP EVENT] Unexpected event \n\r");
#endif
    }
    break;
    }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pHttpEvent - Contains the relevant event information
    \param[in]      pHttpResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
        SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
#ifdef PRINT_VERBOSE
    UARTprintf(" [HTTP EVENT] Unexpected event \n\r");
#endif
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
 */
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
#ifdef PRINT_VERBOSE
    UARTprintf(" [GENERAL EVENT] \n\r");
#endif
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
 */
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" [SOCK EVENT] NULL Pointer Error \n\r");
#endif
        return;
    }

    switch( pSock->Event )
    {
    case SL_SOCKET_TX_FAILED_EVENT:
        /*
         * TX Failed
         *
         * Information about the socket descriptor and status will be
         * available in 'SlSockEventData_t' - Applications can use it if
         * required
         *
         * SlSockEventData_u *pEventData = NULL;
         * pEventData = & pSock->socketAsyncEvent;
         */
        switch( pSock->socketAsyncEvent.SockTxFailData.status )
        {
        case SL_ECLOSE:
#ifdef PRINT_VERBOSE
            UARTprintf(" [SOCK EVENT] Close socket operation, failed to transmit all queued packets\n\r");
#endif
            break;
        default:
#ifdef PRINT_VERBOSE
            UARTprintf(" [SOCK EVENT] Unexpected event \n\r");
#endif
            break;
        }
        break;

        default:
#ifdef PRINT_VERBOSE
            UARTprintf(" [SOCK EVENT] Unexpected event \n\r");
#endif
            break;
    }
}
/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */

//*****************************************************************************
//
//! Initializes Wireless LAN simplelink TCP protocol server
//!
//! \param  None
//!
//! \return None.
//
//*****************************************************************************
_i8 EF_WLAN_Init()
{
    _i32          retVal           = -1;



    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    displayBanner(); /* Displays the banner of UDP connection establishment */

    /*
     * Following function configures the device to default state by cleaning
     * the persistent settings stored in NVMEM (viz. connection profiles &
     * policies, power policy etc)
     *
     * Applications may choose to skip this step if the developer is sure
     * that the device is in its default state at start of application
     *
     * Note that all profiles and persistent settings that were done on the
     * device will be lost
     */
    retVal = configureSimpleLinkToDefaultState();
    if(retVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == retVal)
        {
#ifdef PRINT_VERBOSE
            UARTprintf(" Failed to configure the device in its default state \n\r");
#endif
        }

        //                LOOP_FOREVER();
    }
#ifdef PRINT_VERBOSE
    UARTprintf(" Device is configured in default state \n\r");
#endif
    /*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
    /* Initializing the CC3100 device */
    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) ||
            (ROLE_STA != retVal) )
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" Failed to start the device \n\r");
#endif
        //                LOOP_FOREVER(); /* Cancelled forever loop to avoid  stuck and to give the capability to try to re-establish the connection */
    }
#ifdef PRINT_VERBOSE
    UARTprintf(" Device started as STATION \n\r");
#endif
    /* Connecting to WLAN AP - Set with static parameters defined at the top
            After this call we will be connected and have IP address */
    retVal = establishConnectionWithAP();
    if(retVal < 0)
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" Failed to establish connection w/ an AP \n\r");
#endif
        //                LOOP_FOREVER(); /* Cancelled forever loop to avoid  stuck and to give the capability to try to re-establish the connection */
    }
    else
    {
#ifdef PRINT_VERBOSE
        UARTprintf(" Connection established w/ AP and IP is acquired \n\r");
#endif
        gu8APConnectionStatus = CONNECTED; /* In case the connection to access point was successfull change the status to be CONNECTED */
        /* Disconnection is detected by an interrupt */
    }

#ifdef PRINT_VERBOSE
    UARTprintf(" Creating TCP Server \n\r");
#endif
    retVal = BsdTcpServer(PORT_NUM); /* Establishing a TCP server with port number equals PORT_NUM */
#ifdef PRINT_VERBOSE
    if(retVal < 0)
        UARTprintf(" Failed to create a TCP Server \n\r");
    else
        UARTprintf(" TCP server created and waiting for clients to connect \n\r");
#endif

    return TRUE;
}

//*****************************************************************************
//
//! Transmit Wireless LAN simplelink TCP packet
//!
//! \param  s16SocketNumber - Client socket number to which we will transmit the packet
//!
//! \param  pDataBuffer     - Pointer to the buffer containing the packet payload
//!
//! \param  s16BufferLength - The payload length
//!
//! \return None.
//
//*****************************************************************************
void EF_WLAN_Transmit(_i16 s16SocketNumber, const void *pDataBuffer, _i16 s16BufferLength)
{
    sl_Send(s16SocketNumber, pDataBuffer, s16BufferLength, 0); /* Replace 0 flag by a macro */
}

//*****************************************************************************
//
//! Receives Wireless LAN simplelink TCP packet
//!
//! \param  s16SocketNumber - Client socket number from which we will receive the packet
//!
//! \param  pDataBuffer     - Pointer to the buffer containing the packet payload
//!
//! \param  s16BufferLength - The payload length
//!
//! \return None.
//
//*****************************************************************************
int16_t EF_WLAN_Receive(_i16 s16SocketNumber, void *pDataBuffer, _i16 s16BufferLength)
{
    int16_t s16ReceptionStatus;


    s16ReceptionStatus = sl_Recv(s16SocketNumber, pDataBuffer, s16BufferLength, 0); /* Replace 0 flag by a macro */
    return s16ReceptionStatus;
}

//*****************************************************************************
//
//! Accepts new clients connections
//!
//! \param  s16SocketNumber      - Client socket number which will connect to our server
//!
//! \param  pIP_Address_Accepted - The IP address of the connecting client
//!
//! \param  pIpAddressLength     - IP address length
//!
//! \return None.
//
//*****************************************************************************
_i16 EF_WLAN_Server_Accept_Connection(_i16 s16SocketNumber, SlSockAddr_t *pIP_Address_Accepted, SlSocklen_t *pIpAddressLength)
{
    _i16 s16AcceptedSocket;


    s16AcceptedSocket = sl_Accept(s16SocketNumber, ( struct SlSockAddr_t *)pIP_Address_Accepted, (SlSocklen_t*)pIpAddressLength);
    return s16AcceptedSocket;
}

//*****************************************************************************
//
//! Accepts new clients connections
//!
//! \param  s16SocketNumber - Client socket number of which we will close the connection
//!
//! \return None.
//
//*****************************************************************************
void EF_WLAN_Close_Connection(_i16 s16SocketNumber)
{
    sl_Close(s16SocketNumber);
}



