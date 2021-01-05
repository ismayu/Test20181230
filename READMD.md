#### 简单易用的Timer

利用Linux下`timerfd`的定时器接口创建定时器，配合`epoll`完成了定时器启动、添加多个定时器、多线程处理定时器任务。

通过直接添加时间和回调函数的方式简化了定时任务的添加，添加定时任务代码如下：

```c++
timers times1;

times1.Init();

times1.AddTimer(2.2, std::bind(Add,20, 50), NULL, 0);
```

添加回调函数之后会开辟单独的线程去处理。


