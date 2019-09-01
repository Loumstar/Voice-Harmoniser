#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    double period;

    if(argc > 1){
        period = atof(argv[1]);
    } else {
        printf("Period has not been specified.\n");
        return 1;
    }

    while(1){ 
        clock_t start = clock(),
                end = clock();
        while(period > (double) (end - start) * 1000 / CLOCKS_PER_SEC){
            end = clock();
        }
        start = clock();
        printf("tick\n");
        while(period > (double) (end - start) * 1000 / CLOCKS_PER_SEC){
            end = clock();
        }
        start = clock();
        printf("tock\n");
    }
    return 0;
}