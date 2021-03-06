#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <proto.h>

#include "medialib.h"
#include "server.h"


int sd;
struct sockaddr_in raddr;
pthread_t tid[NR_CHN+1];

struct server_conf_st server_conf = {\
		.mgroup = DEFAULT_MGROUP,\
		.rcvport = DEFAULT_RCVPORT,\
		.medpath = DEFAULT_PATH};
/*
 *  -M      指定多播组
 *  -P      指定收接端口
 *  -C		指定测试频道
 *  -F		前台调试运行
 *  -H      显示帮助
 * */

static int daemonize(void)
{
	int fd;
	pid_t pid;

	pid = fork();
	if(pid < 0) {
		syslog(LOG_ERR, "fork():%s", strerror(errno));
		return -1;
	}else if(pid > 0) 
		exit(0);

	fd = open("/dev/null", O_RDWR);
	if(fd < 0) {
		syslog(LOG_WARNING, "open():%s", strerror(errno));
	}else {
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);
		if(fd > 2)
			close(fd);
	}
	setsid();
	chdir("/");
	umask(0);

	return 0;
}


int main(int argc, char **argv)
{
	struct mlib_chn_st *listptr;
	int listsize;
	int i, err;
	char buf[BUFSIZE];
	int len, ch;
	struct ip_mreqn req;

	/*conf处理*/
	while(1) {
		ch = getopt(argc, argv, "M:P:C:");
		if(ch < 0) {
			break;
		}
		switch(ch) {
			case 'M':
				server_conf.mgroup = optarg;
				break;
			case 'P':
				server_conf.rcvport = optarg;
				break;
			case 'C':
				server_conf.medpath = optarg;
			default:
				break;
		}
	}

	openlog("server", LOG_PID, LOG_DAEMON);
	//if(daemonize()) {
	//	syslog(LOG_ERR, "daemonize() is failed");
	//	exit(1);
	//}else
	//	syslog(LOG_INFO, "daemonize() is successed");

	/*socket init*/
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0) {
		perror("socket()");
		exit(1);
	}

	inet_pton(AF_INET, server_conf.mgroup, &req.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &req.imr_address);
	req.imr_ifindex = if_nametoindex("eth0");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &req, sizeof(req)) < 0) {
		perror("setsockopt()");
		exit(1);
	}

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(server_conf.rcvport));
	inet_pton(AF_INET, server_conf.mgroup, &raddr.sin_addr);

	/*获取频道列表信息（从medialib中）*/
	err = mlib_getchnlist(&listptr,&listsize);		
	for(i = 0; i < listsize; i++) {
		printf("%d: ", listptr[i].chnid);
		puts(listptr[i].desc);
	}
/*	if(err)
	{
		syslog();
	}*/

	/*创建thr_list*/
	err = thr_list_create(listptr, listsize);
	if(err < 0)
	{
		fprintf(stderr, "thr_list_create() is error\n");
		exit(1);
	}

	/*创建thr_channel*/
	/*1:200  100:200  4:200  200:200*/ 

	for(i = 1; i <= listsize; i++) {
		thr_channel_create(tid+i, (chnid_t)i);   //id会存在竞争
	}

	while(1)
		pause();
	
	thr_channel_destroyall(listsize);
	thr_list_destroy();
	mlib_freechnlist(&listptr, listsize);
	closelog();

	exit(0);
}

