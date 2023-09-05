#ifndef OS_VIEW_H
#define OS_VIEW_H
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include "AdvancedScheduling.h"
#include <QPushButton>>
#include <QLineEdit>
#include <QRadioButton>

const int RECEIVELEN = 5; //后备队列长度
extern int JobQueueLen ;		//后备队列长度
extern int readyQueueLen ;		//就绪队列长度
extern int blockQueueLen ;		//阻塞队列长度
extern int blockedSuspendedQueueLen ;	//阻塞挂起队列长度
extern int readySuspendedQueueLen ;		//就绪挂起队列长度
#define K_INVALID 0 //非法类型
#define K_PCB 1     //PCB类型
#define K_JCB 2     //JCB类型
#define LEFT  0     //方向向左（队首的位置，在左手边）
#define RIGHT 1     //方向向右
#define INCIDENTINVALID 0//事件无效
#define SUSPENDREADY 1  //挂起就绪队列
#define SUSPENDBLOCK 2  //挂起阻塞队列
#define MODELINVALID 0  //非法模式
#define MODELEX 1   //执行模式
#define MODELPLAY 2 //播放模式
#define QUEUEREADY 0 //就绪队列


class MatrixRect : public QWidget
{
public:
    MatrixRect(QWidget *parent = nullptr);
    void setRectangleColor(const QColor& color);
    void setMatrixPosition(const QPoint& position);
    void setMatrixCount(int count); // 设置矩形个数
    void updateData();
    void setQueueName(QString queueName);//设置队列名字
    int Kind = K_INVALID;//默认自己的队列是非法类型
    int DIREC = RIGHT;//默认在头在右边
    int maxLen;//队列最多任务数量
    int nowLen;//队列最少任务数量
    //TODO:进程队列+作业队列
    JCB* jcbQueue;
    PCB* pcbQueue;
    int QueueKind = -1;//队列类型

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int rectWidth ; // 每个矩形的宽度
    int rectHeight ; // 矩形的高度
    QColor m_rectangleColor;
    QVector<int> Num; // 矩形显示依赖数组

    QPoint m_matrixPosition;
    int m_matrixCount; //矩形的个数
    QString queueName;//队列的名字

};





//----------------
class OS_View : public QWidget
{
    Q_OBJECT

public:
    OS_View(QWidget *parent = nullptr);
    ~OS_View();
    void paintEvent(QPaintEvent* event) override;
    void drawCpuGraphics(QPainter& painter, const QPoint& center, int radius);
    void drawLine(QPainter& painter, const QPoint& startPoint, const QPoint& endPoint, QString labelText, bool withArrow, int arrowPosition=RIGHT);
    void VMExect();//模拟执行,一次执行1个CPU_time
    void recordData();//记录数据到文件
    void replay();//重放函数
    void readRecord(int exNum);//从文件中读取记录
    void updateModeButtonColor();//更新按钮颜色
private:
    //模式
    int mode = MODELINVALID; //默认模式
    //int mode = MODELPLAY; //默认模式
    //记录文件的名
    QString recordFileName;
    //定义各种队列
    JOB UserJob[MAX_JOBS];    //所有待完成的作业
    JCB JobQueue[MAX_JobQueue];//后备队列
    PCB readyQueue[MAX_readyQueue];//就绪队列
    PCB blockQueue[MAX_blockQueue];//阻塞队列
    PCB blockedSuspendedQueue[MAX_blockedSuspendedQueue];//阻塞挂起队列
    PCB readySuspendedQueue[MAX_readySuspendedQueue];//就绪挂起队列
    int exNum = 0;//显示计数，调度了第几次
    //定义队列对应的矩形数组
    MatrixRect jobMatrix; // 后备队列矩阵
    MatrixRect readyMatrix; // 就绪队列矩阵
    MatrixRect blockMatrix; // 阻塞队列矩阵
    MatrixRect blockedSuspendedMatrix; // 阻塞挂起矩阵
    MatrixRect readySuspendedMatrix; // 就绪挂起矩阵
    QTimer m_timer;
    int biasX = 50;//默认x坐标偏移
    int biasY = 100;//默认y坐标偏移
    int incident = INCIDENTINVALID;//事件默认无效

    QPushButton *suspendReadyButton; // 挂起就绪队列队首按钮
    QPushButton *suspendBlockButton; // 挂起阻塞队列队首按钮
    QVector<QLineEdit*> inputFields; // 存储输入框的容器
    QPushButton *submitJobButton; // 提交作业按钮
    QPushButton* autoSubmitJobButton; // 自动产生作业按钮



    bool isPaused = true; // 标识 m_timer 是否处于暂停状态
    QPushButton *pauseButton; // 暂停按钮
    QPushButton *nextButton;//用于点击下一步进行演示
    QPushButton *modeButton; // 模式按钮
    //QPushButton *fileSelectButton; // 文件选择按钮xx
    QLabel *selectedFileLabel; // 显示已选择的文件名的标签

    QString selectFile="NULL";//已选择的文件
    QPalette buttonPalette; // 用于设置按钮颜色的QPalette对象
private slots: //按钮槽函数
    void suspendReadyQueueHead();
    void suspendBlockQueueHead();
    void submitJob(); // 提交作业按钮点击事件处理函数
    void autoSubmitJob();//自动产生作业
    void pauseButtonClicked(); // 暂停按钮点击事件处理函数
    void nextButtonClicked();//单步运行的按钮响应函数
    void modeButtonClicked();//模式选择按钮函数
    void fileSelectButtonClicked(); // 文件选择按钮点击事件处理函数
};


#endif // OS_VIEW_H
