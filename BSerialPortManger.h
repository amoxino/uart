/* 
 * File:   BSerialPortManger.h
 */

#ifndef BSERIALPORTMANGER_H
#define BSERIALPORTMANGER_H

#include "BSerialPort.h"
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

/*
 * 功能：实现串口数据的非阻塞读取
 *  主要调用select函数对多个串口文件描述符进行监控
 *  当有串口数据可读时则调用注册的回调函数让用户读取
 *  并处理串口数据。
 * 
 * 使用该类需要以下步骤：
 *  1.初始化串口类。
 *  2.定义SerialPortManger
 *  3.将初始化好的串口类和对应的串口数据处理函数
 *  添加到SerialPortManger对象中
 *  4.创建线程，循环调用read_monitor方法。
 * 
 */
class BSerialPortManger {
public:
    BSerialPortManger();
    BSerialPortManger(const BSerialPortManger& orig);
    virtual ~BSerialPortManger();
    
    /*向串口管理类中添加一个待监控的串口*/
    void add(BSerialPort* serial,void (*func)(BSerialPort *serial));
    void add(BSerialPort* serial,char *cheak_buf,int buf_len,void (*func)(BSerialPort *serial,char *cheak_buf,int buf_len));
    /*从管理类中移除一个串口*/
    void remove(BSerialPort *serial);
    /*每调用一次，检查一次是否有串口数据到达*/
    void read_monitor();
public:
    vector<BSerialPort *> list;
    int fd_max;
    fd_set fd_sets;
    fd_set handle_sets;
};
#endif /* BSERIALPORTMANGER_H */

