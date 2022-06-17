/* 
 * File:   BSerialPort.cpp
 */
#include "BSerialPort.h"
#include "BSerialPortManger.h"
#include "proc.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include <time.h>

using namespace std;


static int speed_arr[] = {
    B921600, B460800, B230400, B115200, B57600, B38400, B19200,
    B9600, B4800, B2400, B1200, B300,
};

static int name_arr[] = {
    921600, 460800, 230400, 115200, 57600, 38400, 19200,
    9600, 4800, 2400, 1200, 300,
};

BSerialPort::BSerialPort() {
    this->fd = -1;
    this->function = NULL;
}


BSerialPort::BSerialPort(char* filename,int speed) {
    printf("name:%s\n",filename);
    cout<<"speed:"<<speed<<endl;
    this->fd = ::open(filename, O_RDWR);
    if (this->fd > 0) {
        if (!set_Speed(speed))
            ::close(this->fd);
        if (!set_Parity(8, 1, 'N'))
            ::close(this->fd);
    } else {
        printf("serial port open failed %s\n", filename);
        exit(2);
    }
    this->portname.insert(0,filename);
    this->function = NULL;
}

BSerialPort::BSerialPort(const char* filename,int speed) {
    printf("name:%s\n",filename);
    cout<<"speed:"<<speed<<endl;
    this->fd = ::open(filename, O_RDWR);
    if (this->fd > 0) {
        if (!set_Speed(speed))
            ::close(this->fd);
        if (!set_Parity(8, 1, 'N'))
            ::close(this->fd);
    } else {
        printf("serial port open failed %s\n", filename);
        exit(2);
    }
        struct termios options;
    this->portname.insert(0,filename);
    this->function = NULL;
}

BSerialPort::~BSerialPort() {
}

/*
 * Function: 
 */
bool BSerialPort::open(char* filename) {
    if (this->fd > 0) {
        ::close(this->fd);
        printf("serial port open failed %s\n", filename);
    }
    this->fd = ::open(filename, O_RDWR);
    if (this->fd > 0) {
        if (!set_Speed(9600))
            ::close(this->fd);
        if (!set_Parity(8, 1, 'N'))
            ::close(this->fd);
        fcntl(this->fd,F_SETFL,FNDELAY);
    }
}

/*
 * Function: 
 */
bool BSerialPort::set_Speed(int speed) {
    struct termios Opt;

    if (this->fd < 0)
        return false;
    tcgetattr(this->fd, &Opt);

    for (int i = 0; i < sizeof (speed_arr) / sizeof (int); i++) {
        if (speed == name_arr[i]) {
            tcflush(this->fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            int status = tcsetattr(this->fd, TCSANOW, &Opt);
            if (status == 0)
                return true;
            else if (status == -1)
                return false;
        }
        tcflush(this->fd, TCIOFLUSH);
    }
    printf("set speed failed\n");
    exit(3);
}

/*
 * Function: 
 */
bool BSerialPort::set_Parity(int databits, int stopbits, char parity) {
    struct termios options;
    memset(&options,0,sizeof(options));
    if (tcgetattr(this->fd, &options) != 0) {
        return false;
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            printf("databits set failed\n");
            return false;
    }

    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag = IGNPAR;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 's':
        case 'S':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            printf("parity set failed\n");
            return false;
    }

    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            printf("stopbits set failed\n");
    }


    options.c_iflag &= ~(INLCR | ICRNL | IGNCR| IXON);
    //options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // options.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 2;
    tcflush(this->fd, TCIFLUSH);
    options.c_lflag = 0;
    if (tcsetattr(this->fd, TCSANOW, &options) != 0)
    return false;
    return true;
}

/*
 * Function: 
 */
int BSerialPort::read(char* dest, int bytes) {
    int byte = ::read(this->fd, dest, bytes);
    return byte;
}

/*
 * Function: 
 */
int BSerialPort::write(const char* buff, int bytes) {
    int byte = ::write(this->fd, buff, bytes);
    return byte;
}

/*
 * Function: 
 */
void BSerialPort::close() {
    ::close(this->fd);
}

int SndBufferCount = 6;

int strchk(char *send_buf,char *read_buf,int buf_len)
{
    for(int i = 0;i < buf_len;i++)
    {
        if(send_buf[i] != read_buf[i])return 1;
    }
    return 0;
}


void uart_loop(BSerialPort *serial)
{
    int rdf;
    int scalepoint = 0;
    char buff[1024];
    char *bufscale;
    memset(buff,'\0',strlen(buff));
    bufscale = buff;
    while((rdf = serial->read(bufscale,1024)) != 0)
    {
        scalepoint += rdf;
        if(scalepoint >= 1024)break;
        bufscale = buff + scalepoint;
    }
    // usleep(10);
    cout << serial->portname <<" receive----" <<scalepoint<<endl;
    // printf("receive----%d\n",scalepoint );
    for(int i=0;i<scalepoint;i++)
    {
        printf("%02X ",buff[i]);
    }
    printf("\n");
    if (scalepoint != 0) {
        rdf = serial->write(buff,scalepoint);
        scalepoint = 0;
    }
}


#define THREADMAX 10

int sid[10];
pthread_t thread,threadd;
pthread_mutex_t mut; //互斥锁类型

void *thread1(void *arg) //线程函数
{
    //(&mut,NULL);//用默认属性初始化互斥锁
    //pthread_mutex_lock(&mut); //加锁，用于对共享变量操作
    struct timespec ts, ts1;
    struct BSerialPortManger *pstru;
    pstru = ( struct BSerialPortManger *) arg;
    ts.tv_nsec=30000;
    ts.tv_sec=0;
    while(1)
    {
        pstru->read_monitor();
        if ( nanosleep(&ts, &ts1) == -1 )
        {
            printf("error!\n");
            exit(1);
        }
    }
    //pthread_mutex_unlock(&mut); //解锁

}

extern const char *uart_names[4];

static void versiondisplay(void) { std::cout << "version 1.0.0" << "\n"; }

static void help(bool iserror) {
  std::ostream &output = (iserror ? std::cerr : std::cout);

  output << "		-V : 程序版本.\n";
  output << "		-h : 帮助.\n";
  output << "		-p : 设定检测串口节点以\",\"隔开,例（-p ttysz3,ttysz4,......）.\n";
  output << "		-s : 设置串口波特率，例(-s 9600).\n";
}
extern bool input_value;
bool cheak_mode = 0;
int speed = 9600;
extern vector<char *> uart;
void uart_list(char *optarg_name)
{
    char *out_ptr = NULL;
    char dev_name[30];
    char  *tmpStr = strtok_r(optarg_name, ",", &out_ptr);
    while(tmpStr != NULL) 
    {
        memset(dev_name,0,sizeof(dev_name));
        sprintf(dev_name,"/dev/%s",tmpStr);
        uart.push_back(strdup(dev_name));
        tmpStr = strtok_r(NULL, ",", &out_ptr);
    }
}
int main(int argc, char **argv) {
  
    int opt;
    while ((opt = getopt(argc, argv, "Vhp:s:")) != -1) {
        switch (opt) {
            case 'V':
                versiondisplay();
                exit(0);
            case 'h':
                help(false);
                exit(0);
            case 'p':
                input_value = 1;
                uart_list(optarg);
                break;
            case 's':
                speed = atoi(optarg);
                break;
            default:
                help(true);
                exit(EXIT_FAILURE);
        }
    }
    vector<BSerialPort *> BSerial_uart;
    reread_mapping();
    if(input_value)
    {
        for(int i=0;i<uart.size();i++)
        {
            BSerial_uart.push_back(new BSerialPort(uart[i],speed));
        }
    }else
    {
        for(int i=0;i<4;i++)
        {
            BSerial_uart.push_back(new BSerialPort(uart_names[i],speed));
        }
    }
    BSerialPortManger ppp;
    for(int i=0;i<BSerial_uart.size();i++)
    {
        ppp.add(BSerial_uart[i],uart_loop);
    }
    pthread_create(&thread, NULL, thread1, &ppp);
    printf("开始检测，请使用串口输入!!!\n");
    while(1)
    {
        sleep(1);
    }

    pthread_cancel(thread);
    pthread_join(thread,NULL); 
    return 0;
}