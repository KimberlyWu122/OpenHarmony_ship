/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(MQTTLiteOS_H)
#define MQTTLiteOS_H


#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#if defined(WIN32)
/* default on Windows is 64 - increase to make Linux and Windows the same */
#define FD_SETSIZE 1024
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 256
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINVAL WSAEINVAL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#define ioctl ioctlsocket
#define socklen_t int
#else
#define INVALID_SOCKET SOCKET_ERROR
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "../../../../../third_party/mbedtls/include/mbedtls/net_sockets.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/ssl.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/entropy.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/ctr_drbg.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/error.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/debug.h"
#include "../../../../../third_party/mbedtls/include/mbedtls/platform.h"

#if defined(WIN32)
#include <Iphlpapi.h>
#else
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

#include "lwip/sockets.h"

#define MQTT_TASK

typedef struct Thread
{
	osThreadId_t task;
} Thread;

int ThreadStart(Thread*, void (*fn)(void*), void* arg);

typedef struct Timer
{
	struct timeval end_time;
} Timer;

typedef struct Mutex
{
	osMutexId_t mutex;
} Mutex;

void MqttMutexInit(Mutex*);
int MqttMutexLock(Mutex*);
int MqttMutexUnlock(Mutex*);

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

typedef struct Network
{
	int my_socket;
	mbedtls_ssl_context* ssl_ctx;
    mbedtls_ssl_config* ssl_conf;
    mbedtls_net_context* net_ctx;
    mbedtls_ctr_drbg_context* ctr_drbg;
    mbedtls_entropy_context* entropy;
	int (*mqttread) (struct Network*, unsigned char*, int, int);
	int (*mqttwrite) (struct Network*, unsigned char*, int, int);
	void (*disconnect) (struct Network*);
} Network;

int linux_read(Network*, unsigned char*, int, int);
int linux_write(Network*, unsigned char*, int, int);

 void NetworkInit(Network*);
 int NetworkConnect(Network*, char*, int);
 void NetworkDisconnect(Network*);
 int TLSConnectWithoutVerify(Network* n, char* addr, int port);

#endif
