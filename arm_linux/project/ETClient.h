/*******************************************************************************
 * Copyright (c)
 * Contributors:
 *    Peter - initial API and implementation
 *******************************************************************************/

/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(ETCLIENT_H)
#define ETCLIENT_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport  __attribute__ ((visibility ("default")))
#endif

#include <stdio.h>


/**
 * Return code: No error. Indicates successful completion of an ET client
 * operation.
 */
#define ET_SUCCESS 0
/**
 * Return code: A generic error code indicating the failure of an ET client
 * operation.
 */
#define ET_FAILURE -1

/*free message flag */

#define ET_OK     1


extern int et_err_no; /*When function err, or callback err, you can print this err value, value define like ET_ERR_XXX*/

/* error code -2 is ET_PERSISTENCE_ERROR */

#define ET_PERSISTENCE_ERROR -2

/**
 * ERR code: parameter isn't correct.
 */
#define ET_ERR_PARAMETER -3
/**
 * Return code: The maximum number of messages allowed to be simultaneously
 * in-flight has been reached.
 */
#define ET_MAX_MESSAGES_INFLIGHT -4
/**
 * Return code: An invalid UTF-8 string has been detected.
 */
#define ET_BAD_UTF8_STRING -5
/**
 * Return code: A NULL parameter has been supplied when this is invalid.
 */
#define ET_NULL_PARAMETER -6
/**
 * ERR code: string include some character can't identify
 */
#define ET_ERR_STRING_FORMAT -7
/**
 * Return code: A structure parameter does not have the correct eyecatcher
 * and version number.
 */
#define ET_BAD_STRUCTURE -8
/**
 * ERR code: domain parse fail
 */
#define ET_ERR_DOMAIN_PARSE -9
/**
 * Return code: All 65535 ET msgids are being used
 */
#define ET_NO_MORE_MSGIDS -10

/**
 *  ERR code: get server addr fail
 */
#define ET_ERR_GET_SERVER_ADDR -11

/**
 *  ERR code: the same userid login, so the client is disconnet by system
 */
#define ET_ERR_SAME_USERID_LOGIN -0x1001

#define ET_ERR_MQTTCONN_RESP_UNKOWN -0x1009
/**
 *  ERR code:  like mqtt conack 1-5
 */
#define ET_ERR_MQTTCONN_PROTOCOL_VER  -0x1010
#define ET_ERR_MQTTCONN_INVALIDE_ID   -0x1011
#define ET_ERR_MQTTCONN_INVALIDE_SERVER  -0x1012
#define ET_ERR_MQTTCONN_BAD_USER_NAME_PWD -0x1013
#define ET_ERR_MQTTCONN_NOT_AUTHORIZED    -0x1014

/**
 *  ERR code:  sys exceed user total limit, so can't allow new id login
 */
#define ET_ERR_SYS_USRTOTAL_LIMIT -0x1016


/**
 *   ERR code:  client isn't login
 */
#define ET_ERR_ILINK_NOT_LOGIN -0x1111
/**
 * Default ET version to connect with.  Use 3.1.1 then fall back to 3.1
 */
#define ETVERSION_DEFAULT 0
/**
 * ET version to connect with: 3.1
 */
#define ETVERSION_3_1 3
/**
 * ET version to connect with: 3.1.1
 */
#define ETVERSION_3_1_1 4
/**
 * Bad return code from subscribe, as defined in the 3.1.1 specification
 */
#define ET_BAD_SUBSCRIBE 0x80


#define ET_PERSISTENCE_DEFAULT 0

#define  ET_PERSISTENCE_NONE  1

#define ET_FREE_MESSAGE_SUCCESS  1

#define ET_SRVRESP_NOSUPPORT 6
#define ET_SRVRESP_GROUP_PERMISSION_DENY 8
#define ET_SRVRESP_GROUP_NOEXIST 10


#define ET_MAX_GROUPNAME_STRLEN 64

enum
{
    ET_ILINK_NONE,  /*It is unusable*/
	ET_ILINK_INTERNET_SERVER ,
	ET_ILINK_INNER_SERVER,
};

enum
{
    ET_OPT_BINDTODEVICE = 1,
    ET_OPT_SET_LOCAL_DISCOVER_INFO,
    /*new opt add here*/
    
    ET_OPT_MAX,
 
};

/**
 * A handle representing an ET client. A valid client handle is available
 * following a successful call to et_create_context().
 */
typedef void* et_context;

/*
A structure representing the payload and attributes of an ET data
*/

typedef void* et_item_hanle;//
typedef struct et_node
{
	et_item_hanle handle;//
	struct et_node * next;//

}* et_list;

typedef struct _et_user_info
{
	char username[66];
	char userid[66];
}et_user_info;

typedef struct _et_group_info
{
	char group_name[66];
	char group_id[66];
}et_group_info;

typedef struct _et_video_room_info
{
	char room_name[66];
	char room_id[66];
}et_video_room_info;


typedef struct _et_buddy_info
{
	char nickname[66];
	char username[66];
	char userid[66];
}et_buddy_info;


typedef struct _et_transfer_file_config
{
	char *userid;
	char *local_filename;
	char *file_desc_info;

}et_transfer_file_config;



typedef struct {
	int crc32;
	long file_size;
	char source_ip_addr[32];  //source storage ip address
	char port[8];
	char file_name[256];
	char file_id[128];
	char descn[1024];
	char url[1024];
} et_dfs_file_info;

/**
 * A value representing an ET message. A token is returned to the
 * client application when a message is published. The token can then be used to
 * check that the message was successfully delivered to its destination (see
 * et_context_publishMessage(),
 */
typedef int et_context_token;

/**
 * A structure representing the payload and attributes of an ET message. The
 * message topic is not part of this structure (see et_context_publishMessage(),
 */
typedef struct
{
	/** The eyecatcher for this structure.   */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** The length of the ET message payload in bytes. */
	int payload_len;
	/** A pointer to the payload of the ET message. */
	void* payload;
	/**
     * The quality of service (QoS) assigned to the message.
     */
	int qos;
	/**
     * The retained flag serves two purposes depending on whether the message
     * it is associated with is being published or received.
     */
	int retained;
	/**
      * The dup flag indicates whether or not this message is a duplicate.
      * It is only meaningful when receiving QoS1 messages. When true, the
      * client application should take appropriate action to deal with the
      * duplicate message.
      */
	int dup;
	/** The message identifier is normally reserved for internal use by the
      * ET client and server.
      */
	int msgid;
}et_context_message;

typedef struct _et_server_data
{
	int type;
	int port;
	char addr[20];
	char userid[128];
}et_server_data;

enum
{
	TOPIC_PUBLISH_MESSAGE = 0,
	CHAT_TO_CHAT_MESSAGE,
	FILE__TRANSFERS_MESSAGE,
	USER_ONLINE_MESSAGE,
	USER_OFFLINE_MESSAGE,
	NOTICE_ADD_BUDDY_MESSAGE,
	NOTICE_REMOVE_BUDDY_MESSAGE,
	GROUP_PUBLISH_MESSAGE,
        CHAT_TO_CHAT_JSON_MESSAGE,
        NOTICE_LOGIN_LOCAL_SERVER,
        NOTICE_LOGOUT_LOCAL_SERVER,

};

enum ET_TRACE_LEVELS
{
	ET_TRACE_MAXIMUM = 1,
	ET_TRACE_MEDIUM,
	ET_TRACE_MINIMUM,
	ET_TRACE_PROTOCOL,
	ET_TRACE_ERROR,
	ET_TRACE_SEVERE,
	ET_TRACE_FATAL,
};

/*set log callbak */

typedef void te_trace_callback(enum ET_TRACE_LEVELS level, char* message);



/**
 * This is a callback function. The client application
 * must provide an implementation of this function to enable asynchronous
 * receipt of messages. The function is registered with the client library by
 * passing it as an argument to et_set_callback(). It is
 * called by the client library when a new message that matches a client
 * subscription has been received from the server. This function is executed on
 * a separate thread to the one on which the client application is running.
 * @param context A pointer to the <i>context</i> value originally passed to
 * et_set_callback(), which contains any application-specific context.
 * @param topic_name The topic associated with the received message.
 * @param topic_len The length of the topic if there are one
 * more NULL characters embedded in <i>topic_name</i>, otherwise <i>topic_len</i>
 * is 0. If <i>topic_len</i> is 0, the value returned by <i>strlen(topic_name)</i>
 * can be trusted. If <i>topic_len</i> is greater than 0, the full topic name
 * can be retrieved by accessing <i>topic_name</i> as a byte array of length
 * <i>topic_len</i>.
 * @param message The et_context_message structure for the received message.
 * This structure contains the message payload and attributes.
 * @return This function must return a boolean value indicating whether or not
 * the message has been safely received by the client application. Returning
 * true indicates that the message has been successfully handled.
 * Returning false indicates that there was a problem. In this
 * case, the client library will reinvoke te_on_message() to
 * attempt to deliver the message to the application again.
 */
typedef int te_on_message(void* context, int msg_type,char *send_userid,char* topic_name, int topic_len, et_context_message* message);


/**
 * This is a callback function. The client application
 * must provide an implementation of this function to enable asynchronous
 * notification of delivery of messages to the server. The function is
 * registered with the client library by passing it as an argument to et_set_callback().
 * It is called by the client library after the client application has
 * published a message to the server. It indicates that the necessary
 * handshaking and acknowledgements for the requested quality of service (see
 * et_context_message.qos) have been completed. This function is executed on a
 * separate thread to the one on which the client application is running.
 * @param context A pointer to the <i>context</i> value originally passed to
 * et_set_callback(), which contains any application-specific context.
 * @param token The ::et_context_token associated with
 * the published message. Applications can check that all messages have been
 * correctly published by matching the tokens returned from calls to
 * et_publish() and et_context_sendMessage() with the tokens passed
 * to this callback.
 */
typedef void te_on_delivery(void* context, et_context_token token);

/**
 * This is a callback function. The client application
 * must provide an implementation of this function to enable asynchronous
 * notification of the loss of connection to the server. The function is
 * registered with the client library by passing it as an argument to
 * et_set_callback(). It is called by the client library if the client
 * loses its connection to the server. The client application must take
 * appropriate action, such as trying to reconnect or reporting the problem.
 * This function is executed on a separate thread to the one on which the
 * client application is running.
 * @param context A pointer to the <i>context</i> value originally passed to
 * et_set_callback(), which contains any application-specific context.
 * @param cause The reason for the disconnection.
 * Currently, <i>cause</i> is always set to NULL.
 */
typedef void te_on_broken(void* context, et_server_data *server_data,char* cause);

typedef int te_on_discover(void* context,et_server_data *data);

typedef int te_on_query(void* context,void *msg, int len);

typedef int te_on_client_connect(void* context,et_server_data *data);




typedef struct _et_callback_handles{
	te_on_message *pfn_on_msg;
	te_on_broken *pfn_broken;
	te_on_delivery *pfn_on_delivery;
	te_on_discover  *pfn_on_discover;
	//ON_QUERY pfn_on_query;
} et_callback_handles;



/** The data returned on completion of an unsuccessful API call in the response callback on_failure. */
typedef struct
{
	/** A token identifying the failed request. */
	et_context_token token;
	/** A numeric code identifying the error. */
	int code;
	/** Optional text explaining the error. Can be NULL. */
	char* message;

	union /* A union of the different values for server or file*/
	{
		/* server data struct*/
		et_server_data server_data;
		char *file_name;
		int discover_fail_type;
	}alt;

} et_context_failure_data;

/** The data returned on completion of a successful API call in the response callback on_success. */
typedef struct
{
	/** A token identifying the successful request. Can be used to refer to the request later. */
	et_context_token token;
	/** A union of the different values that can be returned for subscribe, upload file, down file ,unsubscribe and publish. */
	union
	{
		/** For subscribe, the granted QoS of the subscription returned by the server.
		*/
		int qos;
		/** For subscribeMany, the list of granted QoSs of the subscriptions returned by the server. */
		int* qos_list;
		/* for send file success*/
		struct
		{
			int crc32;
			long file_size;
			char *source_ip_addr;  //source storage ip address
			char *port;
			char *file_name;
			char *file_id;
		}file_info;
		/** For publish, upload file, down file,the message being sent to the server. */
		struct
		{
			et_context_message message;
			char* destination_name;
		} pub;
		/* For connect, the server connected to, ET version used, and session_present flag */
		struct
		{
			char* server_uri;
			int et_version;
			int session_present;
		} connect;
		/*for request user state*/
		struct
		{
			int user_state;
			char *user_id;
			int user_id_len;
		}state;
		/*for time request*/
		struct
		{
			char *time_str;
			int time_len;
		}time;

		/*for list example get buddies*/
		et_list list;
	} alt;
} et_context_success_data;

/**
 * This is a callback function. The client application
 * must provide an implementation of this function to enable asynchronous
 * notification of the successful completion of an API call. The function is
 * registered with the client library by passing it as an argument in
 * ::et_context_response_options.
 * @param context A pointer to the <i>context</i> value originally passed to
 * ::et_context_response_options, which contains any application-specific context.
 * @param response Any success data associated with the API completion.
 */
typedef void et_context_on_success(void* context, et_context_success_data* response);

/**
 * This is a callback function. The client application
 * must provide an implementation of this function to enable asynchronous
 * notification of the unsuccessful completion of an API call. The function is
 * registered with the client library by passing it as an argument in
 * ::et_context_response_options.
 * @param context A pointer to the <i>context</i> value originally passed to
 * ::et_context_response_options, which contains any application-specific context.
 * @param response Any failure data associated with the API completion.
 */
typedef void et_context_on_failure(void* context,  et_context_failure_data* response);

typedef struct
{
	/**
    * A pointer to a callback function to be called if the API call successfully
    * completes.  Can be set to NULL, in which case no indication of successful
    * completion will be received.
    */
	et_context_on_success* on_success;
	/**
    * A pointer to a callback function to be called if the API call fails.
    * Can be set to NULL, in which case no indication of unsuccessful
    * completion will be received.
    */
	et_context_on_failure* on_failure;
	/**
	* A pointer to any application-specific context. The
    * the <i>context</i> pointer is passed to success or failure callback functions to
    * provide access to the context information in the callback.
    */
	void* context;
	et_context_token token;   /* output */
} et_context_response_options;


typedef struct
{
	/**
    * A pointer to a callback function to be called if the API call successfully
    * completes.  Can be set to NULL, in which case no indication of successful
    * completion will be received.
    */
	et_context_on_success* on_success;
	/**
    * A pointer to a callback function to be called if the API call fails.
    * Can be set to NULL, in which case no indication of unsuccessful
    * completion will be received.
    */
	et_context_on_failure* on_failure;
	/**
    * A pointer to a callback function to be called if the on query
    */
	te_on_query* pfn_on_query;
	/**
    * A pointer to a callback function to be called if a client connect
    */
	te_on_client_connect *pfn_on_connect;
	/**
	* A pointer to any application-specific context. The
    * the <i>context</i> pointer is passed to success or failure callback functions to
    * provide access to the context information in the callback.
    */
	void* context;
	et_context_token token;
} et_context_on_query_options;


/**
 * This function sets the global callback functions for a specific client.
 * If your client application doesn't use a particular callback, set the
 * relevant parameter to NULL. Any necessary message acknowledgements and
 * status communications are handled in the background without any intervention
 * from the client application.  If you do not set a messageArrived callback
 * function, you will not be notified of the receipt of any messages as a
 * result of a subscription.
 *
 * <b>Note:</b> The ET client must be disconnected when this function is
 * called.
 * @param handle A valid client handle from a successful call to
 * et_create_context().
 * @param context A pointer to any application-specific context. The
 * the <i>context</i> pointer is passed to each of the callback functions to
 * provide access to the context information in the callback.
 * @param cl A pointer to an te_on_broken() callback
 * function. You can set this to NULL if your application doesn't handle
 * disconnections.
 * @param ma A pointer to an te_on_message() callback
 * function.  You can set this to NULL if your application doesn't handle
 * receipt of messages.
 * @param dc A pointer to an te_on_delivery() callback
 * function. You can set this to NULL if you do not want to check
 * for successful delivery.
 * @return ::et_context_SUCCESS if the callbacks were correctly set,
 * ::et_context_FAILURE if an error occurred.
 */
DLLExport int et_set_callback(et_context handle, void* context, et_callback_handles *callback);


/**
 * This function creates an ET client ready for connection to the
 * specified server and using the specified persistent storage (see
 * et_context_persistence). See also et_destroy_context().
 * @param handle A pointer to an ::et_context handle. The handle is
 * populated with a valid client reference following a successful return from
 * this function.
 * @param server_uri A null-terminated string specifying the server to
 * which the client will connect. It takes the form <i>192.168.1.125</i>.


 * @param persistence_type The type of persistence to be used by the client:
 * <br>
 * ::ET_PERSISTENCE_NONE: Use in-memory persistence. If the device or
 * system on which the client is running fails or is switched off, the current
 * state of any in-flight messages is lost and some messages may not be
 * delivered even at QoS1 and QoS2.
 * <br>
 * ::ET_PERSISTENCE_DEFAULT: Use the default (file system-based)
 * persistence mechanism. Status about in-flight messages is held in persistent
 * storage and provides some protection against message loss in the case of
 * unexpected failure.
 * <br>
 * ::ET_PERSISTENCE_USER: Use an application-specific persistence
 * implementation. Using this type of persistence gives control of the
 * persistence mechanism to the application. The application has to implement
 * the et_context_persistence interface.
 * @param persistence_context If the application uses
 * ::ET_PERSISTENCE_NONE persistence, this argument is unused and should
 * be set to NULL. For ::ET_PERSISTENCE_DEFAULT persistence, it
 * should be set to the location of the persistence directory (if set
 * to NULL, the persistence directory used is the working directory).
 * Applications that use ::ET_PERSISTENCE_USER persistence set this
 * argument to point to a valid et_context_persistence structure.
 * @return ::et_context_SUCCESS if the client is successfully created, otherwise
 * an error code is returned.
 */
DLLExport int et_create_context(et_context* handle,char *app_key,char *userid,char *local_ip, const char* server_uri,int server_port,int persistence_type, void* persistence_context);

DLLExport int et_set_option(et_context handle, int option, void *value, int len);

DLLExport int et_discover_servers(et_context handle,int time_out,et_context_response_options* response);

DLLExport int et_discover_internet_svrs(et_context handle,int time_out,et_context_response_options* response);

DLLExport int et_discover_inner_svrs(et_context handle,int time_out,et_context_response_options* response);


/**
 * et_connect_options defines several settings that control the way the
 * client connects to an ET server.  Default values are set in
 */
typedef struct
{
	/** The "keep alive" interval, measured in seconds, defines the maximum time
      * that should pass without communication between the client and the server
      * The client will ensure that at least one message travels across the
      * network within each keep alive period.  In the absence of a data-related
	  * message during the time period, the client sends a very small ET
      * "ping" message, which the server will acknowledge. The keep alive
      * interval enables the client to detect when the server is no longer
	  * available without having to wait for the long TCP/IP timeout.
	  * Set to 0 if you do not want any keep alive processing.
	  */
	int keep_alive_interval;
	/**
      * This is a boolean value. The cleansession setting controls the behaviour
      * of both the client and the server at connection and disconnection time.
      * The client and server both maintain session state information. This
      * information is used to ensure "at least once" and "exactly once"
      * delivery, and "exactly once" receipt of messages. Session state also
      * includes subscriptions created by an ET client. You can choose to
      * maintain or discard state information between sessions.
      *
      * When cleansession is true, the state information is discarded at
      * connect and disconnect. Setting cleansession to false keeps the state
      * information. When you connect an ET client application with
      * et_connect(), the client identifies the connection using the
      * client identifier and the address of the server. The server checks
      * whether session information for this client
      * has been saved from a previous connection to the server. If a previous
      * session still exists, and cleansession=true, then the previous session
      * information at the client and server is cleared. If cleansession=false,
      * the previous session is resumed. If no previous session exists, a new
      * session is started.
	  */
	int cleansession;
	/**
      * This controls how many messages can be in-flight simultaneously.
	  */
	int max_inflight;

	/**
      * ET servers that support the ET v3.1 protocol provide authentication
      * and authorisation by user name and password. This is the user name
      * parameter.
      */
	//const char* username;
	/**
      * ET servers that support the ET v3.1 protocol provide authentication
      * and authorisation by user name and password. This is the password
      * parameter.
      */
	//const char* password;
	/**
      * The time interval in seconds to allow a connect to complete.
      */
	int connect_timeout;
	/**
	 * The time interval in seconds
	 */
	int retry_interval;

	/**
      * A pointer to a callback function to be called if the connect successfully
      * completes.  Can be set to NULL, in which case no indication of successful
      * completion will be received.
      */
	et_context_on_success* on_success;
	/**
      * A pointer to a callback function to be called if the connect fails.
      * Can be set to NULL, in which case no indication of unsuccessful
      * completion will be received.
      */
	et_context_on_failure* on_failure;
	/**
	  * A pointer to any application-specific context. The
      * the <i>context</i> pointer is passed to success or failure callback functions to
      * provide access to the context information in the callback.
      */
	void* context;
	/**
	  * The number of entries in the server_uris array.
	  */
	//int server_uri_count;
	/**
	  * An array of null-terminated strings specifying the servers to
      * which the client will connect. Each string takes the form <i>protocol://host:port</i>.
      * <i>protocol</i> must be <i>tcp</i> or <i>ssl</i>. For <i>host</i>, you can
      * specify either an IP address or a domain name. For instance, to connect to
      * a server running on the local machines with the default ET port, specify
      * <i>tcp://localhost:1883</i>.
      */
	//char* const* server_uris;
	//et_server_data server_uri;
	//int et_context_version;
} et_connect_options;

/**
  * This function attempts to connect a previously-created client (see
  * et_create_context()) to an ET server using the specified options. If you
  * want to enable asynchronous message and status notifications, you must call
  * et_set_callback() prior to et_connect().
  * Is is used first connect server When we discover it, if you attempts reconnect, please use "et_reconnect"
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param options A pointer to a valid et_connect_options
  * structure.
  * @return ::et_context_SUCCESS if the client connect request was accepted.
  * If the client was unable to connect to the server, an error code is
  * returned via the on_failure callback, if set.
  * Error codes greater than 0 are returned by the ET protocol:<br><br>
  * <b>1</b>: Connection refused: Unacceptable protocol version<br>
  * <b>2</b>: Connection refused: Identifier rejected<br>
  * <b>3</b>: Connection refused: Server unavailable<br>
  * <b>4</b>: Connection refused: Bad user name or password<br>
  * <b>5</b>: Connection refused: Not authorized<br>
  * <b>6-255</b>: Reserved for future use<br>
  */
DLLExport int et_connect(et_context handle, et_server_data *server_info,const et_connect_options* options);


/**
  * This function used when conncet failed, must retry connect
  * because it support safety connect in callback, or other thread, so must call it, and don't use "et_connect"
  */
DLLExport int et_reconnect(et_context handle);


typedef struct
{
	/**
      * The client delays disconnection for up to this time (in
      * milliseconds) in order to allow in-flight message transfers to complete.
      */
	int timeout;
	/**
    * A pointer to a callback function to be called if the disconnect successfully
    * completes.  Can be set to NULL, in which case no indication of successful
    * completion will be received.
    */
	et_context_on_success* on_success;
	/**
    * A pointer to a callback function to be called if the disconnect fails.
    * Can be set to NULL, in which case no indication of unsuccessful
    * completion will be received.
    */
	et_context_on_failure* on_failure;
	/**
	* A pointer to any application-specific context. The
    * the <i>context</i> pointer is passed to success or failure callback functions to
    * provide access to the context information in the callback.
    */
	void* context;
} et_disconnect_options;

/**
  * This function attempts to disconnect the client from the ET
  * server. In order to allow the client time to complete handling of messages
  * that are in-flight when this function is called, a timeout period is
  * specified. When the timeout period has expired, the client disconnects even
  * if there are still outstanding message acknowledgements.
  * The next time the client connects to the same server, any QoS 1 or 2
  * messages which have not completed will be retried depending on the
  * cleansession settings for both the previous and the new connection (see
  * et_connect_options.cleansession and et_connect()).
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param options The client delays disconnection for up to this time (in
  * milliseconds) in order to allow in-flight message transfers to complete.
  * @return ::et_context_SUCCESS if the client successfully disconnects from
  * the server. An error code is returned if the client was unable to disconnect
  * from the server
  */
DLLExport int et_disconnect(et_context handle, const et_disconnect_options* options);


/**
  * This function allows the client application to test whether or not a
  * client is currently connected to the ET server.
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @return Boolean true if the client is connected, otherwise false.
  */
DLLExport int et_connect_state(et_context handle);


/**
  * This function attempts to subscribe a client to a single topic, which may
  * contain wildcards (see @ref wildcard). This call also specifies the
  * @ref qos requested for the subscription
  * (see also et_subscribe_many()).
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param topic The subscription topic, which may include wildcards.
  * @param qos The requested quality of service for the subscription.
  * @param response A pointer to a response options structure. Used to set callback functions.
  * @return ::et_context_SUCCESS if the subscription request is successful.
  * An error code is returned if there was a problem registering the
  * subscription.
  */
DLLExport int et_subscribe(et_context handle, const char* topic, int qos, et_context_response_options* response);


/**
  * This function attempts to subscribe a client to a list of topics, which may
  * contain wildcards (see @ref wildcard). This call also specifies the
  * @ref qos requested for each topic (see also et_subscribe()).
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param count The number of topics for which the client is requesting
  * subscriptions.
  * @param topic An array (of length <i>count</i>) of pointers to
  * topics, each of which may include wildcards.
  * @param qos An array (of length <i>count</i>) of @ref qos
  * values. qos[n] is the requested QoS for topic[n].
  * @param response A pointer to a response options structure. Used to set callback functions.
  * @return ::et_context_SUCCESS if the subscription request is successful.
  * An error code is returned if there was a problem registering the
  * subscriptions.
  */
DLLExport int et_subscribe_many(et_context handle, int count, char* const* topic, int* qos, et_context_response_options* response);

/**
  * This function attempts to remove an existing subscription made by the
  * specified client.
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param topic The topic for the subscription to be removed, which may
  * include wildcards (see @ref wildcard).
  * @param response A pointer to a response options structure. Used to set callback functions.
  * @return ::et_context_SUCCESS if the subscription is removed.
  * An error code is returned if there was a problem removing the
  * subscription.
  */
DLLExport int et_unsubscribe(et_context handle, const char* topic, et_context_response_options* response);

/**
  * This function attempts to remove existing subscriptions to a list of topics
  * made by the specified client.
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param count The number subscriptions to be removed.
  * @param topic An array (of length <i>count</i>) of pointers to the topics of
  * the subscriptions to be removed, each of which may include wildcards.
  * @param response A pointer to a response options structure. Used to set callback functions.
  * @return ::et_context_SUCCESS if the subscriptions are removed.
  * An error code is returned if there was a problem removing the subscriptions.
  */
DLLExport int et_unsubscribe_many(et_context handle, int count, char* const* topic, et_context_response_options* response);


/**
  * This function attempts to publish a message to a given topic . An ::et_context_token is issued when
  * this function returns successfully. If the client application needs to
  * test for successful delivery of messages, a callback should be set
  * (see ::et_context_on_success() and ::te_on_delivery()).
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param destination_name The topic associated with this message.
  * @param payload_len The length of the payload in bytes.
  * @param payload A pointer to the byte array payload of the message.
  * @param qos The @ref qos of the message.
  * @param retained The retained flag for the message.
  * @param response A pointer to an ::et_context_response_options structure. Used to set callback functions.
  * This is optional and can be set to NULL.
  * @return ::ET_SUCCESS if the message is accepted for publication.
  * An error code is returned if there was a problem accepting the message.
  */
DLLExport int et_publish(et_context handle, const char* destination_name, int payload_len, void* payload, int qos, int retained, et_context_response_options* response);

/**
  * This function attempts to publish a message to a user. An ::et_context_token is issued when
  * this function returns successfully. If the client application needs to
  * test for successful delivery of messages, a callback should be set
  * (see ::et_context_on_success() and ::te_on_delivery()).
  * @param handle A valid client handle from a successful call to
  * et_create_context().
  * @param userid send message to user id.
  * @param payload_len The length of the payload in bytes.
  * @param payload A pointer to the byte array payload of the message.
  * @param response A pointer to an ::et_context_response_options structure. Used to set callback functions.
  * This is optional and can be set to NULL.
  * @return ::ET_SUCCESS if the message is accepted for publication.
  * An error code is returned if there was a problem accepting the message.
  */

DLLExport int et_chat_to(et_context handle, const char* userid, int payload_len, void* payload, int retained,et_context_response_options* response);


/*   This function send json format message to this userid.
 *   @param payload_json, json format string end with null.
 *      when you send offline message to user on iphone/ios with APN , your json must include "nickname" and "content"
 */
DLLExport int et_chat_to_json(et_context handle, const char* userid, char* payload_json, int retained,et_context_response_options* response);

/**
  * This function frees the memory allocated to an MT client (see
  * et_create_context()). It should be called when the client is no longer
  * required.
  * @param handle A pointer to the handle referring to the ::et_context
  * structure to be freed.
  */
DLLExport void et_destroy_context(et_context handle);

/**
  * This function frees memory allocated to an ET message, including the
  * additional memory allocated to the message payload. The client application
  * calls this function when the message has been fully processed. <b>Important
  * note:</b> This function does not free the memory allocated to a message
  * topic string. It is the responsibility of the client application to free
  * this memory using the et_free_memory() library function.
  * @param msg The address of a pointer to the ::ETAsync_message structure
  * to be freed.
  */
DLLExport void et_free_message(et_context_message** msg);

/**
  * This function frees memory allocated by the ET C client library, especially the
  * topic name. This is needed on Windows when the client libary and application
  * program have been compiled with different versions of the C compiler.  It is
  * thus good policy to always use this function when freeing any ET C client-
  * allocated memory.
  * @param ptr The pointer to the client library storage to be freed.
  */
DLLExport void et_free_memory(void* ptr);

/**
  * This function get ip address from loadbalancer
  */

DLLExport int et_get_address(char *app_key,char *userid,const char *server_addr,int server_port,char *addr,char *port);

/**
  * This function get a user on-line or off-line state
  */

DLLExport int et_get_user_state(et_context handle,char *userid,et_context_response_options* response);

/**
  * This function get system time
  */

DLLExport int et_get_ilink_time(et_context handle,et_context_response_options* response);

/* this function request offline message*/

DLLExport int et_request_offline_message(et_context handle,et_context_response_options* response);


/* this function subscribe user on or off state*/

DLLExport int et_sub_user_state(et_context handle,char *userid,et_context_response_options* response);

/* this function unsubscribe user on or off state*/

DLLExport int et_unsub_user_state(et_context handle,char *userid,et_context_response_options* response);

/* this function get online buddies*/

DLLExport int et_online_buddies(et_context handle,et_context_response_options* response);


/**
  * This function add a friend to user
  */

DLLExport int et_add_buddy(et_context handle,char *secret_key,char *userid,int flag);
/**
  * This function delete a friend to user
  */

DLLExport int et_remove_buddy(et_context handle,char *secret_key,char *userid,int flag);

/**
  * This function from list get buddy info
  */

DLLExport int et_list_buddy_info(et_context handle,et_list list,int pos,et_buddy_info** buddy_info);

/**
  * This function get  friend lists user
  */

DLLExport int et_get_buddies(et_context handle,char *secret_key,et_list list);

/**
  * This function subcribe buddies status
  */


DLLExport int et_sub_buddies_state(et_context handle,et_context_response_options* response);


/**
  * This function unsubcribe buddies status
  */


DLLExport int et_unsub_buddies_state(et_context handle,et_context_response_options* response);


/**
  * This function create group
  */

DLLExport int et_create_group(et_context handle,char *secret_key,char *group_name,const char **group_userid,int group_size,et_group_info *info);


/**
  * This function send group msg
  */
DLLExport int et_publish_to_group(et_context handle, const char* group_id, int payload_len, void* payload, int qos, int retained,et_context_response_options* response);

/**
  * This function from list get group info
  */

DLLExport int et_list_group_info(et_context handle,et_list list,int pos,et_group_info** group_info);

/**
  * This function logout group
  */

DLLExport int et_exit_group(et_context handle,char *secret_key,char *groupid);


/**
  * This function get group lists
  */

DLLExport int et_get_groups(et_context handle,char *secret_key,et_list list);


/**
  * This function delete group
  */

DLLExport int et_destroy_group(et_context handle,char *secret_key,char *groupid);

/**
  * This function add a user to group
  */

DLLExport int et_add_group_members(et_context handle,char *secret_key,char *groupid,const char **group_userid,int group_size);

/**
  * This function delete a user from group
  */

DLLExport int et_remove_group_members(et_context handle,char *secret_key,char *groupid,const char **group_userid,int group_size);


/**
  * This function get user list from group
  */

DLLExport int et_get_group_members(et_context handle,char *secret_key,char *groupid,et_list list);
/**
  * This function get sdk version
  */

DLLExport int et_get_sdk_version(char *ret_str,int size);

/**
  * This function create ET data list
  */


DLLExport int et_create_list(et_context handle,et_list *list);

/**
  * This function add username to ET list
  */

DLLExport int et_add_list_by_user_name(et_context handle,et_list list,char *username);

/**
  * This function get ET list size
  */

DLLExport int et_list_size(et_context handle,et_list list);

/**
  * This function destroy ET list
  */

DLLExport int et_destroy_list(et_context handle,et_list *list);

/**
  * This function destroy ET user info
  */

DLLExport int et_list_user_info(et_context handle,et_list list,int pos,et_user_info** user_info);

DLLExport int et_file_to(et_context handle,et_transfer_file_config *file_config,et_context_response_options* response);

DLLExport int et_download_file(et_context handle,et_dfs_file_info *file_info,char *save_filename,et_context_response_options* response);

DLLExport int et_file_info(et_context handle,char *file_str,et_dfs_file_info *file_info);

DLLExport int et_start_server(et_context handle,et_context_on_query_options *query_data);

DLLExport int et_stop_server(et_context handle);

DLLExport void et_set_trace_callbackes(et_context handle,te_trace_callback *callback);


#endif

#ifdef __cplusplus
     }
#endif
