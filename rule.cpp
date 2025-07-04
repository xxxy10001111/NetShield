#include "rule.h"
#include "ui_rule.h"

addrule::addrule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addrule)
{
    ui->setupUi(this);

    ui->spinBox_srcPort->setRange(0,65535);
    ui->spinBox_dstPort->setRange(0,65535);

    ui->comboBox_pro->addItem("TCP");
    ui->comboBox_pro->addItem("UDP");
    ui->comboBox_pro->addItem("ICMP");


    ui->comboBox_dir->addItem("入站流量",0);
    ui->comboBox_dir->addItem("出站流量",1);

    ui->comboBox_act->addItem("拦截",0);
    ui->comboBox_act->addItem("允许",1);
}

addrule::~addrule()
{
    delete ui;
}

void addrule::setButtonText(bool add_edit)
{
    a_e = add_edit;
    if(a_e)
        ui->pushButton->setText("确认修改规则");
    else
        ui->pushButton->setText("确认新增规则");
}

void addrule::clearData()
{
    ui->lineEdit_name->clear();
    ui->textEdit->clear();
    ui->comboBox_pro->setCurrentIndex(0);
    ui->comboBox_dir->setCurrentIndex(0);
    ui->comboBox_act->setCurrentIndex(0);
    ui->lineEdit_srcIP->clear();
    ui->lineEdit_dstIP->clear();
    ui->spinBox_srcPort->clear();
    ui->spinBox_srcPort->setValue(0);
    ui->spinBox_dstPort->clear();
    ui->spinBox_dstPort->setValue(0);
    ui->radioButton->setChecked(0);
}

void addrule::setEditData(int Id, int r, Rule before)
{
    ui->lineEdit_name->setText(QString::fromStdString(before.name));
    ui->textEdit->setText(QString::fromStdString(before.description));
    int index = ui->comboBox_pro->findData(QString::fromStdString(before.protocol)); // 找到对应的索引
    if (index != -1) {
        ui->comboBox_pro->setCurrentIndex(index); // 设置当前项
        qDebug()<<index;
    }
    ui->comboBox_pro->show();
    ui->lineEdit_srcIP->setText(QString::fromStdString(before.srcIp));
    ui->lineEdit_dstIP->setText(QString::fromStdString(before.dstIp));
    ui->spinBox_srcPort->setValue(before.srcPort);
    ui->spinBox_dstPort->setValue(before.dstPort);
    ui->comboBox_dir->setCurrentIndex(before.direction);
    ui->comboBox_act->setCurrentIndex(before.action);
    ui->radioButton->setChecked(before.enable);
    tableId = Id;
    row = r;
}

void addrule::on_pushButton_clicked()
{
    //string name, description, srcIP, dstIP, protocol;
    //int direction, action, enable,
    if(ui->lineEdit_name->text().isEmpty()){
        QMessageBox::warning(this, "错误", "未输入规则名！");
        return;
    }
    string name = ui->lineEdit_name->text().toStdString();
    if(!checkString(name)){
        QMessageBox::warning(this, "错误", "规则名不允许有空格！");
        return;
    }
    string description;
    if(ui->textEdit->toPlainText().isEmpty())
        description = "无";
    else
        description = ui->textEdit->toPlainText().toStdString();
    if(!checkString(description)){
        QMessageBox::warning(this, "错误", "规则描述不允许有空格、换行和回车！");
        return;
    }

    string srcIP,dstIP;
    if(ui->lineEdit_srcIP->text().isEmpty())
        srcIP = "0.0.0.0";
    else
        srcIP = ui->lineEdit_srcIP->text().toStdString();
    if(!checkIP(srcIP)){
        QMessageBox::warning(this, "错误", "源IP输入格式错误！\n【格式为：X.X.X.X (X范围0~255)】");
        return;
    }
    if(ui->lineEdit_dstIP->text().isEmpty())
        dstIP = "0.0.0.0";
    else
        dstIP = ui->lineEdit_dstIP->text().toStdString();
    if(!checkIP(dstIP)){
        QMessageBox::warning(this, "错误", "目标IP输入格式错误！\n【格式为：X.X.X.X (X范围0~255)】");
        return;
    }
    int srcPort = ui->spinBox_srcPort->value();
    int dstPort = ui->spinBox_dstPort->value();
    string protocol = ui->comboBox_pro->currentText().toStdString();
    int direction = ui->comboBox_dir->currentIndex();
    int action = ui->comboBox_act->currentIndex();
    int enable;
    if (ui->radioButton->isChecked())
        enable = 1;
    else
        enable = 0;

    Rule r =  toRule(name,description,srcIP,dstIP,direction,srcPort,dstPort,protocol,action,enable);
    if(a_e)
        emit sendEditRule(tableId, row, r);
    else
        emit sendAddRule(r);
    this->close();
}

