#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "proto.h"

#define IP_SIZE 16
#define PRONUM 4

int worker_loop(int sd){
	char str[IP_SIZE]={'\0'};
	int len;
	int newsd;
	struct sockaddr_in raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	rlen = sizeof(raddr);

	while (1) {
		newsd = accept(sd, (void *) &raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}

		printf("pid:[%d]radd:%s rport:%d\n", \
				getpid(), ip, htons(raddr.sin_port));

		len = sprintf(str, FMT_STAMP, (long long)time(NULL));
		if(send(newsd, str, len, 0) < 0){
			perror("send()");
			exit(-3);
		}
		close(newsd);
	}
	close(sd);

}

//4个进程随机并发
//for i in {1..100}; do { ./client 127.0.0.1; }& done
int main(){
	int sd, newsd, i;
	struct sockaddr_in laddr;
	pid_t pid;
	sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);
	if(sd < 0){
		perror("socket");
		exit(0);
	}

	int val=1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){
		perror("setsockopt()");
		exit(0);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	if(inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(0);
	}

	listen(sd, 200);
	for(i=0; i<PRONUM; i++){
		pid = fork();
		if(pid == 0){
			worker_loop(sd);
			exit(0);
		}
	}

	for(i=0; i<PRONUM; i++){
		wait(NULL);
	}

	exit(0);
}
