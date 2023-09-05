#pragma warning(disable: 4996)

#include"AdvancedScheduling.h"

const int RECEIVELEN = 5; //后备队列长度
extern int JobQueueLen;		//后备队列长度
extern int readyQueueLen;		//就绪队列长度
extern int blockQueueLen;		//阻塞队列长度
extern int blockedSuspendedQueueLen;	//阻塞挂起队列长度
extern int readySuspendedQueueLen;		//就绪挂起队列长度
//显示用户提交的作业队列
void showJob(JOB job[])
{
	printf("priority JID  taken   io  time_spent  Username\n");
	for (int i = 0; i < 50; i++)
	{
		if (job[i].valid == TRUE)
			printf("     %d      %d     %d      %d    %d        %d\n", job[i].priority, job[i].JID, job[i].taken, job[i].io, job[i].time_spent, job[i].UserName);
		else
			break;
	}
}

//显示用户作业Userjob
void showUserJob(JOB job[])
{
	printf("\n----------------------------------------------------\n");
	printf("用户提交的所有作业如下：\n");
	printf("priority JID  taken   io  time_spent  Username\n");
	for (int i = 0; i < MAX_JOBS; i++)
	{
		if (job[i].valid == TRUE)
			printf("     %d      %d     %d      %d    %d        %d\n", job[i].priority, job[i].JID, job[i].taken, job[i].io, job[i].time_spent, job[i].UserName);
		else
			break;
	}
}

//显示后备队列
void showJCB(JCB jcb[])
{
	printf("\n----------------------------------------------------\n");
	printf("后备队列如下：\n");
	printf("# priority JID  taken   io    time_spent    Username\n");
	for (int i = 0; i < JobQueueLen; i++)
	{

		printf("     %d      %d     %d      %d       %d             %d\n", jcb[i].priority, jcb[i].JID, jcb[i].taken, jcb[i].io, jcb[i].time_spent, jcb[i].UserName);
	}
	printf("\n----------------------------------------------------\n");
}


void showPCB(PCB pcb[], int len)
{

	printf("队列如下：\n");
	printf("     PID   io   io_when  need_time    io_finish  priority\n");
	for (int i = 0; i < len; i++)
	{

		printf("     %d      %d     %d        %d           %d          %d\n", pcb[i].PID, pcb[i].io, pcb[i].io_when, pcb[i].need_time, pcb[i].io_finish, pcb[i].priority);
	}
	printf("\n----------------------------------------------------\n");
}

//显示就绪队列
void showReadyQueue(PCB* readyQueue)
{
	printf("\n----------------------------------------------------\n");
	printf("就绪");
	showPCB(readyQueue, readyQueueLen);
}


//显示阻塞队列
void showBlockQueue(PCB* blockQueue)
{
	printf("\n----------------------------------------------------\n");
	printf("阻塞");

	PCB* pcb = blockQueue;
	int len = blockQueueLen;
	printf("队列如下：\n");
	printf("     PID   io   io_when  need_time    io_finish  priority   io_exe\n");
	for (int i = 0; i < len; i++)
	{

		printf("     %d      %d     %d        %d           %d          %d          %d\n", pcb[i].PID, pcb[i].io, pcb[i].io_when, pcb[i].need_time, pcb[i].io_finish, pcb[i].priority, pcb[i].io_exe);
	}
	printf("\n----------------------------------------------------\n");
}

//显示阻塞挂起队列
void showBlockedSuspendedQueue(PCB* blockedSuspendedQueue)
{
	printf("\n----------------------------------------------------\n");
	printf("阻塞挂起队列如下：\n");

	PCB* pcb = blockedSuspendedQueue;
	int len = blockedSuspendedQueueLen;

	printf("     PID   io   io_when  need_time    io_finish  priority  suspend_time\n");
	for (int i = 0; i < len; i++)
	{

		printf("     %d      %d     %d        %d           %d          %d          %d\n", pcb[i].PID, pcb[i].io, pcb[i].io_when, pcb[i].need_time, pcb[i].io_finish, pcb[i].priority, pcb[i].suspend_time);
	}
	printf("\n----------------------------------------------------\n");
}

//显示就绪挂起队列
void showReadySuspendedQueue(PCB* readySuspendedQueue)
{
	printf("\n----------------------------------------------------\n");
	printf("就绪挂起队列如下：\n");

	PCB* pcb = readySuspendedQueue;
	int len = readySuspendedQueueLen;

	printf("     PID   io   io_when  need_time    io_finish  priority  suspend_time\n");
	for (int i = 0; i < len; i++)
	{

		printf("     %d      %d     %d        %d           %d          %d          %d\n", pcb[i].PID, pcb[i].io, pcb[i].io_when, pcb[i].need_time, pcb[i].io_finish, pcb[i].priority, pcb[i].suspend_time);
	}
	printf("\n----------------------------------------------------\n");
}

int main()
{
	//-----------------------数据队列定义 start---------------------------
	JOB UserJob[MAX_JOBS];	//所有待完成的作业
	//后备队列
	JCB JobQueue[MAX_JobQueue];
	JobQueueLen = 0;			//后备队列长度

	//就绪队列
	PCB readyQueue[MAX_readyQueue];
	readyQueueLen = 0;

	//阻塞队列
	PCB blockQueue[MAX_blockQueue];
	blockQueueLen = 0;

	//阻塞挂起队列
	PCB blockedSuspendedQueue[MAX_blockedSuspendedQueue];
	blockedSuspendedQueueLen = 0;

	//就绪挂起队列
	PCB readySuspendedQueue[MAX_readySuspendedQueue];
	readySuspendedQueueLen = 0;

	//-----------------------数据队列定义 end----------------------------

	readJob(UserJob);		//从文件中读取用户提交的所有作业
	showJob(UserJob);

	updateJobQueue(JobQueue, UserJob);
	showJCB(JobQueue);

	//高级调度——作业调度:最短时间优先
	JobScheduling(JobQueue, readyQueue, UserJob);
	printf("就绪");
	showPCB(readyQueue, readyQueueLen);

	showJCB(JobQueue);

	//低级调度——进程调度：先来先服务
	printf("======================初始化完成，开始调度模拟=================\n");
	int i = 0;
	while (1)
	{
		printf("请选择功能：1.正常执行 2.挂起阻塞队列 3.挂起就绪队列 4.提交新的作业");


		/*	EXECUTE 1
			BLOCKSUSPEND 2
			READYSUSPEND 3*/
		int choice;
		scanf("%d", &choice);

		if (choice == ADDNEWUSERJOB) {
			manaulAddJobToUserJob(UserJob);
		}

		run(UserJob, JobQueue, readyQueue, blockQueue, readySuspendedQueue, blockedSuspendedQueue, choice);
		printf("第 %d 次调度完成：\n", i += 1);
		//printf("JobQueueLen=%d", JobQueueLen);
		showUserJob(UserJob);//显示所有的用户作业
		//showJCB(JobQueue, JobQueueLen);//
		showJCB(JobQueue);//显示后备队列
		showReadyQueue(readyQueue);//显示就绪队列
		showBlockQueue(blockQueue);//显示阻塞队列
		showBlockedSuspendedQueue(blockedSuspendedQueue);//显示阻塞挂起队列
		showReadySuspendedQueue(readySuspendedQueue);//显示就绪挂起队列

		system("pause");
	}

	return 0;
}