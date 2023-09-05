#pragma once
#include <stdlib.h>
#include <stdio.h>
#define MAX_JOBS 50         //�û�һ������ύ50����ҵ
#define MAX_JobQueue  5		//�󱸶�������
#define MAX_readyQueue 10	//������������
#define MAX_blockQueue 10	//������������
#define MAX_blockedSuspendedQueue 10	//���������������
#define MAX_readySuspendedQueue 10		//���������������
#define TRUE  1             //��
#define FALSE 0             //��
#define EXECUTE 1
#define BLOCKSUSPEND 2
#define READYSUSPEND 3
#define ADDNEWUSERJOB 4


//�û��ύ����ҵ
typedef struct
{
	int UserName;	//�ύ����ҵ���û������� -1Ϊ�Ƿ��û�
	int JID;		//��ҵ��ʶ��
	int start_time;	//�ύʱ��
	int taken;		//����ҵ��Ҫ�Ľ�����
	int time_spent;	//ִ����Ҫ���ѵ�ʱ��
	int num_peocess;//��Ҫ�����Ľ�������
	int io;			//�Ƿ���Ҫio ,��Ҫ=1������Ҫ=0
	int priority;	//��ҵ���ȼ�
	int valid;      //��ҵ�Ƿ���Ч ��Ч=1/TRUE����Ч=0/FALSE
}JOB;


//��ҵ���ƿ�
typedef struct
{
	int UserName;	//�ύ����ҵ���û�������
	int JID;		//��ҵ��ʶ��
	int taken;		//����ҵ��Ҫ�Ľ�����
	int time_spent;	//ִ����Ҫ���ѵ�ʱ��
	int io;			//�Ƿ���Ҫio ,��Ҫ=1/TRUE������Ҫ=0/FALSE
	
	int io_time;	//io������ʱ��Ƭ�ĸ���

	int priority;	//��ҵ���ȼ�

}JCB;

//���̿��ƿ�
typedef struct
{
	int PID;  //���̱�ʶ��
	int io;			//�Ƿ���Ҫio ,��Ҫ=1/TRUE������Ҫ=0/FALSE
	int io_when;	//���ý���ִ�е��ڼ���ʱ��Ƭʱ����io
	int need_time;	//������Ҫ��ʱ��Ƭ��
	int io_exe;		//io�Ѿ�ִ�е�ʱ��
	int io_finish;	//io�Ƿ���ɣ����=true��δ���=false
	int suspend_time;//�Ѿ������ʱ��
	int wait_time;	//�Ѿ��ȴ���ʱ��
	int priority;	//���ȼ�
}PCB;

 
//�ļ���ȡ
void readJob(JOB* UserJob);

//�߼����ȡ�����ҵ����
void JobScheduling(JCB* JobQueue, PCB* readyQueue, JOB* UserJob);

//�ֶ��ӿ���̨���û��ύ����ҵUserJob�������ҵ
void manaulAddJobToUserJob(JOB* UserJob);

////�м�����:�����ȷ���
void middleScheduling(PCB* readyQueue, PCB* readySuspendedQueue);

//�ͼ�����
void processScheduling(PCB* readyQueue, PCB* blockQueue);

//�����е�һ���Ƴ�
void removeFirstProcessScheduling(PCB* pcbQueue, int* len);

//��������
void run(JOB* UserJob, JCB* JobQueue,
	PCB* readyQueue, PCB* blockQueue, PCB* readySuspendedQueue, PCB* blockedSuspendedQueue,
	int choice);//choice����ѡ����Ҫ���ڹ����ж�

//�߼����ȡ�����ҵ����
void JobScheduling(JCB* JobQueue, PCB* readyQueue, JOB* UserJob);

//���󱸶���������µ���ҵ
void updateJobQueue(JCB* JobQueue, JOB* UserJob);