#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

int
main()
{
  char buf[256]= {0};
#if 0
  int fd = open("/tmp/test.dat", O_RDWR | O_CREAT, 0777);
  int dp = dup(fd);
  printf("fd=%d dp=%d\n",fd,dp);
  close(fd);
  printf("%d\n", write(dp, "hello world\n", strlen("hello world\n")));
  sync();
  close(dp);
#endif
  printf("%s\n",getwd(buf));
  printf("%s\n",buf);
  printf("%s\n",getcwd(buf,256));
  printf("%s\n",buf);
  printf("%s\n",get_current_dir_name());
  return 0;
}
