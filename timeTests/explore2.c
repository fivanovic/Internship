
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

int main75 ()
{

    FILE * reslist;

    reslist = fopen("/sys/bus/pci/devices/0000:01:00.0/resource", "r");
    char Memloc[20], sizeCoef[20], str3[20];

    fscanf(reslist,"%s %s %s", Memloc, sizeCoef, str3);

    long firstval = strtol(Memloc, NULL, 16);
    long secondval = strtol(sizeCoef, NULL, 16);

    long sizeval = (secondval - firstval)+1;

    int fd = open("/sys/bus/pci/devices/0000:01:00.0/resource0", O_RDWR | O_SYNC);
    void* base_address = (void*)Memloc;
    size_t size = sizeval; // 1MiB
    void* void_memory = mmap(base_address,
                         size,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_FIXED,
                         fd,
                         0);
    uint16_t* memory = (uint16_t*)void_memory;


    double readWriteTimesTotal;
    int i;
    double minReadWrite, maxReadWrite;
    minReadWrite = 10000.0;
    maxReadWrite = 0.0;
    uint16_t first_register,second_register,third_register = 0;
    int repeats = 1000000;

    double readWriteTimes[repeats];



    for (i=0; i<repeats; i++)
    {
        struct timespec start,end;


        clock_gettime(CLOCK_MONOTONIC,&start);
        //uint16_t first_register = memory[i%(1024*1024/4)];
        memory[1] = 65534;
        first_register = memory[1];
        clock_gettime(CLOCK_MONOTONIC,&end);
        memory[1] = 0;
        double timeNanoSec;

        if(start.tv_sec == end.tv_sec)
        {
            timeNanoSec = (end.tv_nsec - start.tv_nsec);
        }
        else
        {
            timeNanoSec = (end.tv_nsec - start.tv_nsec + 1000000000);

        }

        if (timeNanoSec>maxReadWrite){
        maxReadWrite = timeNanoSec;
        }

        if (timeNanoSec<minReadWrite){
        minReadWrite = timeNanoSec;
        }

        readWriteTimes[i] = timeNanoSec;

        readWriteTimesTotal = readWriteTimesTotal + timeNanoSec;

    }

    double avgReadWrite = readWriteTimesTotal/(i+1);


    printf( "Min,Mean,Max for write -> read : %.1lf %.1lf %.1lf nanoseconds\n", minReadWrite, avgReadWrite, maxReadWrite );


    FILE *fp;
    fp=fopen("output.txt","w+");
    for(i=0;i<repeats;i++)
    {
        fprintf(fp,"%.1lf\n", readWriteTimes[i]);
    }
    fclose(fp);


    return(0);

}
