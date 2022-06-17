/* 
 * File:   system.h
 */

#ifndef SYSTEM_H
#define SYSTEM_H
#include <fcntl.h>
#include <locale>
#include <termios.h>
#include <pty.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


class BShellCommand {
public:
    
    BShellCommand(const char *comd_buf, const char *cheak_buf);
    BShellCommand(const char *comd_buf, const char *cheak_buf,int p);
    
    virtual ~BShellCommand();
    int comd_cheak = 0;

    /*
     * 参数: 串口设备文件路径
     * 功能：打开串口，默认设置串口设备参数：9600,N,1
     * 返回值：成功 true ; 失败 false
     */
    bool open(void);

    void cheak(void);

    void close(void);
private:
    /*文件描述符*/
    FILE *pipein_fp;
    /*输入指令*/
    bool ping_cheak = 0;
    const char *delim = " ";
    const char *comd_buf = NULL;
    const char *cheak_buf = NULL;
};

void shell_cheak(void);
#endif /* BSERIALPORT_H */

