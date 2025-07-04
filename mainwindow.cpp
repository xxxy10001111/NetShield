#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RM = new RuleManager;
    add = new addrule;
    edit = new addrule;
    log = new logpage;

    ui->spinBox_srcPort->setRange(0,65535);
    ui->spinBox_dstPort->setRange(0,65535);
    ui->spinBox_srcPort->setValue(0);
    ui->spinBox_dstPort->setValue(0);

    ui->comboBox_pro->addItem("<空>");
    ui->comboBox_pro->addItem("TCP");
    ui->comboBox_pro->addItem("UDP");
    ui->comboBox_pro->addItem("ICMP");

    ui->comboBox_act->addItem("拦截");
    ui->comboBox_act->addItem("允许");
    ui->comboBox_act->addItem("<空>");

    ui->comboBox_en->addItem("禁用");
    ui->comboBox_en->addItem("启用");
    ui->comboBox_en->addItem("<空>");

    ui->comboBox_act->setCurrentIndex(2);
    ui->comboBox_en->setCurrentIndex(2);

    //增删改查
    connect(ui->pushButton_search,SIGNAL(clicked()),this,SLOT(pushButton_search_clicked()));//查询
    connect(ui->pushButton_add,SIGNAL(clicked()),this,SLOT(pushButton_add_clicked()));//新增
    connect(ui->pushButton_log,SIGNAL(clicked()),this,SLOT(pushButton_log_clicked()));//查看日志

    connect(add,SIGNAL(sendAddRule(Rule)),this,SLOT(receiveAddRule(Rule)));
    connect(edit,SIGNAL(sendEditRule(int,int,Rule)),this,SLOT(receiveEditRule(int,int,Rule)));
    connect(log,SIGNAL(sendswitch(bool)),this,SLOT(receiveswich(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButton_search_clicked()//按下查询规则按钮
{
    showrule();
}

void MainWindow::pushButton_add_clicked()//按下新增规则按钮
{
    add->setWindowTitle("新增规则");
    add->setButtonText(0);
    add->clearData();
    add->show();
}

void MainWindow::receiveAddRule(Rule r)
{
    RM->addRule(r);
    showrule();
}

void MainWindow::onEditButtonClicked(int tableId, int row)
{
    qDebug()<<"编辑" << QString("编辑表 %1 的第 %2 行").arg(tableId).arg(row);
    edit->setWindowTitle("编辑规则");
    edit->setButtonText(1);
    if(tableId == 0)
        edit->setEditData(tableId, row, RM->rulesIn[row]);
    else
        edit->setEditData(tableId, row, RM->rulesOut[row]);
    edit->show();
}

void MainWindow::receiveEditRule(int tableId, int row, Rule r)
{
    RM->updateRule(tableId, row+1, r);
    showrule();
}

void MainWindow::onDeleteButtonClicked(int tableId, int row)
{
    qDebug()<<"删除" << QString("删除表 %1 的第 %2 行").arg(tableId).arg(row);
    RM->deleteRule(tableId, row+1);
    showrule();
}

void MainWindow::addRuleList(QTableWidget* tableWidget, int tableId, Rule r)
{
    int rowCount = tableWidget->rowCount();
    tableWidget->insertRow(rowCount); // 在表格末尾插入一行

    // 设置表格内容
    tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::fromStdString(r.name)));
    tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::fromStdString(r.description)));

    tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::fromStdString(r.protocol)));
    tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::fromStdString(r.srcIp)));
    tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(r.srcPort)));
    tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::fromStdString(r.dstIp)));
    tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QString::number(r.dstPort)));

    if (r.action == 0)
        tableWidget->setItem(rowCount, 7, new QTableWidgetItem("拦截"));
    else if (r.action == 1)
        tableWidget->setItem(rowCount, 7, new QTableWidgetItem("允许"));

    if (r.enable == 0)
        tableWidget->setItem(rowCount, 8, new QTableWidgetItem("禁用"));
    else if(r.enable == 1)
        tableWidget->setItem(rowCount, 8, new QTableWidgetItem("启用"));
    // 创建编辑按钮
    QPushButton *editButton = new QPushButton(this);
    editButton->setIcon(QIcon(":/new/icons/static/edit.png"));
    tableWidget->setCellWidget(rowCount, 9, editButton);
    editButton->setStyleSheet("QPushButton { background-color: white; }");

    // 创建删除按钮
    QPushButton *deleteButton = new QPushButton(this);
    deleteButton->setIcon(QIcon(":/new/icons/static/delete.png"));
    tableWidget->setCellWidget(rowCount, 10, deleteButton);
    deleteButton->setStyleSheet("QPushButton { background-color: white; }");

    // 连接编辑按钮的信号到槽函数
    connect(editButton, &QPushButton::clicked, [this, tableId, rowCount, editButton, tableWidget]() {
        onEditButtonClicked(tableId, rowCount);
        editButton->setDown(false); // 恢复按钮状态
        tableWidget->clearSelection();
    });

    // 连接删除按钮的信号到槽函数
    connect(deleteButton, &QPushButton::clicked, [this, tableId, rowCount, deleteButton, tableWidget]() {
        onDeleteButtonClicked(tableId, rowCount);
        deleteButton->setDown(false); // 恢复按钮状态
        tableWidget->clearSelection();
    });
}

void MainWindow::showrule()//显示出入站规则
{
    string n,sip,dip,pro;
    QString name = ui->lineEdit_name->text();
    if (name.isEmpty()) {
        n = "";
        qDebug() << "name输入框为空";
    } else {
        n = name.toStdString();
        qDebug() << "name输入框不为空";
    }
    QString src_IP =ui->lineEdit_srcIP->text();
    if (src_IP.isEmpty())
        sip = "0.0.0.0";
    else
        sip = src_IP.toStdString();
    QString dst_IP =ui->lineEdit_dstIP->text();
    if (dst_IP.isEmpty())
        dip = "0.0.0.0";
    else
        dip = dst_IP.toStdString();
    qDebug()<<"src_ip:"<<QString::fromStdString(sip);
    qDebug()<<"dst_ip:"<<QString::fromStdString(dip);
    //判断ip正确性
    if(!checkIP(sip)){
        QMessageBox::warning(this, "错误", "源IP输入格式错误！\n【格式为：X.X.X.X (X范围0~255)】");
        return;
    }
    if(!checkIP(dip)){
        QMessageBox::warning(this, "错误", "目标IP输入格式错误！\n【格式为：X.X.X.X (X范围0~255)】");
        return;
    }
    int src_port = ui->spinBox_srcPort->value();
    int dst_port = ui->spinBox_dstPort->value();
    qDebug()<<"sre_port:"<<src_port;
    qDebug()<<"dst_port:"<<dst_port;

    int protocol = ui->comboBox_pro->currentIndex();
    //qDebug()<<protocol;
    if(protocol==0)
        pro = "";
    else
        pro = ui->comboBox_pro->currentText().toStdString();
    qDebug()<<"protocol:"<<QString::fromStdString(pro);

    int act = ui->comboBox_act->currentIndex();
    int en = ui->comboBox_en->currentIndex();
    qDebug()<<"act:"<<act<<"en:"<<en;

    Rule r = toRule(n,"",sip,dip,-1,src_port,dst_port,pro,act,en);
    RM->listRules(r);

    QStringList headers;
    headers << "规则名" << "规则描述" << "协议" << "源IP" << "源端口" << "目标IP" << "目标端口" << "动作" << "状态" <<"编辑" << "删除";
    ui->tableWidget_in->clear();
    ui->tableWidget_in->setRowCount(0);
    ui->tableWidget_in->setColumnCount(11);
    ui->tableWidget_in->setHorizontalHeaderLabels(headers);
    ui->tableWidget_in->setColumnWidth(9,65);
    ui->tableWidget_in->setColumnWidth(10,65);
    ui->tableWidget_out->clear();
    ui->tableWidget_out->setRowCount(0);
    ui->tableWidget_out->setColumnCount(11);
    ui->tableWidget_out->setHorizontalHeaderLabels(headers);
    ui->tableWidget_out->setColumnWidth(9,65);
    ui->tableWidget_out->setColumnWidth(10,65);

    qDebug()<<RM->rulesIn.size();
    qDebug()<<RM->rulesOut.size();

    for(int i=0;i<RM->rulesIn.size();i++){
        addRuleList(ui->tableWidget_in,0,RM->rulesIn[i]);
    }
    for(int i=0;i<RM->rulesOut.size();i++){
        addRuleList(ui->tableWidget_out,1,RM->rulesOut[i]);
    }

    ui->tableWidget_in->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_out->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::pushButton_log_clicked()
{
    qDebug()<<"查看日志";
    log->setWindowTitle("查看日志");
    log->s = RM->L.sw;
    log->settext();
    log->show();
}

void MainWindow::receiveswich(bool sw)
{
    RM->L.sw = sw;
}

//检测管理员权限
bool IsAdmin() {
    BOOL f = FALSE;
    PSID adminGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(
            &NtAuthority, 2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &adminGroup)) {

        CheckTokenMembership(NULL, adminGroup, &f);
        FreeSid(adminGroup);
    }

    return f;
}
