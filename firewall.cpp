#include "firewall.h"
#include <QDebug>

Rule toRule(string name, string description,
                         string srcIp, string dstIp,
                         int direction, UINT16 srcPort, UINT16 dstPort,
                         string protocol, int action, int enable)
{
    Rule r;
    r.name = name;
    r.description = description;
    strcpy(r.srcIp, srcIp.c_str());
    strcpy(r.dstIp, dstIp.c_str());
    r.direction = direction;
    r.srcPort = srcPort;
    r.dstPort = dstPort;
    r.protocol = protocol;
    r.action = action;
    r.enable = enable;

    return r;
}

bool checkIP(string IP)
{
    int dots = 0;
    int num = 0;
    string segment;
    stringstream ss(IP);

    while (getline(ss, segment, '.')) {
        if (++dots > 4)
            return false;
        if (segment.empty() || segment.length() > 3)
            return false;
        for (int i = 0; i < segment.size(); ++i) {
            if (!isdigit(segment[i]))
                return false;
        }
        num = stoi(segment);
        if (num < 0 || num > 255)
            return false;
        if (segment.length() > 1 && segment[0] == '0')
            return false;
    }

    return dots == 4;
}

bool checkString(string s)
{
     if (s.find(" ") != string::npos || s.find("\n") != string::npos || s.find("\r\n") != string::npos)
        return false;
    return true;
}

void RuleManager::readRule()
{
    ifstream frule("Data/Rules.txt");
    if (!frule.is_open()) {
        return;
    }

    string name, description, srcIp, dstIp, d,protocol,a,e;
    int direction, srcPort, dstPort, action, enable;
    while (frule >> name >> description >> srcIp>> dstIp >> d >> srcPort>> dstPort>> protocol>> a >> e) {
        if (d == "入站")
            direction = 0;
        else
            direction = 1;
        if (a == "拦截")
            action = 0;
        else
            action = 1;
        if (e == "禁用")
            enable = 0;
        else
            enable = 1;
        rules.push_back(toRule(name, description, srcIp, dstIp, direction, srcPort, dstPort, protocol, action, enable));
    }
    frule.close();
    return;
}

void RuleManager::addRule(Rule r)
{
    ofstream frule("Data/Rules.txt", ios::app);
    if (!frule.is_open()) {
        L.wrong(1);
        return;
    }

    frule << r.name + " " + r.description + " " + r.srcIp + " " + r.dstIp + " " +
                 ((r.direction == 0) ? "入站" : "出站") + " " +
                 to_string(r.srcPort) + " " + to_string(r.dstPort) + " " + r.protocol + " " +
                 ((r.action == 0) ? "拦截" : "允许") + " " + ((r.enable == 0) ? "禁用" : "启用") << endl;

    frule.close();
    rules.push_back(r);
    L.write(1, r.name);

    return;
}

void RuleManager::deleteRule(int tableNum, int no)
{
    ifstream ifrule("Data/Rules.txt");
    if (!ifrule.is_open()) {
        L.wrong(2);
        return;
    }
    vector<string> lines;
    string line;
    int current = 0, t=0, c;
    while (getline(ifrule, line)) {
        t++;
        if ((tableNum == 0 && line.find("入站") != string::npos) ||
            (tableNum == 1 && line.find("出站") != string::npos)) {
            current++;
            if (current == no){
                c=t;
                continue;
            }
        }
        lines.push_back(line);
    }
    ifrule.close();

    ofstream ofrule("Data/Rules.txt");
    if (!ofrule) {
        L.wrong(2);
        return;
    }
    for (int i = 0; i < lines.size();++i) {
        ofrule << lines[i] << endl;
    }
    ofrule.close();
    L.write(2, rules[c - 1].name);
    rules.erase(rules.begin() + c - 1);

    return;
}

void RuleManager::updateRule(int tableNum, int no, Rule r)
{
    ifstream ifrule("Data/Rules.txt");
    if (!ifrule.is_open()) {
        L.wrong(3);
        return;
    }
    vector<string> lines;
    string line;
    int current = 0, t=0, c;
    while (getline(ifrule, line)) {
        t++;
        if ((tableNum == 0 && line.find("入站") != string::npos) ||
            (tableNum == 1 && line.find("出站") != string::npos)) {
            current++;
            if (current == no) {
                line = r.name + " " + r.description + " " + r.srcIp + " " + r.dstIp + " " +
                       ((r.direction == 0) ? "入站" : "出站") + " " +
                       to_string(r.srcPort) + " " + to_string(r.dstPort) + " " + r.protocol + " " +
                       ((r.action == 0) ? "拦截" : "允许") + " " + ((r.enable == 0) ? "禁用" : "启用");
                c = t;
            }
        }
        lines.push_back(line);
    }
    ifrule.close();

    ofstream ofrule("Data/Rules.txt");
    if (!ofrule) {
        L.wrong(3);
        return;
    }
    for (int i = 0; i < lines.size(); ++i) {
        ofrule << lines[i] << endl;
    }
    ofrule.close();

    rules[c - 1] = r;
    L.write(3, r.name);

    return;
}

void RuleManager::listRules(Rule r)
{
    rulesIn.clear();
    rulesOut.clear();
    for (int i = 0; i < rules.size(); ++i) {
        if (match(rules[i], r)) {
            if (!rules[i].direction)
                rulesIn.push_back(rules[i]);
            else
                rulesOut.push_back(rules[i]);
        }
    }
    return;
}

bool RuleManager::match(Rule r1, Rule r2)
{
    if (r2.name.size() > 0 && r1.name.find(r2.name) == string::npos) {
        return false;
    }
    if (strcmp(r2.srcIp, "0.0.0.0") && strcmp(r1.srcIp, r2.srcIp)) {
        return false;
    }
    if (strcmp(r2.dstIp, "0.0.0.0") && strcmp(r1.dstIp, r2.dstIp)) {
        return false;
    }
    if (r2.srcPort != 0 && r1.srcPort != r2.srcPort) {
        return false;
    }
    if (r2.dstPort != 0 && r1.dstPort != r2.dstPort) {
        return false;
    }
    if (r2.protocol.size() > 0 && r1.protocol.find(r2.protocol) == string::npos) {
        return false;
    }
    if (r2.action != 2 && r1.action != r2.action) {
        return false;
    }
    if (r2.enable != 2 && r1.enable != r2.enable) {
        return false;
    }
    return true;
}

bool RuleManager::findRule(PWINDIVERT_IPHDR ip_header, UINT8 protocol, PWINDIVERT_ICMPHDR icmp_header, PWINDIVERT_TCPHDR tcp_header, PWINDIVERT_UDPHDR udp_header)
{
    string msg;
    char sip[16],dip[16];
    WinDivertHelperFormatIPv4Address(WinDivertHelperNtohl(ip_header->SrcAddr), sip, 16);
    WinDivertHelperFormatIPv4Address(WinDivertHelperNtohl(ip_header->DstAddr), dip, 16);

    for (int i = 0; i < rules.size(); ++i) {
        if (!rules[i].enable)
            continue;
        if (rules[i].action)
            continue;
        if (strcmp(rules[i].srcIp, "0.0.0.0") && strcmp(rules[i].srcIp, sip))
            continue;

        if (strcmp(rules[i].dstIp, "0.0.0.0") && strcmp(rules[i].dstIp, dip))
            continue;

        if (protocol == 1) {
            if (rules[i].protocol != "ICMP")
                continue;
            if (icmp_header->Type == 8 && rules[i].direction || icmp_header->Type == 0 && !rules[i].direction) {
                msg = "ICMP ";
                msg += sip;
                msg += " ";
                msg += dip;
                L.write(4, msg);
                return true;
            }
        }
        else if (protocol == 6) {
            if (rules[i].protocol != "TCP")
                continue;
            if ((tcp_header->SrcPort == rules[i].srcPort || !rules[i].srcPort) && (tcp_header->DstPort == rules[i].dstPort || !rules[i].dstPort)) {
                msg = "TCP ";
                msg += sip;
                msg += " ";
                msg += dip;
                msg += " " + to_string(tcp_header->SrcPort) + " " + to_string(tcp_header->DstPort);
                L.write(4, msg);
                return true;
            }
        }
        else if (protocol == 17) {
            if (rules[i].protocol != "UDP")
                continue;
            if ((udp_header->SrcPort == rules[i].srcPort || !rules[i].srcPort) && (udp_header->DstPort == rules[i].dstPort || !rules[i].dstPort)) {
                msg = "UDP ";
                msg += sip;
                msg += " ";
                msg += dip;
                msg += " " + to_string(udp_header->SrcPort) + " " + to_string(udp_header->DstPort);
                L.write(4, msg);
                return true;
            }
        }
    }
    if (protocol == 1) {
        msg = "ICMP ";
        msg += sip;
        msg += " ";
        msg += dip;
        L.write(5, msg);
    }
    else if (protocol == 6) {
        msg = "TCP ";
        msg += sip;
        msg += " ";
        msg += dip;
        msg += " " + to_string(tcp_header->SrcPort) + " " + to_string(tcp_header->DstPort);
        L.write(5, msg);
    }
    else if (protocol == 17) {
        msg = "UDP ";
        msg += sip;
        msg += " ";
        msg += dip;
        msg += " " + to_string(udp_header->SrcPort) + " " + to_string(udp_header->DstPort);
        L.write(5, msg);
    }
    return false;
}
