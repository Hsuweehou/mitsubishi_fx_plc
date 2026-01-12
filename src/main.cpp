#include <iostream>
#include "../include/tcpclient.h"

int mitsubishi_comm_test(){
    TcpClient client;
    int nReg = client.slot_Connect("192.168.88.172", 502);
    if (0 == nReg)
        std::cout << "sucess" << endl;
    else
        std::cout << "failed" << endl;

    Sleep(100);

    //write D
    int16_t data1 = 222;
    int nReg1 = client.WriteD(170, 2, &data1);
    if (0 == nReg1)
        std::cout << "WriteD D170 sucess \n";
    else 
        std::cout << "WriteD D170 failed \n";
    //read D
    int16_t data2;
    int nReg2 = client.ReadD(170, 2, &data2);
    if(0 == nReg2)
        std::cout << "ReadD D170 = " << data2 << "\n";
    
    data1 = 221;
    nReg1 = client.WriteD(170, 2, &data1);
    nReg2 = client.ReadD(170, 2, &data2);
    if(0 == nReg2)
        std::cout << "ReadD D170 = " << data2 << "\n";
    
    data1 = 220;
    nReg1 = client.WriteD(170, 2, &data1);
    nReg2 = client.ReadD(170, 2, &data2);
    if(0 == nReg2)
        std::cout << "ReadD D170 = " << data2 << "\n";

    //write M
    uint8_t data3 = 16; //16的十六进制就是10 如果写某一个M点写1要把写入之写成16  / 写0就不需要了直接写0就行
    int nReg3 = client.WriteM(200, 1, &data3);
    if (0 == nReg3)
        std::cout << "WriteD M200 sucess \n";
    else
        std::cout << "WriteD M200 failed \n";
    //read M
    uint8_t data4;
    int nReg4 = client.ReadM(200, 1, &data4);
    if (0 == nReg4)
        std::cout << "ReaD D200 = " << (int)data4 << "\n";

    //write string = sub_command==SUB_WORD==size*2==7*2 所以读取的时候size要*2
    char pStr[] = "i made it";
    int nReg5 = client.WriteD(300, 7, &pStr);
    if (0 == nReg5)
        std::cout << "WriteStr D300 sucess \n";
    else
        std::cout << "WriteStr D300 failed \n";
    //read string
    char str2[30];
    int nReg6 = client.ReadD(300, 15, &str2);
    if (0 == nReg6)
        std::cout << "ReadStr D300 =  " << str2 << "\n";
    
    system("pause");
    return 0;
}


int main()
{
    mitsubishi_comm_test();
    // xinje_comm_test();
    return 0;
   
}
