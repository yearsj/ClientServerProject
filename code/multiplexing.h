#pragma once

#ifndef MULTIPLEXING_H
#define MULTIPLEXING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <unistd.h>
using namespace std;

#define MAXLINE     1024
class Multiplexing {
protected:
	static const int DEFAULT_IO_MAX = 10; //默认的最大文件描述符
	static const int INFTIM = -1;
	int io_max; //记录最大文件描述符
	int listenfd; //监听句柄
public:
	Multiplexing() { this->io_max = DEFAULT_IO_MAX; }
	Multiplexing(int max, int listenfd) { this->io_max = max; this->listenfd = listenfd; }
	~Multiplexing() {}

	virtual void server_do_multiplexing() = 0; //服务端io多路复用
	virtual void client_do_multiplexing() = 0; //客户端io多路复用
	virtual void handle_client_msg() = 0; //处理客户端消息
	virtual bool accept_client_proc() = 0; //接收客户端连接
	virtual bool add_event(int confd, int event) = 0;
	virtual int wait_event() = 0; // 等待事件
};

//-----------------select-------------------------
class MySelect : public Multiplexing {
private:
	fd_set* allfds;      //句柄集合
	int* clifds;   //客户端集合
	int maxfd; //记录句柄的最大值
	int cli_cnt; //客户端个数

public:
	MySelect() : Multiplexing() { allfds = NULL; clifds = NULL; maxfd = 0; cli_cnt = 0; }
	MySelect(int max, int listenfd);
	~MySelect() {
		if (allfds) {
			delete allfds;
			allfds = NULL;
		}
		if (clifds) {
			delete clifds;
			clifds = NULL;
		}
	}

	void server_do_multiplexing();
	void client_do_multiplexing();
	void handle_client_msg();
	bool accept_client_proc();
	bool add_event(int confd, int event);
	bool init_event(); //每次调用select前都要重新设置文件描述符
	int wait_event(); // 等待事件

};

//-----------------poll-------------------------
typedef struct pollfd Pollfd;
class MyPoll : public Multiplexing {
private:
	Pollfd* clientfds; //poll中使用pollfd结构体指定一个被监视的文件描述符
	int max_index; //记录当前clientfds数组中使用的最大下标

public:
	MyPoll() : Multiplexing() { clientfds = NULL; max_index = -1; }
	MyPoll(int max, int listenfd);
	~MyPoll() {
		if (clientfds) {
			delete clientfds;
			clientfds = NULL;
		}
	}

	void server_do_multiplexing();
	void client_do_multiplexing();
	void handle_client_msg();
	bool accept_client_proc();
	bool add_event(int confd, int event);
	int wait_event(); // 等待事件
};

//-----------------epoll-------------------------
typedef struct epoll_event Epoll_event;
class MyEpoll : public Multiplexing {
private:
	int epollfd; //epoll的句柄，用来管理多个文件描述符
	Epoll_event *events; //事件数组
	int nready; //在handle_client_msg函数中用到，传给handle_client_msg函数的当前事件的个数
public:
	MyEpoll() : Multiplexing() { events = NULL; epollfd = -1; }
	MyEpoll(int max, int listenfd);
	~MyEpoll() {
		if (events) {
			delete events;
			events = NULL;
		}
	}

	void server_do_multiplexing();
	void client_do_multiplexing();
	void handle_client_msg();
	bool accept_client_proc();
	bool add_event(int confd, int event);
	bool delete_event(int confd, int event);
	int wait_event(); // 等待事件
};

#endif // !MULTIPLEXING_H