#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char* argv[])
{
  // 创建两个管道
  int p[2][2];
  pipe(p[0]), pipe(p[1]);

  // 传输用字节
  char buf[] = { 'a' };

  // 创建子进程
  int pid = fork();
  if (!pid) 
  {
    // 子进程
    // 关闭写端
    close(p[0][1]);
    if (read(p[0][0], buf, 1) > 0)
    {
      printf("%d: received ping\n", getpid());
    }
    else     
    {
      printf("%d: read error\n", getpid());
    }
    // 关闭读端
    close(p[1][0]);
    write(p[1][1], buf, 1);
    exit(0);
  }
  else if (pid > 0)
  {
    // 父进程
    // 关闭读端
    close(p[0][0]);
    write(p[0][1], buf, 1);

    wait(0);
    // 关闭写端
    close(p[1][1]);
    if (read(p[1][0], buf, 1) > 0)
    {
      printf("%d: received pong\n", getpid());
    }
    else
    {
      printf("%d: read error\n", getpid());
    }
  }
  else
  {
    // fork失败
    printf("fork error\n");
  }
  exit(0);
}
