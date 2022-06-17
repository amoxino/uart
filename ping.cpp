/* 
 * File:   BSerialPort.cpp
 */
#include "ping.h"
#include "BSerialPort.h"
#include<iostream>
#include<ctime>
#include <fstream>


using namespace std;
/*
 * Function: BSerialPort
 * 
 * 构造函数，建立并将串口口初始化为 9600,N,1
 * 或初始化一个虚拟串口，如果要定义一个虚拟串口
 * 请将type参数传递为Virtual
 * 
 * Prarm:
 *      filename: 串口设备文件名
 *      type：串口类型，可以是Actual或Virtual;
 */

BShellCommand::BShellCommand(const char *comd_buf, const char *cheak_buf) {
    this->comd_buf = comd_buf;
    this->cheak_buf = cheak_buf;
}

BShellCommand::BShellCommand(const char *comd_buf, const char *cheak_buf,int p) {
    this->comd_buf = comd_buf;
    this->cheak_buf = cheak_buf;
    this->ping_cheak = 1;
}

BShellCommand::~BShellCommand() {
}

/*
 * Function: 
 */
bool BShellCommand::open(void) {
    if ((this->pipein_fp = popen(this->comd_buf, "r")) == NULL)
    {
        perror("popen");
        return 1;
    }
    return 0;
}


/*
 * Function: 
 */
void BShellCommand::cheak(void){
    char readbuf[80];
    char  *p = NULL;
    if(this->open() == 0)
    {
         while(fgets(readbuf, 80, this->pipein_fp))
        {
            if(strstr(readbuf,this->cheak_buf) != NULL)
            {
                if(ping_cheak != 0)
                {
                    strtok(readbuf, this->delim);
                    while((p = strtok(NULL, this->delim)))
                    {
                        if (strcmp(p, "0%") == 0)
                        {
                            this->comd_cheak = 1;
                        }
                    }
                }else
                {
                    this->comd_cheak = 1;
                }
            }
           /* fputs(readbuf, pipeout_fp); 输出打印*/  
        }
        this->close();
    }
}


/*
 * Function: 
 */
void BShellCommand::close() {
    ::pclose(this->pipein_fp);
}


void shell_cheak()
{
    fstream f1;
    f1.open("/home/sysadm/cheak.txt",ios::out|ios::app);

    const char *ping_comf = "ping -c 6 192.168.1.1";
    const char *ping_cheak = "0% packet loss";

    const char *sim_comf = "ifconfig";
    const char *sim_cheak = "ppp-0";

    const char *esame_comd = "./esam1187YDevTest";
    const char *esame_cheak = "round 100 OK";

    const char *gpio1_on_cheak = "gpio-110 (K1_gpio             ) in  lo";
    const char *gpio2_on_cheak = "gpio-111 (K2_gpio             ) in  lo";

    const char *gpio1_off_cheak = "gpio-110 (K1_gpio             ) in  hi";
    const char *gpio2_off_cheak = "gpio-111 (K2_gpio             ) in  hi";
    
    const char *gpio_comf = "cat /sys/kernel/debug/gpio";

    char KA1_open[8] = {0x01,0x05,0x08,0x10,0xFF,0x00,0x8F,0x9F};
    char KA1_close[8] = {0x01,0x05,0x08,0x10,0x00,0x00,0xCE,0x6F};
    char KA2_open[8] = {0x01,0x05,0x08,0x11,0xFF,0x00,0xDE,0x5F};
    char KA2_close[8] = {0x01,0x05,0x08,0x11,0x00,0x00,0x9F,0xAF};
    char net1_open[8] = {0x01,0x05,0x08,0x16,0x00,0x00,0x2E,0x6E};
    char net2_open[8] = {0x01,0x05,0x08,0x16,0xFF,0x00,0x6F,0x9E};
    
    BShellCommand ping_net1(ping_comf,ping_cheak,1);
    BShellCommand ping_net2(ping_comf,ping_cheak,1);

    BShellCommand sim(sim_comf,sim_cheak);

    BShellCommand esame(esame_comd,esame_cheak);

    BShellCommand gpio1_on(gpio_comf,gpio1_on_cheak);
    BShellCommand gpio1_off(gpio_comf,gpio1_off_cheak);

    BShellCommand gpio2_on(gpio_comf,gpio2_on_cheak);
    BShellCommand gpio2_off(gpio_comf,gpio2_off_cheak);
    
    BSerialPort uart6("/dev/ttyS0");
    uart6.set_Speed(19200);
    uart6.set_Parity(8, 1, 'o');

    uart6.write(net1_open,sizeof(net1_open));
    sleep(2);
    ping_net1.cheak();
    if (ping_net1.comd_cheak != 1 )
    {
        f1<<"NET1检测失败！！"<<endl;
        cout<<"NET1检测失败！！"<<endl;
    }
    else
    {
        f1<<"NET1检测成功！！"<<endl;
        cout<<"NET1检测成功！！"<<endl;
    }

    uart6.write(net2_open,sizeof(net2_open));
    sleep(2);
    ping_net2.cheak();
    if (ping_net2.comd_cheak != 1 )
    {
        f1<<"NET2检测失败！！"<<endl;
        cout<<"NET2检测失败！！"<<endl;
    }
    else
    {
        f1<<"NET2检测成功！！"<<endl;
        cout<<"NET2检测成功！！"<<endl;
    }

    esame.cheak();
    if (esame.comd_cheak != 1 )
    {
        f1<<"esame检测失败！！"<<endl;
        cout<<"esame检测失败！！"<<endl;
    }
    else
    {
        f1<<"esame检测成功！！"<<endl;
        cout<<"esame检测成功！！"<<endl;
    }

    sim.cheak();
    if (sim.comd_cheak != 1 )
    {
        f1<<"4G检测失败！！"<<endl;
        cout<<"4G检测失败！！"<<endl;
    }
    else
    {
        f1<<"4G检测成功！！"<<endl;
        cout<<"4G检测成功！！"<<endl;
    }

    uart6.write(KA1_open,sizeof(KA1_open));
    sleep(2);
    gpio1_on.cheak();

    uart6.write(KA1_close,sizeof(KA1_close));
    sleep(2);
    gpio1_off.cheak();

    if (gpio1_on.comd_cheak != 1 && gpio2_off.comd_cheak != 1)
    {
        f1<<"遥信1检测失败！！"<<endl;
        cout<<"遥信1检测失败！！"<<endl;
    }
    else
    {
        f1<<"遥信1检测成功！！"<<endl;
        cout<<"遥信1检测成功！！"<<endl;
    }

    uart6.write(KA2_open,sizeof(KA2_open));
    sleep(2);
    gpio2_on.cheak();

    uart6.write(KA2_close,sizeof(KA2_close));
    sleep(2);
    gpio2_off.cheak();

    if (gpio1_on.comd_cheak != 1 && gpio2_off.comd_cheak != 1)
    {
        f1<<"遥信2检测失败！！"<<endl;
        cout<<"遥信2检测失败！！"<<endl;
    }
    else
    {
        f1<<"遥信2检测成功！！"<<endl;
        cout<<"遥信2检测成功！！"<<endl;
    }

    f1<<"----------------------------------------"<<endl;
    cout<<"---------------------------------------"<<endl;
    f1.close();
}