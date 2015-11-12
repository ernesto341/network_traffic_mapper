#ifndef _CAPTURE_H_
#define _CAPTURE_H_
#pragma once

#include <cstring>
#include <queue>
#include <signal.h>
#include <pthread.h>

#include <itoa.h>
#include <line.h>
#include <iptocoord.h>

#ifndef __FAVOR_BSD
# define __FAVOR_BSD
#endif

#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <pcap.h>

using namespace std;

#ifndef MAX_THREADS
#define MAX_THREADS 5
#endif

struct threadStuff
{
	queue<Line> live_data;
	pthread_mutex_t live_data_mutex;
	pthread_t tid;
	char * dev;
};

extern volatile sig_atomic_t done;
extern pthread_mutex_t lines_mutex;
extern threadStuff threadArgs[MAX_THREADS];

int fib(int);
void * simulateCapture(void *);
void * capture(void *);

#endif
