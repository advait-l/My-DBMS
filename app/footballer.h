#ifndef FOOTBALLER_H
#define FOOTBALLER_H

#define FOOTBALLER_SUCCESS 0
#define FOOTBALLER_FAILURE 1

struct Footballer{
    int footballer_id;
    int footballer_rating; 
    int goals_scored;
    int assists;
    int clean_sheets;
    int yellow_cards;
    int red_cards;
    int is_banned;
    char footballer_name[30];
    char club[30];
    char nationality[30];
    char position[30];
};

extern struct PDS_RepoInfo *repohandle;

// Methods
// Add the given footballer into the repository by calling put_rec_by_key 
int add_footballer( struct Footballer *f);

void open_database(char* name);
// Display footballer info in a single line as a CSV without any spaces
void print_footballer(struct Footballer *f);

int search_footballer( int footballer_id, struct Footballer *f);

int search_footballer_by_club(char *club, struct Footballer *f, int *io_count);
int match_footballer_club(struct Footballer *f, char *club);



#endif