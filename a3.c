#include "graffit.c"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "rngs.c"
#include <math.h>
#include <time.h>

#define SIZE MAT_SIZE
/* 
 * CSCA48 A3 Tester - Andrew Aucie 

Add this line to the User struct in graffit.c:
struct friend_node_struct* userList;

Initialize user->userList = allUsers in create_user()

*/


int input, value, streamVal=0;
FriendNode *allUsers;
FILE *terminal;

void nextRandom(int value, int streamVals) {
    if (streamVals<255) {
        streamVals++;
        SelectStream(streamVals);
        return;
    }
    while (value != input) value = (long) (log(1.0 - Random()) / log(value));
    PlantSeeds(value);
    streamVals=0;
}

long randomize(long a, long b) {
    nextRandom(value, streamVal);
    return (a + (long) ((b - a + 1) * Random()));
}

void print_users(int n) {
    for (FriendNode *f=allUsers; f; f=f->next) {
        printf("%s | ", f->user->name);
        fprintf(terminal, "%s | ", f->user->name);
        if (n) {
            printf("Brands: ");
            fprintf(terminal, "Brands: ");
            for (BrandNode *b = f->user->brands; b; b = b->next) {
                printf("%s", b->brand_name);
                fprintf(terminal, "%s", b->brand_name);
                if (b->next) {
                    printf(", ");
                    fprintf(terminal, ", ");
                }
            }
            printf("\n");
            fprintf(terminal, "\n");
        }
        else {
            for (FriendNode *p = f->user->friends; p; p = p->next) {
                printf("%s", p->user->name);
                fprintf(terminal, "%s", p->user->name);
                if (p->next) {
                    printf(", ");
                    fprintf(terminal, ", ");
                }
            }
            printf("\n");
            fprintf(terminal, "\n");
        }
    }
    printf("\n");
    fprintf(terminal, "\n");
}
int main() {
    terminal = fopen("output.txt", "w+");
    int totalUsers;
    printf("Enter number of users: ");
    scanf("%d",&totalUsers);
    printf("\nEnter randomizer value (<0 for random): ");
    scanf("%d",&value);
    printf("\nMatrix Size: %d, Users: %d, Randomizer: %d\n", SIZE, totalUsers, value);
    fprintf(terminal,"Matrix Size: %d, Users: %d, Randomizer: %d\n\n", SIZE, totalUsers, value);
    input = value;
    PlantSeeds(value);
    int brand_matrix[SIZE][SIZE];
    char brand_names[SIZE][2], brand_string[1024]="";
    for (int i=0; i<SIZE; i++) {
        brand_names[i][0] = i+65;
        brand_names[i][1] = '\0';
        for (int j=0; j<SIZE; j++) {
            randomize(1,10)>4? (brand_matrix[i][j]=1):(brand_matrix[i][j]=0);
        }
    }
    for (int i=0; i<SIZE; i++) {
        int n1 = randomize(0, SIZE-1);
        int n2 = randomize(0, SIZE-1);
        if (n1 != n2) {
            char tmp[1024];
            strcpy(tmp, brand_names[n1]);
            strcpy(brand_names[n1], brand_names[n2]);
            strcpy(brand_names[n2], tmp);
        }
    }
    for (int i=0; i<SIZE; i++) {
        strcat(brand_string, brand_names[i]);
        if (i != SIZE-1) strcat(brand_string, ",");
        brand_matrix[i][i]=0;
        for (int j=0; j<SIZE; j++) {
            brand_matrix[i][j] = brand_matrix[j][i];
        }
    }
    FILE *f = fopen("brandtest.txt","w+");
    fprintf(f, "%s\n",brand_string);
    for (int i=0; i<SIZE; i++) {
        char num_string[1024]="";
        for (int j=0; j<SIZE; j++) {
            brand_matrix[i][j]? (strcat(num_string, "1")):(strcat(num_string, "0"));
            if (j != SIZE-1) strcat(num_string, ",");
        }
        fprintf(f, "%s\n", num_string);
    }
    fclose(f);
    populate_brand_matrix("brandtest.txt");
    printf("\n-   ");
    fprintf(terminal, "-   ");
    for (int i=0; i<SIZE; i++) {
        printf("%s ",brand_names[i]);
        fprintf(terminal, "%s ",brand_names[i]);
    }
    printf("\n");
    fprintf(terminal, "\n");
    for (int i=0; i<SIZE; i++) {
        printf("%s | ",brand_names[i]);
        fprintf(terminal, "%s | ",brand_names[i]);
        for (int j=0; j<SIZE; j++) {
            printf("%d ",brand_matrix[i][j]);
            fprintf(terminal, "%d ",brand_matrix[i][j]);
        }
        printf("\n");
        fprintf(terminal, "\n");
    }
    for (int i=0; i<totalUsers; i++) {
        char name[1024] = "u";
        char tmp_num[1024];
        sprintf(tmp_num, "%d", i);
        strcat(name, tmp_num);
        User *tmp_user = create_user(name);
        allUsers = tmp_user->userList;
    }
    int totalSuggested, followed;
    char tmp_brand[]="";
    tmp_brand[1]='\0';
    for (int i=0; i<SIZE; i++) {
        tmp_brand[0] = i+65;
        for (FriendNode *f=allUsers; f; f=f->next) {
            if (randomize(1,10)>8) follow_brand(f->user, tmp_brand);
        }    
    }
    printf("\nBrand List:\n");
    fprintf(terminal, "\nBrand List:\n");
    print_users(1);
    printf("\nSUGGESTED BRANDS TEST: follow_suggested_brands(user, n)\n");
    fprintf(terminal, "\nSUGGESTED BRANDS TEST: follow_suggested_brands(user, n)\n");
    for (FriendNode *f=allUsers; f; f=f->next) {
        if (randomize(1,10)>6) {
            totalSuggested = randomize(SIZE*0.1, SIZE*0.5);
            followed = follow_suggested_brands(f->user, totalSuggested);
            printf("(%s, %d) - Suggested Brands Followed: %d\n",f->user->name, totalSuggested, followed);
            fprintf(terminal, "(%s, %d) - Suggested Brands Followed: %d\n",f->user->name, totalSuggested, followed);
        }
    }
    printf("\nUpdated Brand List:\n");
    fprintf(terminal, "\nUpdated Brand List:\n");
    print_users(1);
    for (FriendNode *u=allUsers; u; u=u->next) {
        for (FriendNode *f=allUsers; f; f=f->next) {
            if (randomize(1,10)>8) add_friend(u->user, f->user);
        }    
    }
    printf("Friends List:\n");
    fprintf(terminal, "Friends List:\n");
    print_users(0);
    printf("\nSUGGESTED FRIENDS TEST: add_suggested_friends(user, n)\n");
    fprintf(terminal, "\nSUGGESTED FRIENDS TEST: add_suggested_friends(user, n)\n");
    for (FriendNode *f=allUsers; f; f=f->next) {
        if (randomize(1,10)>6) {
            totalSuggested = randomize(totalUsers*0.1, totalUsers*0.6);
            followed = add_suggested_friends(f->user, totalSuggested);
            printf("(%s, %d) - Suggested Friends Added: %d\n",f->user->name, totalSuggested, followed);
            fprintf(terminal, "(%s, %d) - Suggested Friends Added: %d\n",f->user->name, totalSuggested, followed);
        }
    }
    printf("\nUpdated Friends List:\n"); 
    fprintf(terminal, "\nUpdated Friends List:\n"); 
    print_users(0);
    printf("\nDEGREES OF CONNECTION TEST: get_degrees_of_connection(user A, user B)\n");
    fprintf(terminal, "\nDEGREES OF CONNECTION TEST: get_degrees_of_connection(user A, user B)\n");
    for (FriendNode *u=allUsers; u; u=u->next) {
        for (FriendNode *f=allUsers; f; f=f->next) {
            if (randomize(1,10)==10) {
                int degree = get_degrees_of_connection(u->user, f->user);
                printf("(%s, %s): %d\n", u->user->name, f->user->name, degree);
                fprintf(terminal, "(%s, %s): %d\n", u->user->name, f->user->name, degree);
            }
        }    
    }
    fclose(terminal);
    return 0;
}
