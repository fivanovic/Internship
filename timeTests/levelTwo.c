
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

#define BILLION  1000000000L;


int main545 ()
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
                         MAP_SHARED,
                         fd,
                         0);
    uint16_t* memory = (uint16_t*)void_memory;
    double readTimesTotal, writeTimesTotal;
    int i;
    double minRead, maxRead, minWrite, maxWrite;
    minRead = 10000.0;
    maxRead = 0.0;
    minWrite = 10000.0;
    maxWrite = 0.0;

    int repeats = 1000000;

    double readTimes[repeats];
    double levelTwoRes[10000];
    levelTwoRes[0] = 0;

    //double wTimes[repeats];
    int j = 1;
    double flagnsec = 0;
    double flagsec = 0;
    for (i=0; i<repeats; i++)
    {
        struct timespec start,end,levTwo;

        clock_gettime(CLOCK_MONOTONIC,&start);
        //uint16_t first_register = memory[i%(1024*1024/4)];
        uint16_t first_register = memory[1];
        clock_gettime(CLOCK_MONOTONIC,&end);

        double timeNanoSec;

        if(start.tv_sec == end.tv_sec)
        {
            timeNanoSec = (end.tv_nsec - start.tv_nsec);
        }
        else
        {
            timeNanoSec = (end.tv_nsec - start.tv_nsec + 1000000000);

        }

        //printf("%.1lf\n", timeNanoSec);

        if (timeNanoSec>maxRead){
        maxRead = timeNanoSec;
        }

        if (timeNanoSec<minRead){
        minRead = timeNanoSec;
        }

        readTimes[i] = timeNanoSec;

        readTimesTotal = readTimesTotal + timeNanoSec;

        if(timeNanoSec > 1400 && timeNanoSec < 2500)
        {
          clock_gettime(CLOCK_MONOTONIC,&levTwo);
          double tempnsec = levTwo.tv_nsec; //+ (levTwo.tv_nsec/1000000000);
          double tempsec = levTwo.tv_sec;
          //levelTwoRes[j] = i - levelTwoRes[j-1];
          if(tempsec == flagsec)
        {
            levelTwoRes[j] = tempnsec - flagnsec;
        }
        else
        {
            levelTwoRes[j] = (tempnsec - flagnsec) + (tempsec - flagsec) * 1000000000;

        }
        flagsec = tempsec;
        flagnsec = tempnsec;
        j = j + 1;
          /*
          if
          levelTwoRes[j] = temp - flag1;

          if(levelTwoRes[j] < 0)
          {
            //levelTwoRes[j] = levelTwoRes[j] + 1000000000;
            levelTwoRes[j] = levelTwoRes[j];
          }

          flag1 = temp;
          j = j+1;
            */
        }

        clock_gettime(CLOCK_MONOTONIC,&start);
        //memory[i%(1024*1024/4)] = first_register + i;
        memory[1] = first_register + 1;
        clock_gettime(CLOCK_MONOTONIC,&end);

        if(start.tv_sec == end.tv_sec)
        {
        timeNanoSec = (end.tv_nsec - start.tv_nsec);
        }
        else
        {
            timeNanoSec = (end.tv_nsec - start.tv_nsec + 1000000000);
        }

        if (timeNanoSec>maxWrite){
        maxWrite = timeNanoSec;
        }

        if (timeNanoSec<minWrite){
        minWrite = timeNanoSec;
        }

        //writeTimes[i] = timeNanoSec;

        writeTimesTotal = writeTimesTotal + timeNanoSec;
        //printf("%d\n", i);

        struct timespec rqtp, rmtp  = {0,500};

        //nanosleep(&rqtp,&rmtp);

    }

    //write anomaly investigation program
    double avgRead = readTimesTotal/(i+1);
    double avgWrite = writeTimesTotal/(i+1);

    printf( "Min,Mean,Max for read : %.1lf %.1lf %.1lf nanoseconds\n", minRead, avgRead, maxRead );
    printf( "Min,Mean,Max for write : %.1lf %.1lf %.1lf nanoseconds\n", minWrite, avgWrite, maxWrite);


    FILE *fp;
    fp=fopen("output.txt","w+");
    for(i=0;i<repeats;i++)
    {
        fprintf(fp,"%.1lf\n", readTimes[i]);
    }
    fclose(fp);

    int levTwoReps = j;

    FILE *fp1;
    fp1=fopen("outputlevTwo.txt","w+");
    for(i=0;i<levTwoReps;i++)
    {
        fprintf(fp1,"%.4lf\n", levelTwoRes[i]);
    }
    fclose(fp1);

    return(0);

}
