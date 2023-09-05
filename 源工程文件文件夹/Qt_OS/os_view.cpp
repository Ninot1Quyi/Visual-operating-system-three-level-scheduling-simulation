
#include "OS_View.h"
#include <QPainter>
#include <QTime>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include "QFile"
#include <QFileDialog>
//--------------------------------------------
//Class Name:MatrixRect
//功能：根据队列情况绘制矩阵数组
//Author:曲艺
//-------------------------------------------
MatrixRect::MatrixRect(QWidget *parent)
    : QWidget(parent)
{
    //初始化指针为空
    jcbQueue = NULL;
    pcbQueue = NULL;
    maxLen = 0;
    nowLen = 0;
    this->rectWidth = 50; // 每个矩形的宽度
    this->rectHeight = 80; // 矩形的高度
    //默认颜色是红色
    m_rectangleColor = Qt::red;
    //初始位置0,0
    m_matrixPosition = QPoint(0, 0);
    //默认矩形数量10个
    m_matrixCount = 10;

    //初始化矩阵数组为0
    Num.resize(m_matrixCount, 0);
}

void MatrixRect::setRectangleColor(const QColor& color)
{
    m_rectangleColor = color;
    update();
}

void MatrixRect::setMatrixPosition(const QPoint& position)
{
    m_matrixPosition = position;
    update();
}

void MatrixRect::setMatrixCount(int count)
{
    m_matrixCount = count;
    Num.resize(m_matrixCount, 0);
    update();
}

void MatrixRect::updateData()
{
//    srand(static_cast<unsigned>(time(nullptr)));

//    for (int i = 0; i < m_matrixCount; ++i) {
//        Num[i] = rand() % 2;
//    }

    update();
}

void MatrixRect::setQueueName(QString queueName)//设置队列名字
{
    this->queueName = queueName;
}

void MatrixRect::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 计算整体矩形的位置
    QRect matrixRect(m_matrixPosition.x(), m_matrixPosition.y(), rectWidth * 10, rectHeight);

    painter.save();
    painter.translate(matrixRect.topLeft()); // 平移坐标系，使得绘制的矩形位置相对于整体矩形

    // 绘制队列名称
    QRect textRect(0, -20, rectWidth * m_matrixCount, 20); // 文本矩形的位置和大小
    painter.drawText(textRect, Qt::AlignCenter, queueName);

    // 设置字体大小
    QFont font;
    font.setPointSize(9);  // 设置字体大小为7点
    painter.setFont(font);

    int startIdx = 0;
    int step = 1;
    if (this->DIREC == RIGHT) {
         startIdx = m_matrixCount - 1;
        step = -1;
    }

        for (int i = 0; i < m_matrixCount; ++i) {
            int index = startIdx + step * i;

            QRect rect(i * rectWidth, 0, rectWidth, rectHeight);
            if (nowLen > 0 && (((i == 0 && DIREC == LEFT))||(i == m_matrixCount-1 && DIREC == RIGHT))) {
                if (QueueKind == QUEUEREADY) {
                painter.setBrush(Qt::green);
                }
                else {
                    painter.setBrush(Qt::red);
                }

              
            } else {
                painter.setBrush(Qt::yellow);
            }
            painter.drawRect(rect);
            if (this->Kind == K_PCB && index >= 0 && index < nowLen) {
                // 在矩形内部绘制结构体信息
                PCB pcb = pcbQueue[index];  // 假设pcbQueue是一个包含PCB结构体的队列
                QString pcbInfo = QString("PID: %1\nIO: %2\nNT: %3\nWT: %4\nPR: %5")
                                      .arg(pcb.PID)
                                      //.arg(pcb.io)//
                                      .arg(pcb.io_when)
                                      .arg(pcb.need_time)
                                      //.arg(pcb.io_exe)
                                      //.arg(pcb.io_finish)//
                                      //.arg(pcb.suspend_time)//
                                      .arg(pcb.wait_time)
                                      .arg(pcb.priority);

                painter.drawText(rect, Qt::AlignCenter, pcbInfo);
            }else if (this->Kind == K_JCB && index >= 0 && index < nowLen) {
                // 在矩形内部绘制结构体信息
                JCB jcb = jcbQueue[index];  // 假设pcbQueue是一个包含PCB结构体的队列
                //"ETE"（Estimated Time of Execution）
                //"NPC"（Number of Processes Required）
                QString jcbInfo = QString("JID: %1\nNPC: %2\nETE: %3\nPR: %4")
                                      .arg(jcb.JID)
                                      .arg(jcb.taken)
                                      .arg(jcb.time_spent)
                                      .arg(jcb.priority);

                painter.drawText(rect, Qt::AlignCenter, jcbInfo);
            }
        }



    painter.restore();
}





//--------------------------------------------
//Class Name:OS_View
//功能：将cpu三级调度过程可视化
//Author:曲艺
//-------------------------------------------

OS_View::OS_View(QWidget *parent)
    : QWidget(parent)
{
    //创建本次执行的记录文件

    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    recordFileName = "record/记录_" + currentDate + ".txt";
    QFile file(recordFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // 文件打开失败，处理错误
            return;
    }

    file.close(); // 关闭文件


    QGridLayout *layout = new QGridLayout(this);

    // 设置整体矩形的大小
    int matrixWidth = 100;
    int matrixHeight = 200;

    // 设计距离上边的起始纵坐标
    int startY = biasY;
    // 设置矩形数组的位置和颜色
    // 后备队列
    int jobMatrixX = biasX + 60;
    jobMatrix.setMatrixPosition(QPoint(jobMatrixX, startY));
    jobMatrix.setRectangleColor(Qt::green);
    jobMatrix.setQueueName("后备队列");
    jobMatrix.setMatrixCount(5);
    // TODO:设置队列类型
    jobMatrix.Kind = K_JCB;
    jobMatrix.jcbQueue = JobQueue;
    jobMatrix.maxLen = MAX_JobQueue;
    jobMatrix.nowLen = 0;
    jobMatrix.DIREC = RIGHT;

    // 设置第二列的起始X坐标
    int startX = jobMatrixX + 340;
    // 就绪队列
    int readyMatrixY = startY;
    readyMatrix.setMatrixPosition(QPoint(startX, readyMatrixY));
    readyMatrix.setRectangleColor(Qt::blue);
    readyMatrix.setQueueName("就绪队列");
    readyMatrix.Kind = K_PCB; // 队列类型PCB
    readyMatrix.pcbQueue = readyQueue;
    readyMatrix.maxLen = MAX_readyQueue;
    readyMatrix.nowLen = 0;
    readyMatrix.DIREC = RIGHT;
    readyMatrix.QueueKind = QUEUEREADY;

    // 纵向间隔
    int stepY = 140;
    // 就绪挂起队列
    int readySuspendedMatrixY = readyMatrixY + stepY;
    readySuspendedMatrix.setMatrixPosition(QPoint(startX + 17, readySuspendedMatrixY));
    readySuspendedMatrix.setRectangleColor(Qt::cyan);
    readySuspendedMatrix.setQueueName("就绪,挂起队列");
    // TODO:设置队列类型
    readySuspendedMatrix.Kind = K_PCB;
    readySuspendedMatrix.pcbQueue = readySuspendedQueue;
    readySuspendedMatrix.maxLen = MAX_readySuspendedQueue;
    readySuspendedMatrix.nowLen = 0;
    readySuspendedMatrix.DIREC = LEFT;

    // 阻塞挂起队列
    int blockedSuspendedMatrixY = readySuspendedMatrixY + stepY;
    blockedSuspendedMatrix.setMatrixPosition(QPoint(startX, blockedSuspendedMatrixY));
    blockedSuspendedMatrix.setRectangleColor(Qt::yellow);
    blockedSuspendedMatrix.setQueueName("阻塞,挂起队列");
    // TODO:设置队列类型
    blockedSuspendedMatrix.Kind = K_PCB;
    blockedSuspendedMatrix.pcbQueue = blockedSuspendedQueue;
    blockedSuspendedMatrix.maxLen = MAX_blockedSuspendedQueue;
    blockedSuspendedMatrix.nowLen = 0;
    blockedSuspendedMatrix.DIREC = LEFT;

    // 阻塞队列
    int blockMatrixY = blockedSuspendedMatrixY + stepY;
    blockMatrix.setMatrixPosition(QPoint(startX + 17, blockMatrixY));
    blockMatrix.setRectangleColor(Qt::red);
    blockMatrix.setQueueName("阻塞队列");
    // TODO:设置队列类型
    blockMatrix.Kind = K_PCB;
    blockMatrix.pcbQueue = blockQueue;
    blockMatrix.maxLen = MAX_blockQueue;
    blockMatrix.nowLen = 0;
    blockMatrix.DIREC = LEFT;

    // 添加矩形数组到布局中
    layout->addWidget(&jobMatrix, 0, 0, matrixHeight, matrixWidth);
    layout->addWidget(&readyMatrix, 0, 1, matrixHeight, matrixWidth);
    layout->addWidget(&blockMatrix, 1, 0, matrixHeight, matrixWidth);
    layout->addWidget(&blockedSuspendedMatrix, 1, 1, matrixHeight, matrixWidth);
    layout->addWidget(&readySuspendedMatrix, 2, 0, matrixHeight, matrixWidth);

    connect(&m_timer, &QTimer::timeout, this, &OS_View::VMExect);
    // 连接定时器的timeout事件到矩形更新槽函数
    connect(&m_timer, &QTimer::timeout, &jobMatrix, &MatrixRect::updateData);
    connect(&m_timer, &QTimer::timeout, &readyMatrix, &MatrixRect::updateData);
    connect(&m_timer, &QTimer::timeout, &blockMatrix, &MatrixRect::updateData);
    connect(&m_timer, &QTimer::timeout, &blockedSuspendedMatrix, &MatrixRect::updateData);
    connect(&m_timer, &QTimer::timeout, &readySuspendedMatrix, &MatrixRect::updateData);

    //connect(&m_timer, &QTimer::timeout, this, &OS_View::recordData); // 记录数据的函数

    // 设置定时器间隔为1秒，并启动定时器
    m_timer.setInterval(100);
    if(!isPaused){
        m_timer.start();
    }


    //---------暂停按钮-----------
    isPaused = true; // 初始化为暂停状态

    // 设置按钮位置和大小等属性...
    // ---------挂起按钮------------
    // 定义按钮
    // 创建按钮并设置名称
    pauseButton = new QPushButton("开始", this);
    suspendReadyButton = new QPushButton("挂起就绪队列队首", this);
    suspendBlockButton = new QPushButton("挂起阻塞队列队首", this);

    int peartY = 550;
    // 设置按钮位置和大小
    int buttonStartX = 550;
    suspendReadyButton->move(buttonStartX, peartY+100);
    suspendBlockButton->move(buttonStartX+115, peartY+100);
    pauseButton->move(buttonStartX+235,peartY+100);
    // 设置按钮宽度
    int buttonWidth = 110;
    suspendReadyButton->setFixedWidth(buttonWidth);
    suspendBlockButton->setFixedWidth(buttonWidth);
    pauseButton->setFixedWidth(buttonWidth/2);
    // 连接按钮的点击事件到相应的槽函数
    connect(pauseButton, &QPushButton::clicked, this, &OS_View::pauseButtonClicked);
    connect(suspendReadyButton, &QPushButton::clicked, this, &OS_View::suspendReadyQueueHead);
    connect(suspendBlockButton, &QPushButton::clicked, this, &OS_View::suspendBlockQueueHead);

    // --------添加作业输入框---------------
    // 创建输入框
    int inputFieldX = 50;
    int inputFieldY = peartY+100;
    int inputFieldWidth = 50;
    int inputFieldHeight = 30;
    int labelWidth = inputFieldWidth;
    int labelHeight = 20;
    int labelX, labelY;

    for (int i = 0; i < 6; ++i) {
            // 创建标签
            QLabel* label = new QLabel(this);
            QString labelText;
            switch (i) {
            case 0:
                labelText = "优先级";
                break;
            case 1:
                labelText = "作业编号";
                break;
            case 2:
                labelText = "进程数";
                break;
            case 3:
                labelText = "是否IO";
                break;
            case 4:
                labelText = "执行时间";
                break;
            case 5:
                labelText = "创建者";
                break;
            }
            label->setText(labelText); // 设置标签文本

            // 计算标签位置
            labelX = inputFieldX + i * (inputFieldWidth + 10) + (inputFieldWidth - labelWidth) / 2;
            labelY = inputFieldY - 20;

            // 设置标签位置和大小
            label->setGeometry(labelX, labelY, labelWidth, labelHeight);
            label->setAlignment(Qt::AlignCenter); // 将标签文本居中对齐

            // 创建输入框
            QLineEdit* lineEdit = new QLineEdit(this);
            lineEdit->setGeometry(inputFieldX + i * (inputFieldWidth + 10), inputFieldY, inputFieldWidth, inputFieldHeight);
            inputFields.append(lineEdit);
    }



    // 创建"提交作业"按钮
    submitJobButton = new QPushButton("提交作业", this);
    submitJobButton->setGeometry(inputFieldX + 6 * (inputFieldWidth + 10), inputFieldY, inputFieldWidth+5, inputFieldHeight);
    connect(submitJobButton, &QPushButton::clicked, this, &OS_View::submitJob);

    //自动生成作业按钮
    autoSubmitJobButton = new QPushButton("自动产生作业", this);
    autoSubmitJobButton->setGeometry(inputFieldX + 7 * (inputFieldWidth + 10)-5, inputFieldY, inputFieldWidth + 30, inputFieldHeight);
    connect(autoSubmitJobButton, &QPushButton::clicked, this, &OS_View::autoSubmitJob);


    // 创建 "next" 按钮
    nextButton = new QPushButton("Next", this);
    nextButton->move(buttonStartX + 355, peartY + 100);
    nextButton->setFixedWidth(buttonWidth / 2);
    connect(nextButton, &QPushButton::clicked, this, &OS_View::nextButtonClicked);

    // 创建 "模式" 按钮
    modeButton = new QPushButton("当前模式：\n未选择", this);
    modeButton->move(buttonStartX + 450, peartY + 70);
    modeButton->setFixedWidth(buttonWidth / 2+20);
    modeButton->setFixedHeight(60);
    connect(modeButton, &QPushButton::clicked, this, &OS_View::modeButtonClicked);
    // 设置按钮的颜色
    buttonPalette = modeButton->palette();
    updateModeButtonColor();
    //文件选择
    selectedFileLabel = new QLabel(this);
    selectedFileLabel->setGeometry(buttonStartX + 550, peartY + 80, 200, 60); // 设置标签的位置和大小

    ////显示调度算法类型
    QLabel* label = new QLabel(this);
    label->setText("进程调度：\n抢占式\n高优先权调度"); // 设置标签文本
    label->setGeometry(985,95, 300, 60); // 设置标签的位置和大小
    setLayout(layout);
}


void OS_View::updateModeButtonColor() {
    buttonPalette.setColor(QPalette::ButtonText, Qt::white);
    if (mode == MODELINVALID) {

            modeButton->setStyleSheet("background-color: gray;");
    } else if (mode == MODELEX) {

            modeButton->setStyleSheet("background-color: green;");
    } else if (mode == MODELPLAY) {

            modeButton->setStyleSheet("background-color: blue;");
    }

    modeButton->setPalette(buttonPalette);
    modeButton->update(); // 更新按钮的显示
}


void OS_View::fileSelectButtonClicked()
{
    QString selectedFilePath = QFileDialog::getOpenFileName(this, "选择文件", "./record", "All Files (*.*)");
    if (!selectedFilePath.isEmpty())
    {
            // 在这里处理用户选择的文件路径
            QFileInfo fileInfo(selectedFilePath); // 创建QFileInfo对象，传入文件路径
            QString fileName = fileInfo.fileName(); // 使用fileName()函数获取文件名部分
            selectedFileLabel->setText("已选择文件：\n"+fileName);
            this->selectFile = selectedFilePath;
    }
}



void OS_View::modeButtonClicked(){
    exNum = 0;
    isPaused = true;//默认是暂停状态
    m_timer.stop();
    pauseButton->setText("开始");
    if(this->mode == MODELEX){
        modeButton->setText("当前模式：\n回放");
        this->mode = MODELPLAY;
        updateModeButtonColor();

        fileSelectButtonClicked();//调用选择文件函数
    }else{
        modeButton->setText("当前模式：\n执行作业");

        // 初始化队列长度
        JobQueueLen = 0;
        readyQueueLen = 0;
        blockQueueLen = 0;
        blockedSuspendedQueueLen = 0;
        readySuspendedQueueLen = 0;
        //-----------------------数据队列定义 end----------------------------
        readJob(UserJob);        // 从文件中读取用户提交的所有作业
        updateJobQueue(JobQueue, UserJob);
        JobScheduling(JobQueue, readyQueue, UserJob); // 高级调度——作业调度:最短时间优先
        printf("就绪");
        printf("======================初始化完成，开始调度模拟=================\n");
        this->mode = MODELEX;
        updateModeButtonColor();
    }
}




//单步运行的按钮响应函数
void OS_View::nextButtonClicked()
{
    VMExect(); // 执行与 m_timer 相同的操作
    //recordData(); // 记录数据到文件

    // 执行矩形的更新操作（如果需要的话）
    jobMatrix.updateData();
    readyMatrix.updateData();
    blockMatrix.updateData();
    blockedSuspendedMatrix.updateData();
    readySuspendedMatrix.updateData();
}


//点击暂停按钮事件
void OS_View::pauseButtonClicked()
{
    if (isPaused) {
            // 如果是暂停状态，则继续执行
            m_timer.start();
            isPaused = false;
            pauseButton->setText("暂停");
    } else {
            // 如果不是暂停状态，则暂停执行
            m_timer.stop();
            isPaused = true;
            pauseButton->setText("开始");
    }
}



void OS_View::suspendReadyQueueHead()
{
    incident = SUSPENDREADY;
}

void OS_View::suspendBlockQueueHead()
{
    incident = SUSPENDBLOCK;
}

void OS_View::submitJob()
{

    int numFields = inputFields.size();
 
    int index = 0;
    while (UserJob[index].valid == TRUE) //找到第一个无效作业
    {
            index++;
    }
    if (index >= MAX_JOBS) //作业数组已满
    {
            printf("作业数组已满\n");
            return;
    }

    UserJob[index].valid = true;
    for (int i = 0; i < numFields; ++i) {
        QLineEdit *lineEdit = inputFields[i];
        int value = lineEdit->text().toInt();
        // 根据实际需要将输入框的值赋值给对应的变量，这里仅给出示例
        switch (i) {
        case 0:
            UserJob[index].priority = value;
            break;
        case 1:
            UserJob[index].JID = value;
            break;
        case 2:
            UserJob[index].taken = value;
            break;
        case 3:
            UserJob[index].io = value;
            break;
        case 4:
            UserJob[index].time_spent = value;
            break;
        case 5:
            UserJob[index].UserName = value;
            break;
        default:
            break;
        }
    }
 
}

//自动生成作业
void OS_View::autoSubmitJob() {
    int numFields = inputFields.size();

    int index = 0;
    while (UserJob[index].valid == TRUE) //找到第一个无效作业
    {
        index++;
    }
    if (index >= MAX_JOBS) //作业数组已满
    {
        printf("作业数组已满\n");
        return;
    }

    UserJob[index].valid = true;

    //使用真随机数
    srand((unsigned)time(NULL));
    
    // 随机生成作业的各项参数
    UserJob[index].priority = rand() % 10 + 1;
    UserJob[index].JID = index;
    UserJob[index].taken = rand() % 10 + 1;
    UserJob[index].io = rand() % 10 + 1;
    UserJob[index].time_spent = rand() % 10 + 1;
    UserJob[index].UserName = rand() % 10 + 1;



}

//绘制CPU图形
void OS_View::drawCpuGraphics(QPainter& painter, const QPoint& center, int radius)
{

    // 绘制圆形
    painter.setBrush(Qt::yellow);  // 设置填充颜色为黄色
    painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);

    // 绘制文字
    painter.setPen(Qt::black);    // 设置文字颜色为黑色
    QFont fontCpu;
    fontCpu.setPointSize(12);
    painter.setFont(fontCpu);
    painter.drawText(QRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2), Qt::AlignCenter, "CPU");
}

// 绘制线段，可选择是否带箭头
// 绘制线段，可选择是否带箭头，箭头位置由arrowPosition参数决定
// 绘制线段，可选择是否带箭头，箭头位置由arrowPosition参数决定
// 绘制线段，可选择是否带箭头，箭头位置由arrowPosition参数决定
void OS_View::drawLine(QPainter& painter, const QPoint& startPoint, const QPoint& endPoint, QString labelText, bool withArrow, int arrowPosition)
{
    // 绘制线段
    painter.drawLine(startPoint, endPoint);

    if (withArrow) {
        // 计算箭头的参数
        double angle = std::atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());
        int arrowSize = 5;

        // 计算箭头的三个点
        QPointF arrowPoint1, arrowPoint2, arrowPoint3;

        if (arrowPosition == LEFT) {
            arrowPoint1 = startPoint + QPointF(arrowSize * std::cos(angle - M_PI / 6), arrowSize * std::sin(angle - M_PI / 6));
            arrowPoint2 = startPoint + QPointF(arrowSize * std::cos(angle + M_PI / 6), arrowSize * std::sin(angle + M_PI / 6));
            arrowPoint3 = (startPoint.x() < endPoint.x() ? startPoint : endPoint);
        } else if (arrowPosition == RIGHT) {
            arrowPoint1 = endPoint - QPointF(arrowSize * std::cos(angle + M_PI / 6), arrowSize * std::sin(angle + M_PI / 6));
            arrowPoint2 = endPoint - QPointF(arrowSize * std::cos(angle - M_PI / 6), arrowSize * std::sin(angle - M_PI / 6));
            arrowPoint3 = (startPoint.x() > endPoint.x() ? startPoint : endPoint);
        }

        // 绘制箭头
        painter.setBrush(Qt::black);
        painter.setPen(Qt::black); // 设置画笔样式为实心
        painter.drawPolygon(QPolygonF() << arrowPoint1 << arrowPoint2 << arrowPoint3);
    }

    // 设置字体的点大小为10
    QFont font;
    int fontSize = 10;
    font.setPointSize(fontSize);
    painter.setFont(font);

    QRect textRect = painter.fontMetrics().boundingRect(labelText);
    int textWidth = textRect.width();
    int textHeight = textRect.height();
    int textX, textY;

    if (startPoint.x() == endPoint.x()) {
        // 垂直线段，将文本显示在中间，并按垂直方向排列每个字符
        textX = startPoint.x() - fontSize*2;
        textY = (startPoint.y() + endPoint.y() - textHeight) / 2;

        for (int i = 0; i < labelText.length(); i++) {
            QString character = labelText.at(i);
            QRect characterRect = painter.fontMetrics().boundingRect(character);
            int characterHeight = characterRect.height();
            int characterY = textY + (i * characterHeight);
            painter.drawText(textX, characterY, character);
        }
    } else if (startPoint.y() == endPoint.y()) {
        // 水平线段，将文本显示在中间
        textX = (startPoint.x() + endPoint.x() - textWidth) / 2;
        textY = startPoint.y()  - 8;
        painter.drawText(textX, textY, labelText);
    } else {
        // 斜线段，将文本显示在线段中点附近
        QPointF midPoint = (startPoint + endPoint) / 2;
        textX = midPoint.x() - textWidth / 2 ;
        textY = midPoint.y() - textHeight / 2;
        painter.drawText(textX, textY, labelText);
    }
}










void OS_View::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // 在窗口右上角绘制exNum的数值

    QPainter painterTitle(this);
    QFont fontTitle;
    fontTitle.setPointSize(22); // 设置字体大小为12点
    painterTitle.setPen(Qt::black);
    painterTitle.setFont(fontTitle);
    painterTitle.drawText(width() /2, 40, QString("第%1周期").arg(exNum));
    //---绘制CPU图形 start----------------------------

    QPoint cpuCenter(biasX + 1050, biasY+50);  // CPU图形圆心坐标
    int radius = 40;    // 圆的半径
    drawCpuGraphics(painter, cpuCenter, radius);
    //---绘制箭头线“进程完成”-------
    drawLine(painter, QPoint(biasX + 1050+radius, biasY+50),QPoint(biasX + 1050+radius+130, biasY+50),"进程完成",true,RIGHT);

    //---绘制箭头线“批量作业”-------
    drawLine(painter, QPoint(biasX+5, biasY+50),QPoint(biasX+70, biasY+50),"批量作业",true,RIGHT);

    //---绘制箭头线“后备队列-->就绪队列”-------
    drawLine(painter, QPoint(biasX+320, biasY+50),QPoint(biasX+432, biasY+50),"",true,RIGHT);
    //---绘制线“交互型作业”-------
    drawLine(painter, QPoint(biasX+250, biasY+120),QPoint(biasX+340, biasY+120),"交互型作业",false);
    drawLine(painter, QPoint(biasX+340, biasY+120),QPoint(biasX+340, biasY+52),"",true,RIGHT);

    //---绘制作业调度---
    // 绘制标注文本
    QString labelText = "作业调度：短作业优先";
    QRect textRect = painter.fontMetrics().boundingRect(labelText);
    int textWidth = textRect.width();
    int textX = biasX+330;
    int textY = biasY-20;
    // 设置字体的点大小为10
    QFont font;
    font.setPointSize(10);
    painter.setFont(font);

    painter.drawText(textX- textWidth/2, textY-5, labelText);
    drawLine(painter, QPoint(biasX+330 , biasY-15),QPoint(biasX+330, biasY+48),"",true,RIGHT);

    //---绘制箭头线“时间片完”-------
    drawLine(painter, QPoint(biasX+350, biasY+30),QPoint(biasX+432, biasY+30),"",true,RIGHT);
    drawLine(painter, QPoint(biasX+350 , biasY-15),QPoint(biasX+350, biasY+30),"",false);
    drawLine(painter, QPoint(biasX+350 , biasY-15),QPoint(biasX+1120, biasY-15),"时间片完",false);
    drawLine(painter, QPoint(biasX+1120 , biasY-15),QPoint(biasX+1120, biasY+30),"",false);
    drawLine(painter, QPoint(biasX+1087, biasY+30),QPoint(biasX+1120, biasY+30),"",false);
    //---绘制箭头线“进程调度”-------
    drawLine(painter, QPoint(biasX+930, biasY+50),QPoint(biasX+1010, biasY+50),"",true,RIGHT);

    //---绘制“CPU下侧的线”----
    drawLine(painter, QPoint(biasX+1087, biasY+70),QPoint(biasX+1120, biasY+70),"",false);
    drawLine(painter, QPoint(biasX+1120 , biasY+70),QPoint(biasX+1120, biasY+480),"",false);

    //---绘制“等待事件”-----
    drawLine(painter, QPoint(biasX+930, biasY+480),QPoint(biasX+1150, biasY+480),"等待事件",true,LEFT);

    //----绘制"事件出现" 左---
    drawLine(painter, QPoint(biasX+350, biasY+70),QPoint(biasX+432, biasY+70),"",true,RIGHT);
    drawLine(painter, QPoint(biasX+350 , biasY+70),QPoint(biasX+350, biasY+490),"事件出现",false);
    drawLine(painter, QPoint(biasX+350, biasY+490),QPoint(biasX+432, biasY+490),"",false);

    //绘制“中级调度”---
    //绘制“中级调度”
    QString labelText2 = "中级调度：先来先服务";
    QRect textRect2 = painter.fontMetrics().boundingRect(labelText2);
    int textWidth2 = textRect2.width();
    int textX2 = biasX+390;
    int textY2 = biasY+160;
    // 设置字体的点大小为10
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(textX2- textWidth2/2 + 30, textY2-5, labelText2);
    drawLine(painter, QPoint(biasX+390, biasY+160),QPoint(biasX+390, biasY+210),"",true,RIGHT);
    drawLine(painter, QPoint(biasX+350, biasY+210),QPoint(biasX+432, biasY+210),"",true,LEFT);


    //---绘制“事件出现” 右---
    drawLine(painter, QPoint(biasX+930, biasY+70),QPoint(biasX+970, biasY+70),"",false);
    drawLine(painter, QPoint(biasX+970 , biasY+70),QPoint(biasX+970, biasY+200),"事件出现",false);
    drawLine(painter, QPoint(biasX+930, biasY+200),QPoint(biasX+970, biasY+200),"",true,LEFT);

    //--绘制““就绪挂起队列”与“阻塞挂起队列”的连线”
    drawLine(painter, QPoint(biasX+930, biasY+220),QPoint(biasX+970, biasY+220),"",true,LEFT);
    drawLine(painter, QPoint(biasX+970 , biasY+220),QPoint(biasX+970, biasY+270),"",false);
    drawLine(painter, QPoint(biasX+400 , biasY+270),QPoint(biasX+970, biasY+270),"",false);
    drawLine(painter, QPoint(biasX+400 , biasY+270),QPoint(biasX+400, biasY+340),"",false);
    drawLine(painter, QPoint(biasX+400 , biasY+340),QPoint(biasX+432, biasY+340),"",false);

     //--绘制““阻塞挂起队列”与“阻塞队列”的连线”--
    drawLine(painter, QPoint(biasX+933, biasY+340),QPoint(biasX+970, biasY+340),"",true,LEFT);
    drawLine(painter, QPoint(biasX+970 , biasY+340),QPoint(biasX+970, biasY+400),"挂起",false);
    drawLine(painter, QPoint(biasX+400 , biasY+400),QPoint(biasX+970, biasY+400),"",false);
    drawLine(painter, QPoint(biasX+400 , biasY+400),QPoint(biasX+400, biasY+460),"",false);
    drawLine(painter, QPoint(biasX+400 , biasY+460),QPoint(biasX+432, biasY+460),"",false);
}
//模拟执行一个cpu_time
void OS_View::VMExect(){



    if(this->mode == MODELEX)
    {

        //---------------------------
        int choice = 1;
        //1.正常执行 2.挂起阻塞队列 3.挂起就绪队列 4.提交新的作业");

        if(incident == SUSPENDBLOCK){
            choice = 2;
        }else if(incident == SUSPENDREADY){
            choice = 3;
        }
        incident = INCIDENTINVALID;//将事件值为非法

        if (choice == ADDNEWUSERJOB) {
            manaulAddJobToUserJob(UserJob);
        }
        run(UserJob, JobQueue, readyQueue, blockQueue, readySuspendedQueue,  blockedSuspendedQueue,choice);
        //---记录执行过程-------------
        recordData(); // 记录数据到文件
    }
    else if(this->mode == MODELPLAY)// 播放模式
    {
        readRecord(exNum);

    }else if(this->mode == MODELINVALID){
        return;
    }
    exNum++;//周期+1



    jobMatrix.nowLen = JobQueueLen; // 后备队列队列长度
    readyMatrix.nowLen =readyQueueLen; // 就绪队列矩队列长度
    blockMatrix.nowLen =blockQueueLen; // 阻塞队列队列长度
    blockedSuspendedMatrix.nowLen =blockedSuspendedQueueLen; // 阻塞挂起队列长度
    readySuspendedMatrix.nowLen =readySuspendedQueueLen; // 就绪挂起队列长度
    if(!readyQueueLen && !JobQueueLen && !blockQueueLen && !blockedSuspendedQueueLen && !readySuspendedQueueLen){
        pauseButtonClicked();
    }

}


void OS_View::recordData()
{
    int cycle = exNum;
    QFile file(recordFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        // 文件打开失败，处理错误
        return;
    }

    QTextStream out(&file);

    // 写入队列长度信息
    out << "Cycle " << cycle << " - Queue Lengths:\n";
    out << "JobQueue length: " << JobQueueLen << "\n";
    out << "readyQueue length: " << readyQueueLen << "\n";
    out << "blockQueue length: " << blockQueueLen << "\n";
    out << "blockedSuspendedQueue length: " << blockedSuspendedQueueLen << "\n";
    out << "readySuspendedQueue length: " << readySuspendedQueueLen << "\n";

    // 写入后备队列数据
    out << "Cycle " << cycle << " - JobQueue:\n";
    for (int i = 0; i < MAX_JobQueue; i++) {
        out << "JCB " << i << ": " << "UserName=" << JobQueue[i].UserName << ", JID=" << JobQueue[i].JID
            << ", taken=" << JobQueue[i].taken << ", time_spent=" << JobQueue[i].time_spent
            << ", io=" << JobQueue[i].io << ", io_time=" << JobQueue[i].io_time
            << ", priority=" << JobQueue[i].priority << "\n";
    }

    // 写入就绪队列数据
    out << "Cycle " << cycle << " - readyQueue:\n";
    for (int i = 0; i < MAX_readyQueue; i++) {
        out << "PCB " << i << ": " << "PID=" << readyQueue[i].PID << ", io=" << readyQueue[i].io
            << ", io_when=" << readyQueue[i].io_when << ", need_time=" << readyQueue[i].need_time
            << ", io_exe=" << readyQueue[i].io_exe << ", io_finish=" << readyQueue[i].io_finish
            << ", suspend_time=" << readyQueue[i].suspend_time << ", wait_time=" << readyQueue[i].wait_time
            << ", priority=" << readyQueue[i].priority << "\n";
    }

    // 写入阻塞队列数据
    out << "Cycle " << cycle << " - blockQueue:\n";
    for (int i = 0; i < MAX_blockQueue; i++) {
        out << "PCB " << i << ": " << "PID=" << blockQueue[i].PID << ", io=" << blockQueue[i].io
            << ", io_when=" << blockQueue[i].io_when << ", need_time=" << blockQueue[i].need_time
            << ", io_exe=" << blockQueue[i].io_exe << ", io_finish=" << blockQueue[i].io_finish
            << ", suspend_time=" << blockQueue[i].suspend_time << ", wait_time=" << blockQueue[i].wait_time
            << ", priority=" << blockQueue[i].priority << "\n";
    }

    // 写入阻塞挂起队列数据
    out << "Cycle " << cycle << " - blockedSuspendedQueue:\n";
    for (int i = 0; i < MAX_blockedSuspendedQueue; i++) {
        out << "PCB " << i << ": " << "PID=" << blockedSuspendedQueue[i].PID << ", io=" << blockedSuspendedQueue[i].io
            << ", io_when=" << blockedSuspendedQueue[i].io_when << ", need_time=" << blockedSuspendedQueue[i].need_time
            << ", io_exe=" << blockedSuspendedQueue[i].io_exe << ", io_finish=" << blockedSuspendedQueue[i].io_finish
            << ", suspend_time=" << blockedSuspendedQueue[i].suspend_time << ", wait_time=" << blockedSuspendedQueue[i].wait_time
            << ", priority=" << blockedSuspendedQueue[i].priority << "\n";
    }

    // 写入就绪挂起队列数据
    out << "Cycle " << cycle << " - readySuspendedQueue:\n";
    for (int i = 0; i < MAX_readySuspendedQueue; i++) {
        out << "PCB " << i << ": " << "PID=" << readySuspendedQueue[i].PID << ", io=" << readySuspendedQueue[i].io
            << ", io_when=" << readySuspendedQueue[i].io_when << ", need_time=" << readySuspendedQueue[i].need_time
            << ", io_exe=" << readySuspendedQueue[i].io_exe << ", io_finish=" << readySuspendedQueue[i].io_finish
            << ", suspend_time=" << readySuspendedQueue[i].suspend_time << ", wait_time=" << readySuspendedQueue[i].wait_time
            << ", priority=" << readySuspendedQueue[i].priority << "\n";
    }

    file.close(); // 关闭文件
}


//从文件中读取记录
void OS_View::readRecord(int exNum)
{

    QString recordFileName = this->selectFile;

    QFile file(recordFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 文件打开失败，处理错误
        return;
    }

    QTextStream in(&file);

    // 寻找对应周期数的数据
    QString targetCycle = "Cycle " + QString::number(exNum) + " -";

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.startsWith(targetCycle)) {
            //加载长度
            if (line.contains("Lengths:")){
                for(int i = 0 ; i < 5; i++){
                    QString lengthLine = in.readLine();
                    int len = lengthLine.mid(lengthLine.indexOf(':')+2).toInt();
                    switch(i){
                    case 0:
                        JobQueueLen = len;
                        break;
                    case 1:
                        readyQueueLen = len;
                        break;
                    case 2:
                        blockQueueLen = len;
                        break;
                    case 3:
                        blockedSuspendedQueueLen = len;
                        break;
                    case 4:
                        readySuspendedQueueLen = len;
                        break;
                    }
                }
            }
            // 加载后备队列数据
            else if (line.contains("JobQueue:")) {
                for (int i = 0; i < MAX_JobQueue; i++) {
                    QString jcbLine = in.readLine();
                    // 解析行中的数据，将其加载到JobQueue数组中
                    QStringList jcbData = jcbLine.split(", ");
                    JobQueue[i].UserName = jcbData[0].mid(jcbData[0].indexOf('=') + 1).toInt();
                    JobQueue[i].JID = jcbData[1].mid(jcbData[1].indexOf('=') + 1).toInt();
                    JobQueue[i].taken = jcbData[2].mid(jcbData[2].indexOf('=') + 1).toInt();
                    JobQueue[i].time_spent = jcbData[3].mid(jcbData[3].indexOf('=') + 1).toInt();
                    JobQueue[i].io = jcbData[4].mid(jcbData[4].indexOf('=') + 1).toInt();
                    JobQueue[i].io_time = jcbData[5].mid(jcbData[5].indexOf('=') + 1).toInt();
                    JobQueue[i].priority = jcbData[6].mid(jcbData[6].indexOf('=') + 1).toInt();
                }
            }
            // 加载就绪队列数据
            else if (line.contains("readyQueue:")) {
                for (int i = 0; i < MAX_readyQueue; i++) {
                    QString pcbLine = in.readLine();
                    // 解析行中的数据，将其加载到readyQueue数组中
                    QStringList pcbData = pcbLine.split(", ");
                    readyQueue[i].PID = pcbData[0].mid(pcbData[0].indexOf('=') + 1).toInt();
                    readyQueue[i].io = pcbData[1].mid(pcbData[1].indexOf('=') + 1).toInt();
                    readyQueue[i].io_when = pcbData[2].mid(pcbData[2].indexOf('=') + 1).toInt();
                    readyQueue[i].need_time = pcbData[3].mid(pcbData[3].indexOf('=') + 1).toInt();
                    readyQueue[i].io_exe = pcbData[4].mid(pcbData[4].indexOf('=') + 1).toInt();
                    readyQueue[i].io_finish = pcbData[5].mid(pcbData[5].indexOf('=') + 1).toInt();
                    readyQueue[i].suspend_time = pcbData[6].mid(pcbData[6].indexOf('=') + 1).toInt();
                    readyQueue[i].wait_time = pcbData[7].mid(pcbData[7].indexOf('=') + 1).toInt();
                    readyQueue[i].priority = pcbData[8].mid(pcbData[8].indexOf('=') + 1).toInt();
                }
            }
            // 加载阻塞队列数据
            else if (line.contains("blockQueue:")) {
                for (int i = 0; i < MAX_blockQueue; i++) {
                    QString pcbLine = in.readLine();
                    // 解析行中的数据，将其加载到blockQueue数组中
                    QStringList pcbData = pcbLine.split(", ");
                    blockQueue[i].PID = pcbData[0].mid(pcbData[0].indexOf('=') + 1).toInt();
                    blockQueue[i].io = pcbData[1].mid(pcbData[1].indexOf('=') + 1).toInt();
                    blockQueue[i].io_when = pcbData[2].mid(pcbData[2].indexOf('=') + 1).toInt();
                    blockQueue[i].need_time = pcbData[3].mid(pcbData[3].indexOf('=') + 1).toInt();
                    blockQueue[i].io_exe = pcbData[4].mid(pcbData[4].indexOf('=') + 1).toInt();
                    blockQueue[i].io_finish = pcbData[5].mid(pcbData[5].indexOf('=') + 1).toInt();
                    blockQueue[i].suspend_time = pcbData[6].mid(pcbData[6].indexOf('=') + 1).toInt();
                    blockQueue[i].wait_time = pcbData[7].mid(pcbData[7].indexOf('=') + 1).toInt();
                    blockQueue[i].priority = pcbData[8].mid(pcbData[8].indexOf('=') + 1).toInt();
                }
            }
            // 加载阻塞挂起队列数据
            else if (line.contains("blockedSuspendedQueue:")) {
                for (int i = 0; i < MAX_blockedSuspendedQueue; i++) {
                    QString pcbLine = in.readLine();
                    // 解析行中的数据，将其加载到blockedSuspendedQueue数组中
                    QStringList pcbData = pcbLine.split(", ");
                    blockedSuspendedQueue[i].PID = pcbData[0].mid(pcbData[0].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].io = pcbData[1].mid(pcbData[1].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].io_when = pcbData[2].mid(pcbData[2].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].need_time = pcbData[3].mid(pcbData[3].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].io_exe = pcbData[4].mid(pcbData[4].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].io_finish = pcbData[5].mid(pcbData[5].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].suspend_time = pcbData[6].mid(pcbData[6].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].wait_time = pcbData[7].mid(pcbData[7].indexOf('=') + 1).toInt();
                    blockedSuspendedQueue[i].priority = pcbData[8].mid(pcbData[8].indexOf('=') + 1).toInt();
                }
            }
            // 加载就绪挂起队列数据
            else if (line.contains("readySuspendedQueue:")) {
                for (int i = 0; i < MAX_readySuspendedQueue; i++) {
                    QString pcbLine = in.readLine();
                    // 解析行中的数据，将其加载到readySuspendedQueue数组中
                    QStringList pcbData = pcbLine.split(", ");
                    readySuspendedQueue[i].PID = pcbData[0].mid(pcbData[0].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].io = pcbData[1].mid(pcbData[1].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].io_when = pcbData[2].mid(pcbData[2].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].need_time = pcbData[3].mid(pcbData[3].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].io_exe = pcbData[4].mid(pcbData[4].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].io_finish = pcbData[5].mid(pcbData[5].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].suspend_time = pcbData[6].mid(pcbData[6].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].wait_time = pcbData[7].mid(pcbData[7].indexOf('=') + 1).toInt();
                    readySuspendedQueue[i].priority = pcbData[8].mid(pcbData[8].indexOf('=') + 1).toInt();
                }
            }

            // 若加载完目标周期的数据，跳出循环
            if (line.startsWith("Cycle") && line.endsWith("readySuspendedQueue:"))
                break;
        }
    }

    file.close(); // 关闭文件
}



OS_View::~OS_View()
{
}

