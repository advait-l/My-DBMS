#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "pds.h"
#include "footballer.h"

int id = 0;

int main(){
    while(1){
        printf("=================================\n");
        printf("INSTRUCTION CODES: \n");
        printf("\t1 : OPEN the footballer database\n");
        printf("\t2 : INSERT the footballer data\n");
        printf("\t3 : SEARCH by footballer_id\n");
        printf("\t4 : SEARCH by footballer club\n");
        printf("\t5 : MODIFY the footballer data\n");
        printf("\t6 : DELETE the footballer data\n");
        printf("\t0 : Exit the application\n");
        int input;
        printf("----------------------------------\nEnter your instruction choice : ");
        scanf("%d", &input);
        switch (input)
        {
            case 0:
                printf("Exited the application\n");
                int exit = pds_close();
                if (exit == PDS_SUCCESS) return 0;
            case 1:
                printf("Enter the database name : ");
                char database_name[30];
                scanf("%s", database_name);
                open_database(database_name);
                break;
            case 2:
                printf("Enter the footballer name to add : ");
                char name[30];
                scanf("%s", name);
                struct Footballer *f = (struct Footballer*)malloc(2*sizeof(struct Footballer));
                f->footballer_id = id++;
                srand(time(0));
                f->footballer_rating = rand()%50 + 50;
                f->goals_scored = rand()%41;
                f->assists = rand()%26;
                f->clean_sheets = rand()%16;
                f->yellow_cards = rand()%11;
                f->red_cards = rand()%6;
                f->is_banned = rand()%2;
                strcpy(f->footballer_name, name);
                char name2[30], name3[30], name4[30];
                strcpy(name2, name);
                strcpy(name3, name);
                strcpy(name4, name);
                strcpy(f->club, strcat(name2, "-club"));
                strcpy(f->nationality, strcat(name3, "-nation"));
                strcpy(f->position, strcat(name4, "-position"));
                int status = add_footballer(f);
                if(status == PDS_SUCCESS){
                    printf("STATUS : PASS :: Successfully added %s\n", name);
                }
                print_footballer(f);
                break;
            case 3:
                printf("Enter footballer id : ");
                int fid;
                scanf("%d", &fid);
                struct Footballer *fin = (struct Footballer*)malloc(2*sizeof(struct Footballer));
                fin->footballer_id = -1;
                int get_status = search_footballer(fid, fin);
                if(get_status == 0){
                    printf("STATUS : PASS :: Found footballer with id - %d\n",fid);
                }
                else{
                    printf("STATUS : FAIL :: Footballer with id - %d not found\n", fid);
                }            
                break;
            case 4:
                printf("Enter footballer-club : ");
                char club[30];
                int *io;
                scanf("%s", club);
                struct Footballer *fnin = (struct Footballer*)malloc(2*sizeof(struct Footballer));
                int nstatus = search_footballer_by_club(club, fnin, io);
                if(nstatus == PDS_SUCCESS){
                    printf("STATUS : PASS :: Found footballer with club - %s\n", club);
                }
                else printf("STATUS : FAIL :: Footballer not found for the given club\n");
                break;
            case 5:
                printf("Enter the footballer id whose name should be modified : ");
                int mid;
                scanf("%d", &mid);
                struct Footballer *mf = (struct Footballer*)malloc(2*sizeof(struct Footballer));
                int m_status = search_footballer(mid, mf);
                
                strcat(mf->footballer_name, "-modify");
                m_status = put_rec_by_key(mid, mf);
                if(m_status == PDS_SUCCESS) printf("Modified successsfully\n");
                print_footballer(mf);
                break;  
            case 6:
                printf("Enter footballer id to be deleted : ");
                int did;
                scanf("%d", &did);
                int d_status = delete_rec_by_ndx_key(did);
                if(d_status == PDS_SUCCESS) printf("Successfully deleted\n");
            default:
                break;
        }
    }
    return 0;
}
