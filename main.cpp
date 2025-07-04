#include "mainwindow.h"

#include <QApplication>
#include <thread>

// 全局变量，用于控制循环
volatile bool running = true;

// 信号处理函数，用于捕获程序退出信号
void handleExit() {
    running = false; // 设置标志变量为 false
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!IsAdmin()) {
        QMessageBox::critical(nullptr, "权限不足", "您没有管理员权限，无法访问此程序。");
        return -1;
    }
    if(!filesystem::exists("Data")){
        if(!filesystem::create_directory("Data"))
            return -1;
    }
    MainWindow w;
    w.RM->readRule();
    HANDLE handle = WinDivertOpen("true", WINDIVERT_LAYER_NETWORK, 0, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to open WinDivert handle\n";
        return -1;
    }

    qDebug() << "Firewall running...\n";

    char packet[65535] = {};
    UINT packetLen = 0;
    WINDIVERT_ADDRESS addr;

    // 连接退出信号
    QObject::connect(&a, &QApplication::aboutToQuit, handleExit);

    // 启动一个新线程来处理数据包拦截
    std::thread packetThread([&]() {
        while (running) {
            if (!WinDivertRecv(handle, packet, sizeof(packet), &packetLen, &addr)) {
                DWORD error = GetLastError();
                qDebug() << "Failed to receive packet, error code:" << error;
                running = false; // 退出循环
                continue;
            }

            PWINDIVERT_IPHDR ip_header = nullptr;
            UINT8 protocol = 0;
            PWINDIVERT_ICMPHDR icmp_header = nullptr;
            PWINDIVERT_TCPHDR tcp_header = nullptr;
            PWINDIVERT_UDPHDR udp_header = nullptr;

            if (!WinDivertHelperParsePacket(packet, packetLen,
                                            &ip_header, nullptr, &protocol,
                                            &icmp_header, nullptr,
                                            &tcp_header, &udp_header,
                                            nullptr, nullptr, nullptr, nullptr)) {
                qDebug() << "Failed to parse packet";
                continue;
            }

            if (!ip_header || (!icmp_header && !tcp_header && !udp_header)) {
                if (!WinDivertSend(handle, packet, packetLen, nullptr, &addr)) {
                    qDebug() << "Failed to send packet";
                }
                continue;
            }

            // 判断
            if (w.RM->findRule(ip_header, protocol, icmp_header, tcp_header, udp_header)) {
                continue;
            }

            if (!WinDivertSend(handle, packet, packetLen, nullptr, &addr)) {
                qDebug() << "Failed to send packet";
            }
        }

        WinDivertClose(handle);
    });

    w.setWindowTitle("NetShield");
    w.show();
    //初始化表格
    w.showrule();

    int result = a.exec(); // 运行主事件循环

    // 确保线程退出
    packetThread.join();

    return result;
}
