/* 
 * File:   BSerialPortManger.cpp
 */

#include "BSerialPortManger.h"
#include <stdio.h>


BSerialPortManger::BSerialPortManger() {
    this->fd_max = 0;
    FD_ZERO(&this->fd_sets);
}

BSerialPortManger::BSerialPortManger(const BSerialPortManger& orig) {
    this->list = orig.list;
}

BSerialPortManger::~BSerialPortManger() {
}

/*
 * Function: 
 * 
 */
void BSerialPortManger::add(BSerialPort* serial,void (*func)(BSerialPort *serial)){
    serial->function = func;
    if(serial->fd > 0){
        FD_SET(serial->fd, &this->handle_sets);;
        this->list.push_back(serial);
        this->fd_max = 0;
        cout << "list_count:" << this->list.size()<<endl;
        for(int i = 0; i < this->list.size(); i++){
            if(this->list.at(i)->fd > this->fd_max)
                this->fd_max = this->list.at(i)->fd;
        }
    }   
}
/*
 * Function: 
 */
void BSerialPortManger::remove(BSerialPort* serial){
    for(int i = 0; i < this->list.size(); i++){
        if((this->list.at(i)) == serial){
            this->list.erase(this->list.begin()+i);
        }
    }
    FD_CLR(serial->fd, &this->handle_sets);
    this->fd_max = 0;
    for(int i = 0; i < this->list.size(); i++){
        if(this->list.at(i)->fd > this->fd_max)
            this->fd_max = this->list.at(i)->fd;
    }
}
/*
 * Function: 
 * 
 */
void BSerialPortManger::read_monitor(){
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    this->fd_sets = this->handle_sets;
    int rs = select(this->fd_max+1, &this->fd_sets, NULL, NULL, &timeout);//将不可读的套接字从fd_set里去掉
    if(rs > 0){
        for(int i = 0; i < this->list.size(); i++){
            if(FD_ISSET(this->list.at(i)->fd, &this->fd_sets) && this->list.at(i)->function != NULL){
                this->list.at(i)->function(this->list.at(i));
            }
        }
    }
}
