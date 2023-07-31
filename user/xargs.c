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



int main(int argc, char* argv[])
{
  if (argc < 2) {
    fprintf(STDERR, "usage: xargs <command> ...\n");
    exit(1);
  }

  // 读取命令行参数
  char* cmd[MAX_ARG_NUM];
  for (int i = 1; i < argc; i++) {
    cmd[i - 1] = argv[i];
  }

  // 读取标准输入
  char buf[MAX_ARG_LEN];
  int n;
  while ((n = read(STDIN, buf, MAX_ARG_LEN)) > 0) {
    // 读取到的内容以空格分隔
    int arg_num = 0;
    char* args[MAX_ARG_NUM];
    char* p = buf;
    while (arg_num < MAX_ARG_NUM && p < buf + n) {
      args[arg_num++] = p;
      while (p < buf + n && *p != ' ' && *p != '\n') {
        p++;
      }
      *p = '\0';
      p++;
    }
    args[arg_num] = 0;

    // 执行命令
    int pid = fork();
    if (pid < 0) {
      fprintf(STDERR, "fork failed\n");
      exit(1);
    }
    else if (pid == 0) {
      // 子进程
      exec(cmd[0], cmd);
      fprintf(STDERR, "exec failed\n");
      exit(1);
    }
    else {
      // 父进程
      wait(0);
    }
  }

  return 0;
}
