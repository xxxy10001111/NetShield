#include "logpage.h"
#include "ui_logpage.h"

logpage::logpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::logpage)
    ,timer(new QTimer(this))
{
    ui->setupUi(this);

    ui->comboBox->addItem("全部");
    ui->comboBox->addItem("规则");
    ui->comboBox->addItem("允许");
    ui->comboBox->addItem("拦截");
    ui->comboBox->setCurrentIndex(0);

    update(ui->comboBox->currentIndex());
    // 设置定时器的间隔为 1000 毫秒（1 秒）
    timer->setInterval(1000);

    // 连接定时器的 timeout 信号到 refreshLog 槽函数
    connect(timer, &QTimer::timeout, this, &logpage::refreshLog);

    // 启动定时器
    timer->start();
}

logpage::~logpage()
{
    delete ui;
}

void LogManager::wrong(int n)
{
    if (sw) {
        Log l;
        ofstream fLog("Data/Log.txt", ios::app);
        if (!fLog.is_open())
            return;
        time_t t = time(0);
        tm* now = localtime(&t);
        l.time = to_string(now->tm_year + 1900) + "." + to_string(now->tm_mon + 1) + '.' + to_string(now->tm_mday) + " " +
             to_string(now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
        l.action = " Fail";

        if (n == 1)
            l.subject = " Add Rule.";
        else if (n == 2)
            l.subject = " Delete Rule.";
        else if (n == 3)
            l.subject = " Update Rule.";
        fLog << l.time + l.action + l.subject << endl;
        fLog.close();
    }

    return;
}

void LogManager::write(int n, string sub)
{
    if (sw) {
        Log l;
        ofstream fLog("Data/Log.txt", ios::app);
        if (!fLog.is_open())
            return;
        time_t t = time(0);
        tm* now = localtime(&t);
        l.time = to_string(now->tm_year + 1900) + "." + to_string(now->tm_mon + 1) + '.' + to_string(now->tm_mday) + " " +
                to_string(now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
        if (n > 0 && n < 4)
            l.action = " Success";

        if (n == 1)
            l.subject = " Add Rule:" + sub + ".";
        else if (n == 2)
            l.subject = " Delete Rule:" + sub + ".";
        else if (n == 3)
            l.subject = " Update Rule:" + sub + ".";
        else if (n == 4) {
            l.action = " Block ";
            l.subject = sub + ".";
        }
        else if (n == 5) {
            l.action = " Allow ";
            l.subject = sub + ".";
        }
        fLog << l.time + l.action + l.subject << endl;
        fLog.close();
    }
    return;
}

void logpage::update(int f)
{
    ui->listWidget->clear();
    //qDebug()<<"选择："<<f;
    ifstream logFile("Data/Log.txt");
    //显示Log
    string line;
    while (getline(logFile, line)) {
        if (f==1 && (line.find("Rule") != string::npos)) {
            ui->listWidget->addItem(QString::fromStdString(line));
        }
        else if (f == 2 && line.find("Allow") != string::npos) {
            ui->listWidget->addItem(QString::fromStdString(line));
        }
        else if (f == 3 && line.find("Block") != string::npos) {
            ui->listWidget->addItem(QString::fromStdString(line));
        }
        else if(f == 0){
            //qDebug()<<f<<line;
            ui->listWidget->addItem(QString::fromStdString(line));
        }
    }

    logFile.close();
    return;
}

void logpage::on_comboBox_currentIndexChanged(int index)
{
    update(index);
}

void logpage::refreshLog()
{
    update(ui->comboBox->currentIndex());
}



void logpage::on_pushButton_clicked()
{
    s = !s;
    settext();
    emit sendswitch(s);
}

void logpage::settext()
{
    if(s)
        ui->pushButton->setText("关闭日志");
    else
        ui->pushButton->setText("打开日志");
}


