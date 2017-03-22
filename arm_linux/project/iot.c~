#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "iot_tty.h"
#include "ETClient.h"
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

/*When You want connect linux client in local network, You should enable below*/
#define LINUX_SERVER_MODE_ENABLE

/*When You want show fail message, You should enable below */
//#define PRINT_FAIL_MSG_ENABLE

#define ADDRESS     "115.28.226.129" //"115.28.55.154"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define NAME "424"
#define PASS    "42ff8fca-902a-862532"
#define PORT 8085

#ifdef PRINT_FAIL_MSG_ENABLE
#define PRINTFAIL(format...) printf(format)
#else
#define PRINTFAIL(format...)
#endif // PRINT_FAIL_MSG_ENABLE
TTY_INFO *ptty= NULL;
static int offine_flag = 0;
char buf[9] = {0};//+20170207

void te_connection_lost(void* context,et_server_data *server_data, char* cause)
{
	et_context client = (et_context)context;
	int rc = -1;

	printf("now connect disconnect please check type %d %s\n",server_data->type,server_data->addr);

	if(server_data->type == ET_ILINK_INTERNET_SERVER){
        if(cause && !strcmp(cause, "kickoff")){
            printf("Your are kickoff by server, maybe the same userid login in somewhere.\n");
        }else{
                printf("reconnet ...\n");
            	rc = et_reconnect(client);

                if (rc != ET_SUCCESS)
                {
                    printf("now connect fail\n");
                }
            }
	}



}

int te_message_arrived(void* context,int type,char *send_userid,char* topic_name, int topic_len,et_context_message* message)
{
    int i;
    char* payloadptr = 0;
//    et_context_response_options ropts = {0};
 //   et_context client = (et_context)context;
    if(message) payloadptr = message->payload;

	switch(type)
	{
		case TOPIC_PUBLISH_MESSAGE:
		case CHAT_TO_CHAT_MESSAGE:
        case GROUP_PUBLISH_MESSAGE:
		{
			payloadptr = message->payload;
			memcpy(buf,payloadptr,512);   //+20170207
			//printf("msg:%s\n",buf);
			sendnTTY(ptty,buf,9); 
			for(i=0; i<message->payload_len; i++)
			{
                if(isprint(*payloadptr))
					putchar(*payloadptr++);
                else
                    printf("\\x%2x", (unsigned char)*payloadptr++);
			}
			printf("\n");
		}
		break;

		default:
		{
			printf("send type unkown send id is %s and topic: %s\n",send_userid,topic_name);
		    printf("   message: ");

			payloadptr = message->payload;
			for(i=0; i<message->payload_len; i++)
			{
				putchar(*payloadptr++);
			}
			putchar('\n');
		}
		break;


	}

	et_free_message(&message);
	et_free_memory(topic_name);

    return ET_FREE_MESSAGE_SUCCESS;
}

void message_send_success(void* context, et_context_success_data* response)
{
    if(response->alt.pub.message.payload_len > 0)
    {
        char *msg = calloc(1, response->alt.pub.message.payload_len + 1);
        memcpy(msg, (char *)response->alt.pub.message.payload, response->alt.pub.message.payload_len);
    //��    printf("now send message success: %s\n",msg);
        free(msg);
    }

}

void message_send_fail(void* context,  et_context_failure_data* response)
{
	printf("now fail to connect code is %d and message is %s\n",response->code,response->message);

}

void connect_fail_fun(void* context,  et_context_failure_data* response)
{
	et_context client = (et_context)context;
	int code = response->code;
	printf("connect fail\n");
	printf("now fail to connect code:%d\n", code);

	if(code != ET_ERR_MQTTCONN_INVALIDE_ID &&
       code != ET_ERR_MQTTCONN_BAD_USER_NAME_PWD &&
       code != ET_ERR_MQTTCONN_NOT_AUTHORIZED &&
       code != ET_ERR_SYS_USRTOTAL_LIMIT   /*for docker version server, user total exceed max value, so don't allow new user login*/
    ){
        printf("reconnect start...\n");
        et_reconnect(client);
    }else{
        if(code == ET_ERR_SYS_USRTOTAL_LIMIT) printf("Server of Docker version has reached the pinnacle of online and not allow the new user login\n");

    }

}

static void off_message_success(void* context, et_context_success_data* response)
{
	printf("now off_message_success\n");
}

static void off_message_fail(void* context,  et_context_failure_data* response)
{
	printf("now off_message_fail code is %d and message is %s\n",response->code,response->message);

}

void connect_success_fun(void* context, et_context_success_data* response)
{
	et_context client = (et_context)context;
	et_context_response_options opts = {0};
	int rc;
	//char ret_str[1024] = {0};

	if(offine_flag == 1)
	{
		opts.on_success = off_message_success;
		opts.on_failure = off_message_fail;
		opts.context = client;

		rc = et_request_offline_message(client,&opts);

		if(rc != ET_SUCCESS)
		{
			printf("now et_request_offline_message fail\n");

		}
	}
	printf("connect success\n");
}

void request_success(void* context, et_context_success_data* response)
{
	printf("now id is %d user id is %s state is  %d\n",response->token,response->alt.state.user_id,response->alt.state.user_state);
}

void request_fail(void* context,  et_context_failure_data* response)
{

	printf("now fail to discover code is %d and message is %s\n",response->code,response->message);

}

void sub_state_success(void* context, et_context_success_data* response)
{
	printf("sub_state_success \n");
}

void sub_state_fail(void* context,  et_context_failure_data* response)
{

	printf("sub_state_fail code is %d and message is %s\n",response->code,response->message);

}

void disconnect_success(void* context, et_context_success_data* response)
{
	printf("now disconnect_success \n");
}

void disconnect_fail(void* context,  et_context_failure_data* response)
{
	printf("disconnect_fail fail\n");
}

int te_discover_server(void* context,et_server_data *data)
{
	et_connect_options opts = {0};
	int rc;

	printf("now it is discover\n");

	if(data->type == ET_ILINK_INTERNET_SERVER)
	{
		printf("now is server net mode\n");

		opts.on_success = connect_success_fun;
		opts.on_failure = connect_fail_fun;
		opts.context = context;
		opts.keep_alive_interval = 60;

		opts.cleansession = offine_flag == 1?0:1;
		printf("Connecting\n");
		rc = et_connect(context,data, &opts);
		if (rc != ET_SUCCESS)
		{
			printf("now connect fail\n");
		}
	}
	return 1;
}


int query_success_fun(void* context, void* msg, int len)
{
    if(len){
        char *query_user_msg = malloc(len + 1);
        memcpy(query_user_msg, msg, len);
        query_user_msg[len] = 0;
        printf("Now local client is searching server, the msg is %s\n",query_user_msg);
        free(query_user_msg);
    }

        /*when local server received client search request, You can judge the "msg",
         when You don't allow this client connect, you must return 0, then client can't found Your server
         * or, You need be found by client, you return 1
         */
	return 1;
}

int on_client_connect(void* context,et_server_data *data)
{
	printf("Now connect addr is %s and usrid is %s\n",data->addr,data->userid);
	return 0;

}

void discover_success(void* context, et_context_success_data* response)
{
	printf("now discover_success\n");
}

void discover_fail(void* context,  et_context_failure_data* response)
{
	et_context client = (et_context)context;
	et_context_response_options ropts = {0};
	int rc = 0;

	PRINTFAIL("now  discover_fail\n");

	if(response->alt.discover_fail_type ==  ET_ILINK_INTERNET_SERVER)
	{
		PRINTFAIL("now it is inernet sever fail now retry\n");
		ropts.on_success = discover_success;
		ropts.on_failure = discover_fail;
		ropts.context = client;
		rc = et_discover_internet_svrs(client,60,&ropts);
		if(rc != ET_SUCCESS)
		{
			printf("now discover inner server fail\n");
			return;

		}

	}
}


/***************************************************2017-02-16********************************************************************/
int main(int argc, char* argv[])
{
	et_context client;
	int v = 0,vx = 0;
	char name[128] = "Fc5wGsTuvumomVomAibJDH9jLH2M6mW9hk";
	char *userid[2]={"Fc5wGsTuvumomVomA9G8eu59ZmUKEVFf6q"};

	char server[128] = "lb.kaifakuai.com";
	char appkey[128] = "42ff8fca-902a-862532";
	char sdkversion_str[1024] = {0};
	unsigned char buff[9];	
	et_get_sdk_version(sdkversion_str,1024);

	
	int nbyte;
	

	ptty = readyTTY(0);
	if(ptty == NULL)
	{
		printf("readyTTY(0) error\n");
		return 1;
	}

	lockTTY(ptty);
	if(setTTYSpeed(ptty,115200)>0)
	{
		printf("setTTYSpeed() error\n");
		return -1;
	}
	if(setTTYParity(ptty,8,'N',1)>0)
	{
		printf("setTTYParity() error\n");
		return -1;
	}
	//+testing
	for(v=0;v<1;v++){
		vx = v + 1;
		printf("number: %d userid: %s\n",vx,userid[v]);
	}


	int rc = 0;

	et_context_response_options ropts = {0};
	et_disconnect_options dis_opt = {0};

	et_callback_handles callback = {0};
#ifdef LINUX_SERVER_MODE_ENABLE
	et_context_on_query_options query_data = {0};
#endif
	char str_input[128] = {0};
	char str_cmd[128] = {0};
	//char str_param[128] = {0};
	char str_param_ext[128] = {0};
	char str_param_flag[128] = {0};


#ifdef CLIENT_TRACE_ON
	setenv("MQTT_C_CLIENT_TRACE", "ON", 1);
	setenv("MQTT_C_CLIENT_TRACE_LEVEL", "PROTOCOL", 1);
#endif


	rc = et_create_context(&client,appkey,name,"172.16.2.35",server,PORT,ET_PERSISTENCE_DEFAULT, NULL);

	if(rc != ET_SUCCESS)
	{
        printf("Failed to et_create_context, return code %d\n", rc);
		goto destroy_exit;
    }

	callback.pfn_on_msg = te_message_arrived;
	callback.pfn_broken = te_connection_lost;
	callback.pfn_on_delivery = NULL;
	callback.pfn_on_discover = te_discover_server;
	//setting system callback
	rc = et_set_callback(client, client, &callback);
	if(rc != ET_SUCCESS)
	{
        printf("Failed to et_set_callback, return code %d\n", rc);
		goto destroy_exit;
    }

	ropts.on_success = discover_success;
	ropts.on_failure = discover_fail;
	ropts.context = client;
	//find servers
	rc = et_discover_servers(client,60,&ropts);

	if(rc != ET_SUCCESS)
	{
        printf("Failed to et_discover_servers, return code %d\n", rc);
		goto exit;
    }

#ifdef LINUX_SERVER_MODE_ENABLE
    query_data.context = client;
	query_data.pfn_on_query = query_success_fun;
	query_data.pfn_on_connect = on_client_connect;
	query_data.on_failure = NULL;
	query_data.on_success = NULL;
	//link servers
	rc = et_start_server(client,&query_data);

	if(rc != ET_SUCCESS)
	{
        printf("Failed to et_start_server, return code %d\n", rc);
		goto exit;
    }
#endif
	sleep(1);

	do
        {
        printf("\niot:");
	nbyte = recvnTTY(ptty,buff,8); 
	buff[nbyte] = '\0';
        memset(str_input,0,sizeof(str_input));
     //   fgets(str_input, sizeof(str_input), stdin);
	strcpy(str_input, buff);
	memset(buff, 0, 9);
				memset(str_cmd,0,sizeof(str_cmd));
				sscanf(str_input,"%s",str_cmd);
				sscanf(str_input,"%s%s",str_param_ext,str_param_flag);
		{

			strcpy(str_cmd,"send_msg");
			if(strcmp(str_cmd,"send_msg") == 0)
			{

				et_context_response_options opts = {0};
				int rc;
//				char request_id[128] = {0};
				char message[128] = {0};
                		int snd_msg_len = 0;

				opts.on_success = message_send_success;
				opts.on_failure = message_send_fail;
				opts.context = client;

				if(strcmp(str_param_flag,"-h") != 0)
				{
					if(strlen(str_param_ext) != 0)
					{
						memset(message,0,128);
						strncpy(message,str_param_ext,128);

					}

     				snd_msg_len = strlen(message);
				}

				   for(v=0;v<1;v++){
					rc = et_chat_to(client,userid[v], snd_msg_len, message,0, &opts);
					if(rc != ET_SUCCESS)
					{
						printf("send_msg %s fail\n",userid[v]);//�ӿڵ�1
					}
				}


			}

		}
	} while(1);

exit:
	dis_opt.on_success = disconnect_success;
	dis_opt.on_failure = disconnect_fail;
	dis_opt.context = client;
	et_disconnect(client, &dis_opt);
#ifdef LINUX_SERVER_MODE_ENABLE
	et_stop_server(client);
#endif
destroy_exit:
	et_destroy_context(client);
	return 0;
}
