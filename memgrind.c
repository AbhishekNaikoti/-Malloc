/**
 * Assignment 1: ++Malloc (memgrind.c)
 * 
 * @author Taran Suresh ts875
 * @author Abhishek Naikoti an643
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "mymalloc.h"

/**
 * Memgrind workload function that will malloc() 1 byte and immediately free it. 
 * Calculates runtime using gettimeofday from sys/time.h library.
 * @param[in] number of iterations workloadA should run
 * @param[out] runtime for workloadA
 */ 
long workloadA (int numAIterations) {
    struct timeval start;
    struct timeval end;
    char* c; // one byte that will be malloced and immediately freed

    if (numAIterations < 0) {
        return 0;
    }

    gettimeofday(&start, NULL); // getting current time of day
    for (int i = 0; i < numAIterations; i++) {
        c = malloc(1);
        free(c);
    }
    gettimeofday(&end, NULL); // store end time after running loop

    return (end.tv_usec - start.tv_usec); // returning end time - start time in microseconds
}

/**
 * Memgrind workload function that will malloc() 1 byte and store each pointer
 * in an array. Then will free all pointers in that array subsequently.
 * Calculates runtime using gettimeofday from sys/time.h library.
 * @param[in] number of iterations workloadB should run
 * @param[out] runtime for workloadB
 */ 
long workloadB (int numBIterations) {
    struct timeval start;
    struct timeval end;
    char* pointers[numBIterations]; // array storing pointers to individual bytes

    if (numBIterations < 0) {
        return 0;
    }

    gettimeofday(&start, NULL); // getting current time of day
    // malloc 120 pointers to one byte
    for (int i = 0; i < numBIterations; i++) {
        pointers[i] = malloc(1);
    }

    // free the 120 pointers to their respective bytes
    for (int i = 0; i < numBIterations; i++) {
        free(pointers[i]);
    }
    gettimeofday(&end, NULL); // store end time after running malloc/free

    return (end.tv_usec - start.tv_usec); // returning end time - start time in microseconds
}

/**
 * Memgrind workload function that will randomly choose between a 1 byte malloc() or free(). Ensures that free() will only be called
 * if there are malloced pointers already stored in array. Keeps track of a max limit of pointers able to be malloced.
 * Calculates runtime using gettimeofday from sys/time.h library.
 * @param[in] number of iterations workloadC should run
 * @param[out] runtime for workloadC
 */
long workloadC (int numCIterations) {
    struct timeval start;
    struct timeval end;
    char* pointers[120]; // array storing pointers to individual bytes
    int pointersIndex = 0; // this index should not exceed 120 (max: 119)
    int currMalloced = 0; // keeps track of the number of currently malloced pointers to check if free can be called
    int randomNum = 0; // malloc = 0, free = 1
    int freeIndex = 0; // next pointer to be freed in array

    gettimeofday(&start, NULL); // getting current time of day
    for (int i = 0; i < numCIterations; i++) {
        // within the for loop, generate a random number and decide between malloc or free
        randomNum = (rand() % 2); // randomNum will always be 0 or 1 since those are the only possible remainders when modulo 2
        
        if (randomNum == 0) { // malloc
            if (pointersIndex < 120) { // there are still remaining spots in array to be filled with malloc(1) pointers
                pointers[pointersIndex] = malloc(1);
                pointersIndex++;
                currMalloced++;
            } else { // if maximum number of malloc(1)'s have occured, then just free
                free(pointers[freeIndex]);
                freeIndex++;
                currMalloced--;
            }
        } else if (randomNum == 1) { // free
            if (currMalloced >= 1) { // there are pointers to be freed
                free(pointers[freeIndex]);
                freeIndex++;
                currMalloced--;
            } else { // there are not any pointers to be freed, so malloc(1)
                if (pointersIndex < 120) { // only malloc(1) if there is still space in pointers array
                    pointers[pointersIndex] = malloc(1);
                    pointersIndex++;
                    currMalloced++;
                }

                // if not, then we are probably at the end of the 240 iterations
            }
        }
    }
    gettimeofday(&end, NULL); // store end time after running malloc/free

    return (end.tv_usec - start.tv_usec); // returning end time - start time in microseconds
}

/**
 * Memgrind workload function that will malloc() 1 byte and immediately free it, similar to that of workload A. It also has 
 * an added feature that will randomly choose two arbitrary numbers and see if they are equal, which then 
 * will try to free an invalid pointer. This feature showcases how free() gracefully handles invalid pointers.
 * Calculates runtime using gettimeofday from sys/time.h library.
 * @param[in] number of iterations workloadD should run
 * @param[out] runtime for workloadD
 */ 
long workloadD (int numDIterations) {
    struct timeval start;
    struct timeval end;
    char* c; // one byte that will be malloced and immediately freed

    /* using two random numbres limits the number of times they will be equal to keep the number of errors printed to a moderate amount */
    int randomNum1 = 0;
    int randomNum2 = 0;

    gettimeofday(&start, NULL); // getting current time of day
    for (int i = 0; i < numDIterations; i++) {
        randomNum1 = (rand() % 5);
        randomNum2 = (rand() % 5);
        c = malloc(1);
        free(c);
    
        if (randomNum1 == randomNum2) { // tries to randomly free a pointer an arbitrary number of bytes away from char pointer c
            free(c + 2);
        }

    }
    gettimeofday(&end, NULL); // store end time after running loop

    return (end.tv_usec - start.tv_usec); // returning end time - start time in microseconds
}

/**
 * Memgrind workload function that showcases a multitude of malloc() and free() features. It will malloc() three chunks of 200 bytes (as available in memory) at a time, 
 * and then will free two adjacent blocks at a time. This will showcase how the MetaData linked list coalesces adjacent free blocks, which can be seen visually by calling
 * the printMetaData() function. Also, it will try to allocate more memory than available, which shows how malloc() handles being requested more memory than available.
 * Calculates runtime using gettimeofday from sys/time.h library.
 * @param[out] runtime for workloadE
 */ 
long workloadE () {
    struct timeval start;
    struct timeval end;
    char *ptr1;
    char *ptr2;
    char *ptr3;
    char *ptr4;

    gettimeofday(&start, NULL); // getting current time of day

    ptr1 = (char*) malloc(200);
    ptr2 = (char*) malloc(200);
    ptr3 = (char*) malloc(200);

    /* if calling printMetaData(), refer to main function and uncomment workloadE section code for a cleaner look to output screen */

    /* call (uncomment) printMetaData here if a visual prior to coalescing would like to be seen */
    // printMetaData();
    free(ptr1);
    free(ptr2);
    ptr4 = (char*) malloc(300); // when mallocing these 300 bytes, coalesce() will be called, where in the userdata from ptr1 and ptr2 will be combined
    /* call (uncomment) printMetaData here if a visual after coalescing and mallocing 300 bytes would like to be seen */
    // printMetaData();

    /* malloc() feature that prints an error when more than available memory is requested */
    ptr1 = (char*) malloc(4080);

    /* freeing these two pointers to clean up myblock for next workload call */
    free(ptr3);
    free(ptr4);

    gettimeofday(&end, NULL); // store end time after running workload
    return (end.tv_usec - start.tv_usec); // returning end time - start time in microseconds
}

/**
 * Helper function used to calculate the mean time for each workload A, B, C, D, E.
 * @param[in] array of workload times for each respective workload
 * @param[in] number of iterations each workload is run
 * @param[out] mean time for respective workload
 */ 
long calculateMeanTime (long *workloadTimes, int numWorkloadIterations) {
    long sum = 0;

    for (int i = 0; i < numWorkloadIterations; i++) {
        sum += workloadTimes[i];
    }

    return (sum / numWorkloadIterations);
}

/**
 * Main function is the entry point into the program. The user is able to grind the preset workloads to see the
 * mean run time of each. They are also able to create their own workloads, and test the malloc() and free() functions.
 * Lastly, they are able to print out the simulated memory, as well as the MetaData linked list using functions
 * printMemory() and printMetaData() to see how the memory is being handled after each call to malloc() and free() functions.
 */
int main (int argc, char** argv) {
    int numWorkloadIterations = 50; // defined in specification

    /* arrays storing each workloads run times to calculate mean after populated */
    long workloadATimes[numWorkloadIterations];
    long workloadBTimes[numWorkloadIterations];
    long workloadCTimes[numWorkloadIterations];
    long workloadDTimes[numWorkloadIterations];
    long workloadETimes[numWorkloadIterations];

    /* running each workload numWorkloadIterations times one after the other */
    for (int i = 0; i < numWorkloadIterations; i++) {
        workloadATimes[i] = workloadA(120);
        workloadBTimes[i] = workloadB(120);
        workloadCTimes[i] = workloadC(240);                
        workloadDTimes[i] = workloadD(120);
        workloadETimes[i] = workloadE();
    }

    /* print statements consecutively after calling workloads in case errors occur, which will be printed before mean times */
    printf("----------------------------------------------------\n");
    printf("Mean Workload A Runtime: %lu microseconds\n", calculateMeanTime(workloadATimes, numWorkloadIterations));    
    printf("Mean Workload B Runtime: %lu microseconds\n", calculateMeanTime(workloadBTimes, numWorkloadIterations));    
    printf("Mean Workload C Runtime: %lu microseconds\n", calculateMeanTime(workloadCTimes, numWorkloadIterations));    
    printf("Mean Workload D Runtime: %lu microseconds\n", calculateMeanTime(workloadDTimes, numWorkloadIterations));    
    printf("Mean Workload E Runtime: %lu microseconds\n", calculateMeanTime(workloadETimes, numWorkloadIterations));    
    printf("----------------------------------------------------\n");


    /* uncomment code below if calling printMetaData() from workloadE for a cleaner look, as well as comment prior call to workloadE in above code */
    // for (int i = 0; i < numWorkloadIterations; i++) {
    //     workloadETimes[i] = workloadE();
    // }
    // printf("Mean Workload E Runtime: %lu microseconds\n", calculateMeanTime(workloadETimes, numWorkloadIterations));    

    return EXIT_SUCCESS;
}