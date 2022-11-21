
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

#define BILLION  1000000000L;

int main2 ()
{

    // sudo lspci -s 01:00.0 -v
    // sudo setpci -s 01:00.0 COMMAND=0x02
    // scanf
    // open up vivado design for reference and for validation
    // look at pci express block
    // check IDs, memory block
    // in thr future, look at timing
    // linux kernel timing infrastructure for time snapshot
    FILE * reslist;



    reslist = fopen("/sys/bus/pci/devices/0000:01:00.0/resource", "r");
    char Memloc[20], sizeCoef[20], str3[20];

    fscanf(reslist,"%s %s %s", Memloc, sizeCoef, str3);
    printf("Memory address is %s\n", Memloc);
    printf("Memory size coefficient is %s\n", sizeCoef);
    printf("dont know what this is %s\n", str3);

    long firstval = strtol(Memloc, NULL, 16);
    long secondval = strtol(sizeCoef, NULL, 16);

    long sizeval = (secondval - firstval)+1;
    printf("Size is %ld\n", sizeval);

    int fd = open("/sys/bus/pci/devices/0000:01:00.0/resource0", O_RDWR | O_SYNC);
    void* base_address = (void*)Memloc;
    size_t size = sizeval; // 1MiB
    uint16_t* void_memory = mmap(base_address,
                         size,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd,
                         0);
    //uint16_t* memory = (uint16_t*)void_memory;

    //snapshot here
    struct timespec start,end;

    clock_gettime(CLOCK_MONOTONIC,&start);
    // Read the value of the first register
    uint16_t first_register = void_memory[1];

    //snapshot here
    clock_gettime(CLOCK_MONOTONIC,&end);

    double timeSec, timeNanoSec;
    timeSec = (end.tv_sec - start.tv_sec); //+ (double)(end.tv_nsec - start.tv_nsec)/(double)BILLION;
    timeNanoSec = (end.tv_nsec - start.tv_nsec);
    //printf( "Seconds for Read : %lf\n", timeSec );
    printf( "Nanoseconds for Read : %lf\n", timeNanoSec );
    printf("first register contains %d \n", first_register);

    //another snapshot here with a Write operation

    clock_gettime(CLOCK_MONOTONIC,&start);
    void_memory[1] = first_register + 1;
    clock_gettime(CLOCK_MONOTONIC,&end);

    timeSec = (end.tv_sec - start.tv_sec); //+ (double)(end.tv_nsec - start.tv_nsec)/(double)BILLION;
    timeNanoSec = (end.tv_nsec - start.tv_nsec);
    //printf( "Seconds for Write : %lf\n", timeSec );
    printf( "Nanoseconds for Write : %lf\n", timeNanoSec );
    uint16_t first_register_update = void_memory[1];
    printf("first register contains %d \n", first_register_update);

    return(0);
}
