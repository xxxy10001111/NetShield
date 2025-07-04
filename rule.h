#ifndef RULE_H
#define RULE_H

#include <QWidget>
#include <QMessageBox>
#include <regex>
#include <sstream>
#include "firewall.h"

namespace Ui {
class addrule;
}

class addrule : public QWidget
{
    Q_OBJECT

public:
    explicit addrule(QWidget *parent = nullptr);
    ~addrule();
    void setButtonText(bool add_edit);
    void setEditData(int Id, int r, Rule before);
    void clearData();

signals:
    void sendAddRule(Rule r);
    void sendEditRule(int tableId, int row,Rule r);

private slots:
    void on_pushButton_clicked();

private:
    Ui::addrule *ui;
    bool a_e;
    int tableId, row;
};

#endif // RULE_H
