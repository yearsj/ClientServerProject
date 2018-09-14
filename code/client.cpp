#include "multiplexing.h"
#define MAXLINE     1024
#define IPADDRESS   "127.0.0.1"
#define SERV_PORT   8787

static void handle_connection(int sockfd);

int main(int argc, char *argv[]) {
	int                 sockfd;
	struct sockaddr_in  servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	MyEpoll poll(2, sockfd);
	poll.client_do_multiplexing(); // 处理客户端
	return 0;
}