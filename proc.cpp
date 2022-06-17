#include "proc.h"

bool input_value =0;
std::vector<char *> uart;
bool cmd_cheak = 0;

static std::string read_file(int fd) {
  char buf[255];
  std::string content;

  for (int length; (length = read(fd, buf, sizeof(buf))) > 0;) {
    if (length < 0) {
      std::stringstream error;
      error << "Error reading file:" << std::strerror(errno) << "\n";
      std::fprintf(stderr, "%s", error.str().c_str());
      throw error.str();
    }
    content.append(buf, length);
  }

  return content;
}

static std::string read_file(const char *filepath) {
  int fd = open(filepath, O_RDONLY);

  if (fd < 0) {
    std::stringstream error;
    error << "Error opening " << filepath << ":" << std::strerror(errno)
          << "\n";
    std::fprintf(stderr, "%s", error.str().c_str());
    throw error.str();
  }

  std::string contents = read_file(fd);

  if (close(fd)) {
    std::stringstream error;
    error << "Error opening " << filepath << ":" << std::strerror(errno)
          << "\n";
    std::fprintf(stderr, "%s", error.str().c_str());
    throw error.str();
  }

  return contents;
}

std::string getcmdline(pid_t pid) {
  const int maxfilenamelen = 14 + 20 + 1;
  char filename[maxfilenamelen];
  char *out_ptr = NULL;

  std::snprintf(filename, maxfilenamelen, "/proc/%d/cmdline", pid);
  std::string cmdline;
  bool replace_null = false;
  try {
    cmdline = read_file(filename);
  } catch (const char *e) {
    std::fprintf(stderr, "An exception occurred. Exception %s \n", e);
    cmdline = "";
  } catch (...) {
    std::fprintf(stderr, "An exception occurred while reading cmdline.\n");
    cmdline = "";
  }

  if (cmdline.empty() || cmdline[cmdline.length() - 1] != '\0') {
    // invalid content of cmdline file. Add null char to allow further
    // processing.
    cmdline.append(1, '\0');
    return cmdline;
  }

  // join parameters, keep prgname separate, don't overwrite trailing null
  for (size_t idx = 0; idx < (cmdline.length() - 1); idx++) {
    if (cmdline[idx] == 0x00) {
      if (replace_null) {
        cmdline[idx] = ' ';
      }
      replace_null = true;
    }
  }
  char  *tmpStr = strtok_r((char *)cmdline.c_str(), "/", &out_ptr);
  char  *tmpStr1;
  while(tmpStr != NULL) 
  {
    tmpStr1 = tmpStr;
    tmpStr = strtok_r(NULL, "/", &out_ptr);
  }
  return tmpStr1;
}

const char *uart_names[4] = {"/dev/ttyRS0","/dev/ttyRS1","/dev/ttyRS2","/dev/ttyRS3"};

bool is_number(const char *string) {
  while (*string) {
    if (!isdigit(*string))
      return false;
    string++;
  }
  return true;
}

void get_info_by_linkname(const char *pid, const char *linkname) {
  if (strncmp(linkname, "/dev/", 4) == 0) {
    if(input_value)
    {
      for(int i=0;i<uart.size();i++)
      {
        if(!strcmp(linkname,uart[i]))
        {
          std::cout << getcmdline(atoi(pid)) << " 串口占用："<< linkname <<std::endl;
          cmd_cheak = 1;
        }
      }
    }else
    {
      for(int i =0;i<4;i++)
      {
        if(!strcmp(linkname,uart_names[i]))
        {
          std::cout << getcmdline(atoi(pid)) << " 串口占用："<< linkname <<std::endl;
          cmd_cheak = 1;
        }
      }
    }
  }
}

void get_info_for_pid(const char *pid) {
  char dirname[10 + 20];

  size_t dirlen = 10 + strlen(pid);
  snprintf(dirname, dirlen, "/proc/%s/fd", pid);

  DIR *dir = opendir(dirname);

  if (!dir) {
    return;
  }

  /* walk through /proc/%s/fd/... */
  dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type != DT_LNK)
      continue;

    size_t fromlen = dirlen + strlen(entry->d_name) + 1;
    char fromname[10 + 20 + 1 + 10];
    snprintf(fromname, fromlen, "%s/%s", dirname, entry->d_name);


    int linklen = 80;
    char linkname[linklen];
    int usedlen = readlink(fromname, linkname, linklen - 1);//读取fd下的sockert连接字符串
    if (usedlen == -1) {
      continue;
    }
    linkname[usedlen] = '\0';
    get_info_by_linkname(pid, linkname);
  }
  closedir(dir);
}

void reread_mapping() 
{
  DIR *proc = opendir("/proc");

  if (proc == 0) {
    std::cerr << "Error reading /proc, needed to get inode-to-pid-maping\n";
    exit(1);
  }

  dirent *entry;

  while ((entry = readdir(proc))) 
  {
      if (entry->d_type != DT_DIR)
        continue;

      if (!is_number(entry->d_name))
        continue;

      get_info_for_pid(entry->d_name);
  }
  closedir(proc);
  if(cmd_cheak)exit(1);
}
