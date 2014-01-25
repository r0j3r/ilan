#include <unistd.h>
#include <fcntl.h>

int
main()
{
   unsigned char cent[]={0xc2,0xa2};
   unsigned char euro[]={0xe2,0x82,0xac};
   int fd;
   
   fd = open("utf8_sample", O_CREAT|O_RDWR, 0600);
   write(fd, cent, 2);
   write(fd, euro, 3);
   close(fd); 
}
