
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>

#include <sys/uio.h>


#define BILLION  1000000000L;

int main6848 ()
{
    int numnum = 0;
    while(1)
    {
        numnum = numnum + 1;
        printf("new number for you boss %d", numnum);
        usleep(100);
    }


    return(0);

}
