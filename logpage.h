#ifndef LOGPAGE_H
#define LOGPAGE_H

#include <QWidget>
#include <QMessageBox>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

namespace Ui {
class logpage;
}
struct Log {
    string time;
    string action;
    string subject;
};

class LogManager {
public:
    bool sw = 0;
    void wrong(int n);
    void write(int n, string sub);
};

class logpage : public QWidget
{
    Q_OBJECT

public:
    explicit logpage(QWidget *parent = nullptr);
    ~logpage();
    void update(int f);
    bool s;
    void settext();

signals:
    void sendswitch(bool sw);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void refreshLog();

private:
    Ui::logpage *ui;
    QTimer *timer;
};

#endif // LOGPAGE_H
