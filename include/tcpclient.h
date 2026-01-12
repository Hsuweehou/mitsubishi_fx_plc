#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <WinSock2.h>
#include <string>

using namespace std;
const int PORT = 4997;
#define MaxBufSize 2048
#define _CRT_SECURE_NO_WARINGS

//读写命令
enum COMMAND {
    READ,
    WRITE
};
/*
**位，字命令区分。位=16位为单位读写；字=1字为单位读写
**与请求长度有关：位的size*2 = 字的size
*/
enum SUB_COMMAND {
    SUB_BIT,
    SUB_WORD
};

/*
**不同的寄存器类型区
**读取：M(位类型),X(位类型),Y(位类型),D(字类型)
**写入：M,D
*/
enum TYPE {
    XType,
    YType,
    MType,
    DType
};

class TcpClient
{
public:
    TcpClient();

public:
    int slot_Connect(string ip,int port);
    int ReadD(int address,int size,void *pData);
    int ReadM(int address,int size,void *pData);
    int ReadX(int address,int size,void *pData);
    int ReadY(int address,int size,void *pData);

    int WriteD(int address,int size,void *pData);
    int WriteM(int address,int size,void *pData);

    char* build(int address,int size,int command,int sub_command,int type);

    int ResDataLen(char* data);
    int ResError(char* data);

    void ReConnect(string ip, int port);

    int ConnectStatus();

    size_t customStrlen(const char* data);

private:
    SOCKET		SocketClient;
    SOCKADDR_IN	ClientAddr;				//???socket?????????????????
    int nNum = 0;
    int nConnected = 1;
    string m_ip_in = "127.0.0.1";
    int m_port_in = 4999;
};

#endif
