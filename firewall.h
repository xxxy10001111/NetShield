#ifndef FIREWALL_H
#define FIREWALL_H

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 1
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include "logpage.h"
#include "windivert.h"

#pragma comment(lib, "WinDivert.lib")

using namespace std;

struct Rule {
    string name;
    string description;
    char srcIp[17];
    char dstIp[17];
    int direction;
    UINT16 srcPort;
    UINT16 dstPort;
    string protocol;
    int action;
    int enable;
};

Rule toRule(string name, string description,
            string srcIp, string dstIp,
            int direction, UINT16 srcPort, UINT16 dstPort,
            string protocol, int action, int enable);//转换成Rule
bool checkIP(string IP);//判断IP格式是否合法
bool checkString(string s);//判断string存储的字段格式是否合法

class RuleManager {
private:
    bool match(Rule r1, Rule r2);
public:
    LogManager L;
    vector<Rule> rules,rulesIn,rulesOut;
    void readRule();
    void addRule(Rule r);
    void deleteRule(int tableNum, int no);
    void updateRule(int tableNum, int no, Rule r);
    void listRules(Rule r);
    bool findRule(PWINDIVERT_IPHDR ip_header, UINT8 protocol, PWINDIVERT_ICMPHDR icmp_header, PWINDIVERT_TCPHDR tcp_header, PWINDIVERT_UDPHDR udp_header);
};

#endif // FIREWALL_H
