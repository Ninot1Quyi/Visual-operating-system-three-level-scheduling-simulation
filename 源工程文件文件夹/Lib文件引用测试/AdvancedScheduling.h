#pragma once
#include <stdlib.h>
#include <stdio.h>
#define MAX_JOBS 50         //用户一次最多提交50个作业
#define MAX_JobQueue  5		//后备队列容量
#define MAX_readyQueue 10	//就绪队列容量
#define MAX_blockQueue 10	//就绪队列容量
#define MAX_blockedSuspendedQueue 10	//阻塞挂起队列容量
#define MAX_readySuspendedQueue 10		//就绪挂起队列容量
#define TRUE  1             //真
#define FALSE 0             //假
#define EXECUTE 1
#define BLOCKSUSPEND 2
#define READYSUSPEND 3
#define ADDNEWUSERJOB 4


//用户提交的作业
typedef struct
{
	int UserName;	//提交该作业的用户的名字 -1为非法用户
	int JID;		//作业标识符
	int start_time;	//提交时间
	int taken;		//该作业需要的进程数
	int time_spent;	//执行需要花费的时间
	int num_peocess;//需要创建的进程数量
	int io;			//是否需要io ,需要=1，不需要=0
	int priority;	//作业优先级
	int valid;      //作业是否有效 有效=1/TRUE，无效=0/FALSE
}JOB;


//作业控制块
typedef struct
{
	int UserName;	//提交该作业的用户的名字
	int JID;		//作业标识符
	int taken;		//该作业需要的进程数
	int time_spent;	//执行需要花费的时间
	int io;			//是否需要io ,需要=1/TRUE，不需要=0/FALSE
	
	int io_time;	//io的请求时间片的个数

	int priority;	//作业优先级

}JCB;

//进程控制块
typedef struct
{
	int PID;  //进程标识符
	int io;			//是否需要io ,需要=1/TRUE，不需要=0/FALSE
	int io_when;	//当该进程执行到第几个时间片时发生io
	int need_time;	//进程需要的时间片数
	int io_exe;		//io已经执行的时间
	int io_finish;	//io是否完成，完成=true，未完成=false
	int suspend_time;//已经挂起的时间
	int wait_time;	//已经等待的时间
	int priority;	//优先级
}PCB;

 
//文件读取
void readJob(JOB* UserJob);

//高级调度——作业调度
void JobScheduling(JCB* JobQueue, PCB* readyQueue, JOB* UserJob);

//手动从控制台向用户提交的作业UserJob中添加作业
void manaulAddJobToUserJob(JOB* UserJob);

////中级调度:先来先服务
void middleScheduling(PCB* readyQueue, PCB* readySuspendedQueue);

//低级调度
void processScheduling(PCB* readyQueue, PCB* blockQueue);

//将队列第一个移除
void removeFirstProcessScheduling(PCB* pcbQueue, int* len);

//程序运行
void run(JOB* UserJob, JCB* JobQueue,
	PCB* readyQueue, PCB* blockQueue, PCB* readySuspendedQueue, PCB* blockedSuspendedQueue,
	int choice);//choice功能选择，主要用于挂起判断

//高级调度——作业调度
void JobScheduling(JCB* JobQueue, PCB* readyQueue, JOB* UserJob);

//往后备队列中添加新的作业
void updateJobQueue(JCB* JobQueue, JOB* UserJob);