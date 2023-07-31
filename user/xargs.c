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
  if (argc < 2) 
  {
    fprintf(STDERR, "usage: xargs <command> ...\n");
    exit(1);
  }

  // ��ȡ�����в���
  char* cmd[MAX_ARG_NUM] = {};
  for (int i = 1; i < argc; i++) 
  {
    cmd[i - 1] = argv[i];
  }

  // ��ȡ��׼����
  char buf, arg[MAX_ARG_LEN];
  for (int i = 0; read(STDIN, &buf, 1) > 0; )
  {
    if (buf == '\n')
    {
      // ��ȡ�����з������������ݸ�����
      arg[i] = '\0';

      // forkһ���ӽ���ִ������
      int pid = fork();
      if (pid < 0)
      {
        fprintf(STDERR, "fork error\n");
        exit(1);
      }
      else if (pid == 0)
      {
        // �ӽ���ִ������
        cmd[argc - 2] = arg;
        exec(cmd[0], cmd);
        fprintf(STDERR, "exec error\n");
      }
      else
      {
        // �����̵ȴ��ӽ��̽���
        wait(0);
        i = 0;
      }
    }
    else
    {
      arg[i++] = buf;
    }
  }
  return 0;
}
