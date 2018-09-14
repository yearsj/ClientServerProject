#include "multiplexing.h"

#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define LISTENQ     5
#define OPEN_MAX    1000

//函数声明
//创建套接字并进行绑定
static int socket_bind(const char* ip, int port);

int main(int argc, char *argv[]) {
	int listenfd = socket_bind(IPADDRESS, PORT);
	if (listenfd < 0) {
		perror("socket bind error");
		return 0;
	}

	listen(listenfd, LISTENQ);

	MyEpoll select(OPEN_MAX, listenfd);
	select.server_do_multiplexing(); //处理服务端
	return 0;
}

static int socket_bind(const char* ip, int port) {
	int  listenfd;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		perror("socket error:");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind error: ");
		exit(1);
	}
	return listenfd;
}