#include <stdio.h>
#include <winsock2.h>
#include <WinBase.h>
#include <Winerror.h>
#include "KendyNet.h"
#include "Connection.h"
#include <stdint.h>

uint32_t packet_recv = 0;
uint32_t packet_send = 0;
uint32_t send_request = 0;
uint32_t tick = 0;
uint32_t now = 0;
uint32_t s_p = 0;
uint32_t bf_count = 0;
uint32_t clientcount = 0;
uint32_t last_send_tick = 0;

#define MAX_CLIENT 500
static struct connection *clients[MAX_CLIENT];

void init_clients()
{
	int i = 0;
	for(; i < MAX_CLIENT;++i)
		clients[i] = 0;
}

void add_client(struct connection *c)
{
	uint32_t i = 0;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i] == 0)
		{
			clients[i] = c;
			break;
		}
	}
}

void send2_all_client(rpacket_t r)
{
	uint32_t i = 0;
	wpacket_t w;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i])
		{
			w = wpacket_create_by_rpacket(r);
			++send_request;
			//connection_send(clients[i],w,0);
			connection_push_packet(clients[i],w);
		}
	}
}

void remove_client(struct connection *c)
{
	uint32_t i = 0;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i] == c)
		{
			clients[i] = 0;
			break;
		}
	}
}

void on_process_packet(struct connection *c,rpacket_t r)
{
	//uint32_t i = 0;
	send2_all_client(r);
	
	//wpacket_t w = wpacket_create_by_rpacket(r);
	//connection_send(c,w,0);
	rpacket_destroy(&r);
	++packet_recv;	
}

void accept_callback(SOCKET s,void *ud)
{
	DWORD err_code = 0;
	HANDLE *iocp = (HANDLE*)ud;
	struct connection *c = connection_create(s,0,on_process_packet,remove_client);
	add_client(c);
	//++clientcount;
	printf("cli fd:%d\n",s);
	Bind2Engine(*iocp,(Socket_t)c);
	//������һ��������
	connection_recv(c);
}
const char *ip;
uint32_t port;

DWORD WINAPI Listen(void *arg)
{
	acceptor_t a = create_acceptor(ip,port,&accept_callback,arg);
	while(1)
		acceptor_run(a,100);
	return 0;
}
unsigned long iocp_count = 0; 
unsigned long use_time = 0;
int32_t main(int argc,char **argv)
{
	DWORD dwThread;
	HANDLE iocp;
	uint32_t n;
	ip = argv[1];
	port = atoi(argv[2]);
	uint32_t i = 0;
	//getchar();
	init_wpacket_pool(1000000);
	init_rpacket_pool(500000);
	init_clients();
	InitNetSystem();
	iocp = CreateNetEngine(1);

	CreateThread(NULL,0,Listen,&iocp,0,&dwThread);
	tick = GetTickCount();
	while(1)
	{
		uint32_t t = GetTickCount();
		RunEngine(iocp,15);
		t = GetTickCount()-t;
		use_time = (use_time+t)/2;		
		now = GetTickCount();
		if(now - tick > 1000)
		{
			printf("recv:%u,send:%u,s_req:%u,pool_size:%u,bf:%u,sp:%u,iocp:%u\n",packet_recv,packet_send,send_request,wpacket_pool_size(),use_time,s_p,iocp_count);
			tick = now;
			packet_recv = 0;
			packet_send = 0;
			send_request = 0;
			//s_p = 0;
			//iocp_count = 0;
			use_time = 0;
		}
		if(now - last_send_tick > 25)
		{
			//����,ÿ50ms���з�һ�ΰ�
			last_send_tick = now;
			for(i=0; i < MAX_CLIENT; ++i)
			{
				if(clients[i])
				{
					++send_request;
					connection_send(clients[i],0,0);
				}
			}
		}
	}
	return 0;
}