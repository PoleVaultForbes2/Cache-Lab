/*
    HEADER
NAME: JOSH FORBES
*/

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int s, E, b;
char *tracefile = NULL;
int hit_count = 0;
int miss_count = 0;
int evict_count = 0;

typedef struct{
    int valid;
    unsigned long tag;
    int oldest; //use to help evict (least recently used)
    //don't need block because worried about hit or miss not getting data
} Line;

void simulate_cache(Line **cache, int s, int E, int b, char op, unsigned long address, int size, bool verbose){
    //get set and tag index
    int setIndex = (address >> b) & ((1 << s) - 1);
    unsigned long tag = address >> (s + b);

    Line *set = cache[setIndex]; //looking at this set
    int hit = 0;
    int empty_line = -1;
    // indext of the oldest and we know this using a mx_oldest (simulates how long something hasn't been used)
    int oldest_index = 0;
    int max_oldest = -1;
    int evicted = 0;

    for(int i = 0; i < E; i++){
        if(set[i].valid && set[i].tag == tag){
            hit = 1;
            set[i].oldest = 0;
            hit_count++;
            break;
        }
        // save where the empty line is
        if (empty_line == -1 && !set[i].valid){
            empty_line = i;
        }
        if(set[i].oldest > max_oldest){
            //update oldest informaiton
            max_oldest = set[i].oldest;
            oldest_index = i;
        }
    }

    //if no hits
    if(hit != 1){
        miss_count++;

        int target;
        //if we have empty line make that target otherwise the oldest
        if(empty_line != -1){
            target = empty_line;
        }else { target = oldest_index;}

        //check if we are filling or replacing
        if(set[target].valid){
            evict_count++;
            evicted = 1;
        }

        //fill the line or replace with new info
        set[target].valid = 1;
        set[target].tag = tag;
        set[target].oldest = 0;
    }

    //advance the timer 
    for(int i =0; i < E; i++){
        if(set[i].valid){
            set[i].oldest++;
        }
    }

    //for verbose information
    if(verbose){
        printf("%c %lx,%d", op, address, size);
        if (hit){ 
            printf(" hit");
        }
        else printf(" miss");
        if (evicted) printf(" eviction");
        printf("\n");
    }

}

int main(int argc, char **argv)
{

    bool verbose = false;
    bool help = false;
    int arg_start = 1;

    while (arg_start < argc && argv[arg_start][0] == '-') {
        if (strcmp(argv[arg_start], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[arg_start], "-h") == 0) {
            help = true;
        } else {
            break; // stop if you hit a non-optional flag
        }
        arg_start++;
    }

    if (argc - arg_start != 8) {
        printf("Usage: %s [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
        return 1;
    }

    s = atoi(argv[arg_start + 1]);
    E = atoi(argv[arg_start + 3]);
    b = atoi(argv[arg_start + 5]);
    tracefile = argv[arg_start + 7];

    if (help) {
        printf("This program simulates a cache.\n");
        printf("Usage: %s [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
        return 0;
    }
    if(verbose){
        printf("made it verbose.\n");
    }

    FILE *fp = fopen(tracefile, "r");
    if(fp == NULL){
        fprintf(stderr, "Error opening file %s\n", tracefile);
        exit(1);
    }

    //create the cache
    Line **cache; //a pointer to an array of Line pointers

    int S = 1 << s; //get number of cache sets
    //create a point that points to the sets which is a pointer to an array of lines E
    cache = (Line **)malloc(S * sizeof(Line *));
    for(int i = 0; i < S; i++){
        cache[i] = (Line *)malloc(E * sizeof(Line));
        for(int j = 0; j < E; j++){
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].oldest = 0;
        }
    }

    char line[1000];
    while(fgets(line, sizeof(line), fp)){
        if(line[0] == 'I'){
            continue; //skip the I
        }

        char op;
        unsigned long address;
        int size;

        sscanf(line, " %c %lx, %d", &op, &address, &size);
        simulate_cache(cache, s, E, b, op, address, size, verbose);
        if(op == 'M'){
            simulate_cache(cache, s, E, b, op, address, size, verbose);
        }
    }

    for(int i = 0; i < S; i++){
        free(cache[i]);
    }
    free(cache);

    printSummary(hit_count, miss_count, evict_count);
    fclose(fp);
    return 0;
}
