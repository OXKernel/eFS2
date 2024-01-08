#include <sys/stat.h>

int
main()
{
  int mode = S_IROTH;
  int iter = 2;
  do {
    if(mode & S_IRUSR) {
      printf("iter=%d\n",iter);
    }
    mode <<= 3;
  } while(iter--);
}
