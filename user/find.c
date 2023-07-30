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
  // 找到最后一个 / 后面的字符
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

/**
 * @brief 在指定路径下查找指定文件
 * 
 * @param path 路径
 * @param filename 文件名
 */
void find(char* path, const char* filename)
{
  char buf[BUFSIZE], * p;
  int fd;
  struct dirent de;
  struct stat st;

  // 尝试打开文件
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(STDERR, "find: cannot open %s\n", path);
    return;
  }

  // 尝试获取文件信息
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
    // 如果路径过长，报错
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      break;
    }

    // 将路径拷贝到 buf 中
    strcpy(buf, path);
    p = buf + strlen(buf);
    // 在路径后面加上 /
    *p++ = '/';
    // 读取目录下的文件
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      // 如果文件不存在，或者是 . 或者是 ..，则跳过
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      // 将文件名拷贝到 buf 中
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
  // 及时关闭文件，避免资源泄露
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
