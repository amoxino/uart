#ifndef _PROC_H_
#define _PROC_H_
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void reread_mapping();

extern bool input_value;
extern std::vector<char *> uart;
extern const char *uart_names[4];

#endif