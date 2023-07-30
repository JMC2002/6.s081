#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUFSIZE 512

char* fmtname(char* path)
{
  char* p;
  // �ҵ����һ�� / ������ַ�
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

/**
 * @brief ��ָ��·���²���ָ���ļ�
 * 
 * @param path ·��
 * @param filename �ļ���
 */
void find(char* path, const char* filename)
{
  char buf[BUFSIZE], * p;
  int fd;
  struct dirent de;
  struct stat st;

  // ���Դ��ļ�
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(STDERR, "find: cannot open %s\n", path);
    return;
  }

  // ���Ի�ȡ�ļ���Ϣ
  if (fstat(fd, &st) < 0) {
    fprintf(STDERR, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if (strcmp(fmtname(path), filename) == 0) {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    // ���·������������
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      break;
    }

    // ��·�������� buf ��
    strcpy(buf, path);
    p = buf + strlen(buf);
    // ��·��������� /
    *p++ = '/';
    // ��ȡĿ¼�µ��ļ�
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      // ����ļ������ڣ������� . ������ ..��������
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      // ���ļ��������� buf ��
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, filename);
    }
    break;
  }
  // ��ʱ�ر��ļ���������Դй¶
  close(fd);
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    fprintf(STDERR, "Usage: find <path> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
