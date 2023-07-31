/*****************************************************************//**
 * \file   xargs.c
 * \brief  Write a simple version of the UNIX xargs program: its 
 *         arguments describe a command to run, it reads lines from 
 *         the standard input, and it runs the command for each line, 
 *         appending the line to the command's arguments. Your solution 
 *         should be in the file user/xargs.c
 * 
 * \author JMC
 * \date   July 2023
 *********************************************************************/

#include "kernel/types.h"
#include "user/user.h"

#define MAX_ARG_LEN 32
#define MAX_ARG_NUM 32
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define NULL (void*)0



int main(int argc, char* argv[])
{
  if (argc < 2) 
  {
    fprintf(STDERR, "usage: xargs <command> ...\n");
    exit(1);
  }

  // 读取命令行参数
  char* cmd[MAX_ARG_NUM] = {};
  for (int i = 1; i < argc; i++) 
  {
    cmd[i - 1] = argv[i];
  }

  // 读取标准输入
  char buf[MAX_ARG_LEN];
  int n;
  if ((n = read(STDIN, buf, MAX_ARG_LEN)) < 0) 
  {
    fprintf(STDERR, "xargs: read error\n");
    exit(1);
  }

  // 将标准输入的内容作为命令行参数
  for (char* beg = buf, *end; (end = strchr(beg, '\n')); beg = end + 1)
  {
    *end = '\0';
    cmd[argc - 1] = beg;
    int pid = fork();
    if (pid < 0) 
    {
      fprintf(STDERR, "xargs: fork error\n");
      exit(1);
    }
    else if (pid == 0) 
    {
      if (exec(argv[1], cmd) < 0) 
      {
        fprintf(STDERR, "xargs: exec error\n");
        exit(1);
      }
    }
    else 
    {
      wait(NULL);
    }
  }

  return 0;
}
