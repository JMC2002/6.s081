#include "kernel/types.h"
#include "user/user.h"

#define primeMax  35
#define stdin     0
#define stdout    1
#define stderr    2
#define pipeRead  0
#define pipeWrite 1

void primeFunc(int left[2])
{
  int prime, n;
  close(left[pipeWrite]);
  if (!read(left[pipeRead], &prime, sizeof(prime)))
  { // ��ȡʧ�ܣ�ɸ���������ݹ����
    exit(0);
  }
  close(left[pipeRead]);

  printf("prime %d\n", prime);
  int right[2], pid;
  if (pipe(right) < 0)
  {
    write(stderr, "pipe failed\n", 12);
    exit(-1);
  }

  if ((pid = fork()) < 0)
  {
    write(stderr, "fork failed\n", 12);
    exit(-1);
  }
  else if (!pid)
  { // ������
    close(right[pipeRead]);
    while (read(left[pipeRead], &n, sizeof(n)))
    {
      if (n % prime)
      { // �Ǳ�����д��pipe
        write(right[pipeWrite], &n, sizeof(n));
      }
    }
    close(left[pipeRead]);
    close(right[pipeWrite]);

    wait(0);
    exit(0);
  }
  else
  { // �ӽ���
    primeFunc(right);
    exit(0);
  }
}

int main(int argc, char* argv[])
{
  int p[2], pid;
  if (pipe(p) < 0)
  {
    write(stderr, "pipe failed\n", 12);
    exit(-1);
  }

  if ((pid = fork()) < 0)
  {
    write(stderr, "fork failed\n", 12);)
    exit(-1);
  }
  else if (!pid)
  { // �����̽�2 - 35д��pipe
    close(p[pipeRead]);
    for (int i = 2; i <= primeMax; i++)
    {
      write(p[pipeWrite], &i, sizeof(i));
    }
    close(p[pipeWrite]);

    wait(0);
    exit(0);
  }
  else
  {
    primeFunc(p);
    exit(0);
  }

  return 0;
}
