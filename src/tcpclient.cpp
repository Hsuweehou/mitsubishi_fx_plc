#include "../include/tcpclient.h"
#include <cstdio>
#include<iostream>
#include<string>
#include<WinSock2.h>
#include <cstring>
#include <WS2tcpip.h>

//告诉编译器链接Winsock库
#pragma  comment(lib,"ws2_32.lib")

TcpClient::TcpClient()
{

}

int TcpClient::slot_Connect(string ip, int port)
{
    //保存IP消息
    m_ip_in = ip;
    m_port_in = port;
    WSADATA wsd;//创建一个结构体变量，用于存储关于Winsock库的信息
    int init_result = WSAStartup(MAKEWORD(2, 2), &wsd);//初始化Winsock库，指定版本号2.2
    if (init_result != 0)
    {
        std::cout << "WSAStartup failed: " << init_result << std::endl; //输出错误信息并退出程序
        return 1;
    }
    SocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//TCP套接字
    if (SocketClient == INVALID_SOCKET)
    {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl; //输出错误信息并退出程序
        WSACleanup(); //清除Winsock库
        return 1;
    }
    ClientAddr.sin_family = AF_INET;//指定地址族为IPv4
   // in_addr addr;
   //将字符串类型的IP地址转换为二进制网络字节序的IP地址，并存储在结构体中
    inet_pton(AF_INET, ip.c_str(), &ClientAddr.sin_addr.S_un.S_addr);//ClientAddr.sin_addr.S_un.S_addr
    //将端口号从主机字节序转换为网络字节序，并存储在结构体中
    ClientAddr.sin_port = htons(port);
    int n = 0;
    n = connect(SocketClient, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr));
    if (n == SOCKET_ERROR) {
        std::cout << "connect fail\n";
        nConnected = 1;
        closesocket(SocketClient);
        WSACleanup();
        return 1;
    }
    nConnected = 0;
    nNum = 0;
    return 0;
}

int TcpClient::ReadD(int address, int size, void *pData)
{
    int nAllLen = 9;
    char* data = build(address,size,READ,SUB_WORD,DType);
    std::unique_ptr <char[]> data_ptr(data);
    int nLen = 21;
    //char buf[data.size()];
    //memcpy(buf,data.data(),data.size());
    int nReg;
    nReg = send(SocketClient,data_ptr.get(), nLen, 0);
    if (nReg < 0) {
        nConnected = 1;
//        ReConnect(m_ip_in, m_port_in);
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        std::unique_ptr <char[]> revData(new char[nReg]);
        //revData.resize(nReg);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        memcpy(pData,revData.get()+11,nLen-2);
        return 0;
    } else {
        return 1;
    }
    return 0;
}

int TcpClient::ReadM(int address, int size, void *pData)
{
    int nAllLen = 9;
    int sizeM = size;
    if(sizeM <= 2) {
        sizeM = 1;
    }else {
        if(sizeM%2 == 0)  {
            sizeM = sizeM/2;
        }else {
            sizeM = (sizeM + 1)/2;
        }
    }
    char* data = build(address,size,READ,SUB_BIT,MType);
    std::unique_ptr <char[]> data_ptr(data);
    int nLen = 21;
    //char buf[data.size()];
    //memcpy(buf,data.data(),data.size());
    int nReg = 0;
    nReg = send(SocketClient, data_ptr.get(), nLen,0);
    if (nReg < 0) {

//        ReConnect(m_ip_in, m_port_in);
        nConnected = 1;
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        //revData.resize(nReg);
        // char* revData = new char[nReg];
        std::unique_ptr <char[]> revData(new char[nReg]);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        // std::unique_ptr <uint8_t> dataTemp(new uint8_t[sizeM]);
        // std::unique_ptr <uint8_t> dataM(new uint8_t[sizeM]);
        std::shared_ptr <uint8_t> dataTemp(new uint8_t[sizeM], std::default_delete<uint8_t[]>());
        std::shared_ptr <uint8_t> dataM(new uint8_t[size], std::default_delete<uint8_t[]>());
        memcpy(dataTemp.get(),revData.get()+11,nLen-2);
        int nNum = 0;
        for(int i = 0; i < sizeM; i++) {
            uint8_t firstFourBits = (dataTemp.get()[i] >> 4) & 0x0F;    //firstFourBits
            uint8_t lastFourBits = dataTemp.get()[i] & 0x0F;         //lastFourBits
            if(0 == i) {
                memcpy(dataM.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if(nNum < size) {
                    memcpy(dataM.get()+nNum,&lastFourBits,1);
                }
            } else {
                memcpy(dataM.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if((nNum) < size) {
                    memcpy(dataM.get()+nNum,&lastFourBits,1);
                }
            }
            nNum++;
        }
        memcpy(pData,dataM.get(),size);
        return 0;
    } else {
        return 1;
    }
}

int TcpClient::ReadX(int address, int size, void *pData)
{
    int nAllLen = 9;
    int sizeX = size;
    if(sizeX <= 2) {
        sizeX = 1;
    }else {
        if(sizeX%2 == 0)  {
            sizeX = sizeX/2;
        }else {
            sizeX = (sizeX + 1)/2;
        }
    }
    char* data = build(address,size,READ,SUB_BIT,XType);
    std::unique_ptr <char[]> data_ptr(data);
    int nLen = 21;
    //char buf[data.size()];
    //memcpy(buf,data.data(),data.size());
    int nReg = 0;
    nReg = send(SocketClient,data_ptr.get(), nLen,0);
    if (nReg < 0) {

//        ReConnect(m_ip_in, m_port_in);
        nConnected = 1;
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        //revData.resize(nReg);
        std::unique_ptr <char[]> revData(new char[nReg]);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        std::shared_ptr <uint8_t> dataTemp(new uint8_t[sizeX], std::default_delete<uint8_t[]>());
        std::shared_ptr <uint8_t> dataX(new uint8_t[size], std::default_delete<uint8_t[]>());
        memcpy(dataTemp.get(),revData.get()+11,nLen-2);
        int nNum = 0;
        for(int i = 0; i < sizeX; i++) {
            uint8_t firstFourBits = (dataTemp.get()[i] >> 4) & 0x0F;
            uint8_t lastFourBits = dataTemp.get()[i] & 0x0F;
            if(0 == i) {
                memcpy(dataX.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if(nNum < size) {
                    memcpy(dataX.get()+nNum,&lastFourBits,1);
                }
            } else {
                memcpy(dataX.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if((nNum) < size) {
                    memcpy(dataX.get()+nNum,&lastFourBits,1);
                }
            }
            nNum++;
        }
        memcpy(pData,dataX.get(),size);
        return 0;
    } else {
        return 1;
    }
}

int TcpClient::ReadY(int address, int size, void *pData)
{
    int nAllLen = 9;
    int sizeY = size;
    if(sizeY <= 2) {
        sizeY = 1;
    }else {
        if(sizeY%2 == 0)  {
            sizeY = sizeY/2;
        }else {
            sizeY = (sizeY + 1)/2;
        }
    }
    char* data = build(address,size,READ,SUB_BIT,YType);
    std::unique_ptr <char[]> data_ptr(data);
    int nLen = 21;
    //char buf[data.size()];
    //memcpy(buf,data.data(),data.size());
    int nReg = 0;
    nReg = send(SocketClient, data_ptr.get(), nLen,0);
    if (nReg < 0) {

//        ReConnect(m_ip_in, m_port_in);
        // delete[] data;
        nConnected = 1;
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        //revData.resize(nReg);

        std::unique_ptr <char[]> revData(new char[nReg]);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        std::shared_ptr <uint8_t> dataTemp(new uint8_t[sizeY], std::default_delete<uint8_t[]>());
        std::shared_ptr <uint8_t> dataY(new uint8_t[size], std::default_delete<uint8_t[]>());
        memcpy(dataTemp.get(),revData.get()+11,nLen-2);
        int nNum = 0;
        for(int i = 0; i < sizeY; i++) {
            uint8_t firstFourBits = (dataTemp.get()[i] >> 4) & 0x0F;
            uint8_t lastFourBits = dataTemp.get()[i] & 0x0F;
            if(0 == i) {
                memcpy(dataY.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if(nNum < size) {
                    memcpy(dataY.get()+nNum,&lastFourBits,1);
                }
            } else {
                memcpy(dataY.get()+nNum,&firstFourBits,1);
                nNum += 1;
                if((nNum) < size) {
                    memcpy(dataY.get()+nNum,&lastFourBits,1);
                }
            }
            nNum++;
        }
        memcpy(pData,dataY.get(),size);
        return 0;
    } else {
        return 1;
    }
}

int TcpClient::WriteD(int address, int size, void *pData)
{
    int nAllLen = 9;
    char* data = build(address,size,WRITE,SUB_WORD,DType);
    std::unique_ptr<char[]> data_ptr(data);
    int nDataLen = 21;//data.size();
    int nLastLen = size*2;
    //data.resize(nDataLen + nLastLen);
    char* pDataAll = new char[nDataLen + nLastLen];
    std::unique_ptr<char[]> pDataAll_ptr(pDataAll);
    //char buf[data.size()];
    memcpy(pDataAll_ptr.get(),data_ptr.get(), nDataLen);
    memcpy(pDataAll_ptr.get() + nDataLen, pData, size * 2);
    int nReg = 0;
    nReg = send(SocketClient, pDataAll_ptr.get(), nDataLen + nLastLen,0);
    if (nReg < 0) {

//        ReConnect(m_ip_in, m_port_in);
        nConnected = 1;
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        std::unique_ptr <char[]> revData(new char[nReg]);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        return 0;
    } else {

        return 1;
    }
    return 0;
}

int TcpClient::WriteM(int address, int size, void *pData)
{
    int nAllLen = 9;
    char* data = build(address,size,WRITE,SUB_BIT,MType);
    std::unique_ptr<char[]> data_ptr(data);
    int nDataLen = 21;//data.size();
    int nLastLen = size*1;
    char* pDataAll = new char[nDataLen + nLastLen];
    std::unique_ptr<char[]> pDataAll_ptr(pDataAll);
    memcpy(pDataAll_ptr.get(), data_ptr.get(), nDataLen);
    //data.resize(nDataLen + nLastLen);
    memcpy(pDataAll_ptr.get() +nDataLen,pData,size*1);
    //char *buf = new char[strlen(pDataAll)];
    //memcpy(buf,data.data(),data.size());
    int nReg = 0;
    nReg = send(SocketClient, pDataAll_ptr.get(), nDataLen + nLastLen,0);
    if (nReg < 0) {

    //    ReConnect(m_ip_in, m_port_in);
        nConnected = 1;
        return 1;
    }
    Sleep(5);
    nReg = 0;
    char RecvBuff[MaxBufSize];
    nReg = recv(SocketClient, RecvBuff, sizeof(RecvBuff), 0);
    if(nReg > 0) {
        // char* revData = new char[nReg];
        std::unique_ptr <char[]> revData(new char[nReg]);
        //revData.resize(nReg);
        memcpy(revData.get(),RecvBuff,nReg);
        int nLen = ResDataLen(revData.get());
        nAllLen += nLen;
        if(0 != ResError(revData.get())) {
            return 1;
        }
        return 0;
    } else {
        return 1;
    }
    return 0;
}
/*
**生成三菱FX-TCP协议的21字节报文,包括地址、数据长度、命令类型、子命令类型、寄存器类型
**帧头：0x50 0x00 0x00 0xFF 0xFF 0x03 0x00
**读：0x0C 0x00 0x10 0x00 0x01 0x04
**写：0x0A 0x00 0x01 0x14
**  写位：0x01 0x00
**  写字：0x00 0x00
**地址：data[15]
**寄存器类型：data[18]
**  X:0x9C
**  Y:0x9D
**  M:0x90
**  D:0xA8
**数据长度：data[19] = size
*/
char* TcpClient::build(int address, int size, int command, int sub_command, int type)
{
    //数据定义
    char *array = new char[21];
    // std::unique_ptr<char[]> array_ptr(array);

    uint8_t data[21] = {0};
    data[0] = 0x50;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0x03;
    data[6] = 0x00;
    if(command == READ) {
        data[7] = 0x0C;
        data[8] = 0x00;
        data[9] = 0x10;
        data[10] = 0x00;
        data[11] = 0x01;
        data[12] = 0x04;
    }
    else if(command == WRITE)
     {
        uint16_t reqLen = 0;
        if(sub_command == SUB_BIT)
            reqLen = 12 + (size*1);
        else if(sub_command == SUB_WORD)
            reqLen = 12 + (size*2);
        memcpy(&data[7],&reqLen,2);
        data[9] = 0x0A;
        data[10] = 0x00;
        data[11] = 0x01;
        data[12] = 0x14;
    }
    if(sub_command == SUB_BIT)
    {
        data[13] = 0x01;
        data[14] = 0x00;
    }else if(sub_command == SUB_WORD) 
    {
        data[13] = 0x00;
        data[14] = 0x00;
    }
    //3字节地址，4字节的话，地址拷贝截断？
    memcpy(&data[15],&address,3);
    if(type == XType){
        data[18] = 0x9C;
    }
    else if(type == YType){
        data[18] = 0x9D;
    }
    else if(type == MType){
        data[18] = 0x90;
    }
    else if(type == DType){
        data[18] = 0xA8;
    }
    uint16_t dataLen = size;
    memcpy(&data[19],&dataLen,2);
    memcpy(array,data,21);
    return array;
}


int TcpClient::ResDataLen(char* data)
{
    int nLen = 0;
    memcpy(&nLen,data+7,2);
    return nLen;
}


int TcpClient::ResError(char* data)
{
    int nError = 0;
    memcpy(&nError,data+9,2);
    return nError;
}

/*
** 重连
*/
void TcpClient::ReConnect(string ip, int port)
{
    nConnected = 1;
    nNum += 1;
    closesocket(SocketClient);
    slot_Connect(ip, port);
}

/*
** 检查连接状态
*/
int TcpClient::ConnectStatus()
{
    return nConnected;
}

/*
** 分析数据长度
*/
size_t TcpClient::customStrlen(const char* data) {
    size_t length = 0;
    while (data[length] != '\0') {
        ++length;
    }
    return length;
}
