#ifndef TIMERPOLL__H_
#define TIMERPOLL__H_

#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <pthread.h>
#include <map>
#include <memory>
#include <thread>
#include <iostream>
#include "timer.h"

#define MAXFDS 128

class timers {
    public:
        /**
         *生成定时器对象
         */
        timers();

        /**
         *初始化定时器
         */
        void Init(int max_num = MAXFDS);

        /**
         *增加一个定时任务
         */
        template<typename T>
        int AddTimer(double internalvalue, T callback, void *data, int rep);
        template<typename T>
        int AddTimer(double internalvalue, T callback, void* frame, void* data, int rep);
        
        /**
         *删除定时任务
         */
        int DelTimer(timer* ptimer);
        
        /**
         *定时器开始运行
         */
        int Run();

        /**
         *关闭定时器
         */
        int Stop();

        /**
         *定时器死亡了
         */
        ~timers(){};

        /**
         *当前存在的定时任务个数
         */
        int Size();
    public:

    private:
        /**
         *epoll定时器fd
         */
        int mepollfd;
        
        /**
         *定时器是否有效
         */
        int mactive;
        
        /**
         *定时器集合
         */
        std::map<int, timer*> timersMap;
};


timers::timers(){
    mactive = 1;
}
/**
 *初始化定时器
 */
void timers::Init(int maxnum){
    mepollfd = epoll_create(maxnum);
}

/**
 *增加定时任务
 */
template<typename T>
 int timers::AddTimer(double internalvalue, T callback, void *des, int rep){
    timer* task = new timer(internalvalue, callback, des, rep);
    int timerid = task->GetTimerId();
    struct epoll_event event;
    event.data.fd = timerid;
    event.events = EPOLLIN | EPOLLET;
    timersMap[timerid] = task;
    epoll_ctl(mepollfd, EPOLL_CTL_ADD, timerid, &event);
    task->TimerStart();

    std::thread threadobj(&timers::Run,this);
    threadobj.detach();
    return 0;
    }

template<typename T>
int timers::AddTimer(double internalvalue, T callback, void* des, int rep){
    timer* task = new timer(internalvalue, callback, des, rep);

    int timerid = task->GetTimerId();
    struct epoll_event event;
    event.data.fd = timerid;
    event.events = EPOLLIN | EPOLLET;
    timersMap[timerid] = task;
    epoll_ctl(mepollfd, EPOLL_CTL_ADD, timerid, &event);
    task->TimerStart();
    
    std::thread t1(&timers::Run,this);
    t1.detach();
    return 0;
}



/**
 *删除定时任务
 */
int timers::DelTimer(timer* ptimer){

    int timerId = ptimer->GetTimerId();
    
    struct epoll_event ev;
    ev.data.fd = timerId;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(mepollfd, EPOLL_CTL_DEL, timerId, &ev);
    
    auto itmp = timersMap.find(timerId);
    if(itmp != timersMap.end()){
        // std::cout << "delete successful" << std::endl;
        delete itmp->second;
    }
    timersMap.erase(timerId);
    return 0;    
}

/**
 *运行定时器
 */
int timers::Run(){
    char buffer[MAXFDS] = {0};

    for(; mactive; ){
        struct epoll_event events[MAXFDS];

        bzero(&events, sizeof(events));
        int nfds = epoll_wait(mepollfd, events, MAXFDS, 0);
        for(int i = 0; i < nfds; ++i){
            auto itmp = timersMap.find(events[i].data.fd);
            if(itmp != timersMap.end()){
                while(read(events[i].data.fd, buffer, MAXFDS) > 0){
                }
                (itmp->second->GetUserCallback())();
                printf("%s\n", (char *) itmp->second->GetDes());
                DelTimer(itmp->second);
            }

        }

    }
}

/**
 *停止定时器
 */
int timers::Stop(){
    mactive = 0;
    return 0;
}

/**
 *当前定时任务个数
 */
int timers::Size(){
    return timersMap.size();
}

#endif
