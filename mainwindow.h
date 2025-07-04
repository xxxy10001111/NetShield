#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QIcon>
#include <filesystem>
#include "firewall.h"
#include "rule.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    RuleManager *RM;
    void showrule();

private slots:
    void pushButton_search_clicked();
    void pushButton_add_clicked();
    void pushButton_log_clicked();
    void onEditButtonClicked(int tableId, int row);
    void onDeleteButtonClicked(int tableId, int row);
    void receiveAddRule(Rule r);
    void receiveEditRule(int tableId, int row, Rule r);
    void receiveswich(bool sw);

private:
    Ui::MainWindow *ui;
    addrule *add;
    addrule *edit;
    logpage *log;
    void addRuleList(QTableWidget* tableWidget, int tableId, Rule r);
};
bool IsAdmin();
#endif // MAINWINDOW_H
