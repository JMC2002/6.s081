#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char* argv[])
{
  // ���������ܵ�
  int p[2][2];
  pipe(p[0]), pipe(p[1]);

  // �������ֽ�
  char buf[] = { 'a' };

  // �����ӽ���
  int pid = fork();
  if (!pid) 
  {
    // �ӽ���
    // �ر�д��
    close(p[0][1]);
    if (read(p[0][0], buf, 1) > 0)
    {
      printf("%d: received ping\n", getpid());
    }
    else     
    {
      printf("%d: read error\n", getpid());
    }
    // �رն���
    close(p[1][0]);
    write(p[1][1], buf, 1);
    exit(0);
  }
  else if (pid > 0)
  {
    // ������
    // �رն���
    close(p[0][0]);
    write(p[0][1], buf, 1);

    wait(0);
    // �ر�д��
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
    // forkʧ��
    printf("fork error\n");
  }
  exit(0);
}
