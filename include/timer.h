#ifndef TIMER__H_
#define TIMER__H_

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
#include <functional>

#define MAXFDS 128

class timer{
public:
	typedef std::function<int()> Tcallback;
public:
	timer();
	
	/**
	 * 定时回调普通函数
	 * @param: internalvalue 定时时间
	 * @param: callback 回调函数 对应timer_callback
	 * @param: data 回调函数参数
	 * @param: rep 是否重复定时
	 */
	timer(double internalvalue, Tcallback callback, void* data, int rep);
	
	/**
	 * 定时回调对象有关的函数
	 * @param: internalvalue 定时时间
	 * @param: callback 回调函数 对应timer_callback
	 * @param: frame对象指针 class_timer_callback
	 * @param: data 回调函数参数
	 * @param: rep 是否重复定时
	 */
	timer(double internalvalue, Tcallback callback, void* frame, void* data, int rep);
	
	/**
	 * 拷贝构造
	 */
	timer(const timer &rhs);
	
	timer&        operator=(const timer &rhs);
	
	/**
	 * 启动定时器
	 */
	int           TimerStart();
	/**
	 * 停止定时器
	 */
	int           TimerStop();
	
	/*
	 * 修改定时时间
	 */
	int           TimerChangeInternal(double internal);

	/*
	 * 获得定时器fd
	 */
	int           GetTimerId();
	/*
	 * 获得回调函数参数
	 */
	void*         GetUserData();
	/*
	 * 清空数据
	 */
	void          SetDataNull();
	/*
	 * 删除定时器，需要将fd设置为无效
	 */
	void          DeleteFd();
	/*
	 * 得到回调函数指针
	 */
	Tcallback&         GetUserCallback();
	/*
	 * 获得对象指针
	 */
	void*          GetFrame();
	/*
	 * 当前fd是否有效
	 */
	bool          IsValidFd();
	~timer();
private:
	bool       SetNoBlock(int Fd);
private:
	/*定时时间*/
	double     mInternal;

	/*回调函数*/
	Tcallback      mCb;

	/*当前定时器的fd*/
	int        mTimerId;

	/*定时器是否重复定时*/
	bool       mRepeat;

	/*回调函数的参数*/
	void*      mUserData;

	/*如果是对象类型的回调函数，代表对象指针*/
	void*      mFrame;

	/*fd是否有效*/
	bool       mValidFd;
};

timer::timer():
	mInternal(0.0), mTimerId(0), mRepeat(0), mUserData(0), mFrame(NULL){
		mValidFd = false;
	}
timer::timer(double internalvalue, Tcallback callback, void *data, int rep):
	mInternal(internalvalue), mCb(callback), mUserData(data), mRepeat(rep), mFrame(NULL){
		mValidFd = true;
		mTimerId = timerfd_create(CLOCK_REALTIME, 0);
		SetNoBlock(mTimerId);
	}
timer::timer(double internalvalue, Tcallback callback, void* frame, void* data, int rep):
	mInternal(internalvalue), mCb(callback), mFrame(frame), mUserData(data), mRepeat(rep){
		mTimerId = timerfd_create(CLOCK_REALTIME, 0);
		mValidFd = true;
		SetNoBlock(mTimerId);
	}
timer::timer(const timer &rhs){
	mInternal = rhs.mInternal;
	mCb = rhs.mCb;
	mFrame = rhs.mFrame;
	mTimerId = rhs.mTimerId;
	mRepeat = rhs.mRepeat;
	mUserData = rhs.mUserData;
	mValidFd = false;
}

timer& timer::operator=(const timer &rhs){
	if(this == &rhs){
		return *this;
	}
	mInternal = rhs.mInternal;
	mCb = rhs.mCb;
	mFrame = rhs.mFrame;
	mTimerId = rhs.mTimerId;
	mRepeat = rhs.mRepeat;
	mUserData = rhs.mUserData;
	mValidFd = false;
	return *this;
}
timer::~timer(){
	TimerStop();
}
int timer::TimerStart(){
	struct itimerspec timerinternal;
	bzero(&timerinternal, sizeof(timerinternal));
	timerinternal.it_value.tv_sec = (int)mInternal;
	timerinternal.it_value.tv_nsec = (mInternal - (int)mInternal) * 1000000;
	if(mRepeat){
		timerinternal.it_interval.tv_sec = timerinternal.it_value.tv_sec;
		timerinternal.it_interval.tv_nsec = timerinternal.it_value.tv_nsec;
	}
	timerfd_settime(mTimerId, 0, &timerinternal, NULL);
	return 0;
}

int timer::TimerStop(){
	close(mTimerId);
	return 0;
}

int timer::TimerChangeInternal(double internal){
	this->mInternal = internal;
	TimerStart();
	return 0;
}

int timer::GetTimerId(){
	return mTimerId;
}

void* timer::GetUserData(){
	return mUserData;
}

void timer::SetDataNull(){
	mUserData = NULL;
}

void timer::DeleteFd(){
	mValidFd = false;
}

timer::Tcallback& timer::GetUserCallback(){
	return mCb;
}

void* timer::GetFrame(){
	return mFrame;
}
bool timer::IsValidFd(){
	return mValidFd;
}

bool timer::SetNoBlock(int Fd){
	int flags = fcntl(Fd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if (-1 == fcntl(Fd, F_SETFL, flags)) {
        return false;
	}
	return true;
}
#endif

