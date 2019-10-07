#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "footballer.h"

int add_footballer( struct Footballer *f){
    int status; 
    status = put_rec_by_key(f->footballer_id, f);
    if(status != PDS_SUCCESS){
        printf("Unable to add player with key %d. Error %d", f->footballer_id, status);
        return FOOTBALLER_FAILURE; 
    }
    return status;
}

void open_database(char* name){
    int status = pds_open(name, sizeof(struct Footballer));
    if(status == PDS_SUCCESS){
        printf("Database \"%s\" opened\n", name);
        return;
    }
    printf("Error\n");
}

void print_footballer(struct Footballer *f){
    printf("-------------------------------------------------------------------\n");
    printf("Footballer ID: %d, Rating: %d, Goals: %d, Assists: %d, Clean-sheets: %d\nYellow cards: %d, Red cards: %d, Ban status: %d\nName: %s, Club: %s, Nationality: %s, Position: %s\n", f->footballer_id, f->footballer_rating, f->goals_scored, f->assists, f->clean_sheets, f->yellow_cards, f->red_cards, f->is_banned, f->footballer_name, f->club, f->nationality, f->position);
    printf("-------------------------------------------------------------------\n");
}

int search_footballer( int footballer_id, struct Footballer *f){
    return get_rec_by_ndx_key(footballer_id, f);
}

int search_footballer_by_club(char *club, struct Footballer *f, int *io_count){
    return get_rec_by_non_ndx_key(club, f, match_footballer_club, io_count);
}
int match_footballer_club(struct Footballer *f, char *club){
    if(strcmp(f->club, club) == 0){
        return 0;
    }
    return 1;
}
