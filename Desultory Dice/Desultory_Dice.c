#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

/* Placeholder function to avoid quitting when receiving a SIGUSR1 */
void action(){};
time_t time(time_t *second);
/* Function executed by each player, to play the dice */
void player(char *name, int playerId, int fd);
/* Function executed by the referee, to print current scores and check for a winner */
void checkWinner(int fd, char *name);
int win_score;
char name1[10], name2[10], name3[10], ch[1];
int main(int argc, char *argv[])
{

    int fd;
    pid_t pid1, pid2, pid3;
    char c;
    printf("ENTER THE NAME OF PLAYER 1:\n");
    fgets(name1, 10, stdin);
    name1[strcspn(name1, "\n")] = 0;
    printf("ENTER THE NAME OF PLAYER 2:\n");
    fgets(name2, 10, stdin);
    name2[strcspn(name2, "\n")] = 0;
    printf("ENTER THE NAME OF PLAYER 3:\n");
    fgets(name3, 10, stdin);
    name3[strcspn(name3, "\n")] = 0;
    do
    {

        printf("ENTER Y TO START THE GAME\n");
        printf("ENTER R TO OPEN THE RULEBOOK\n");
        printf("ENTER N TO EXIT THE GAME\n");
        fgets(ch, 2, stdin);

        switch (ch[0])
        {
        case 'Y':

            printf("ENTER THE WINNING SCORE :");
            scanf("%d", &win_score);
            printf("DiceGame: a 3-players game with a Arbitrator\n\n");

            // Creating the binary file before forking
            if ((fd = open("file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777)) == -1)
            {
                perror("File problem");
                exit(1);
            }
            else
            {
                // Writing three zero-integer values to the file
                int *threeZeros;
                threeZeros = malloc(3 * sizeof(int));
                threeZeros[0] = 0;
                threeZeros[1] = 0;
                threeZeros[2] = 0;
                write(fd, threeZeros, 3 * sizeof(int));
                close(fd);
                free(threeZeros);
            }
            // Creating the players and calling the common function "player"
            if ((pid1 = fork()) == 0)
                player(name1, 1, fd);
            if ((pid2 = fork()) == 0)
                player(name2, 2, fd);
            if ((pid3 = fork()) == 0)
                player(name3, 3, fd);
            sleep(1);
            signal(SIGUSR1, action);
            while (1)
            {
                // Make the players play in order: TATA, TITI then TOTO
                fd = open("file.txt", O_RDONLY);
                checkWinner(fd, name1);
                printf("Arbitrator: %s plays\n\n", name1);
                kill(pid1, SIGUSR1);
                pause();
                checkWinner(fd, name2);
                printf("Arbitrator: %s plays\n\n", name2);
                kill(pid2, SIGUSR1);
                pause();
                checkWinner(fd, name3);
                printf("Arbitrator: %s plays\n\n", name3);
                kill(pid3, SIGUSR1);
                pause();
            }
            break;

        case 'R':
             printf("\t\t\t\t\tHOW TO PLAY DESULTORY DICE:-\n\n");
            printf("1. Each player takes turns rolling the dice\n");
            printf("2. When it's your turn, you roll all three dice at the same time.\n");
            printf("3. Points are earned everytime as per the instructions mentioned below:\n");
            printf("4. The sum of the faces will be multiplied by 50 and is added to the player's score,provided  the sum is not equal to 7.\n");
            printf("5. If equal to 7 then 10%% of the winning score will be deducted from the player's score\n");
            printf("6. For three identical faces ,a bonus of 50%% of the winning score will be added.\n");
            printf("7. For any two  identical faces ,a bonus of 20%% of the winning score will be added.\n");
            printf("8. For three consecutive values ,a bonus of 25%% of the winning score will be added.\n\n\n");
            printf("\t\t\t\t\t\tOBJECTIVE:\n");
            printf("\t\t\tThe player to reach first to the winning score ,wins the game!!!!\n\n\n");
            break;

        case 'N':
            exit(0);
            break;
        }
//        printf("DO YOU WANT TO CONTINUE Y/N\n");
//        scanf("%c ",&c);
    scanf("%c ");
    } while (1);
}

void player(char *name, int playerId, int fd)
{
    srand(time(NULL) ^ (getpid() << 16));
    fd = open("file.txt", O_RDONLY);
    int points = 0, dice1, dice2, dice3, oldScore[1];

    while (1)
    {
        signal(SIGUSR1, action);
        pause();
        // Reading the old score
        if (playerId == 1) // TATA
        {
            lseek(fd, 0, SEEK_SET);
            read(fd, oldScore, sizeof(int));
            printf("%s: current score is: %d\n", name1, oldScore[0]);
        }
        else if (playerId == 2) // TITI
        {
            lseek(fd, sizeof(int), SEEK_SET);
            read(fd, oldScore, sizeof(int));
            printf("%s: current score is: %d\n", name2, oldScore[0]);
        }
        else // TOTO
        {
            lseek(fd, 2 * sizeof(int), SEEK_SET);
            read(fd, oldScore, sizeof(int));
            printf("%s: current score is: %d\n", name3, oldScore[0]);
        }
        close(fd);
        // Playing the dice and printing its own name and the dice score
        printf("%s: I'm playing my dice\n", name);
        dice1 = (int)(rand() % 6) + 1;
        dice2 = (int)(rand() % 6) + 1;
        dice3 = (int)(rand() % 6) + 1;

        printf("%s: I got %d points\n\n", name, dice1);
        printf("%s: I got %d points\n\n", name, dice2);
        printf("%s: I got %d points\n\n", name, dice3);
        printf("---------------------------------------------------------------------\n");

        int count_six = 0;
        // Updating the old score
        int old = oldScore[0];
        if ((dice1 + dice2 + dice3) != 7)
        {
            oldScore[0] = oldScore[0] + (dice1 + dice2 + dice3) * 50;
        }
        else
        {
            oldScore[0] = oldScore[0] - (0.1 * win_score);
            printf("Arbitrator: Deducting 10%% of the winning score\n\n");
            printf("---------------------------------------------------------------------\n");
        }
        if (dice1 == dice2 && dice3 == dice2)
        {
            oldScore[0] = oldScore[0] + (0.5 * win_score);
            printf("Arbitrator: Adding a bonus of 50%% of the winning score\n\n");
            printf("---------------------------------------------------------------------\n");
        }
        else if (dice3 - dice2 == 1 && dice2 - dice1 == 1)
        {
            printf("Arbitrator: Adding a bonus of 25%% of the winning score\n\n");
            printf("---------------------------------------------------------------------\n");

            oldScore[0] = oldScore[0] + (0.25 * win_score);
        }
        else if (dice1 == dice2 || dice1 == dice3 || dice2 == dice3)
        {
            printf("%s: Adding a bonus of 20%% of the winning score\n\n", name);
            printf("---------------------------------------------------------------------\n");

            oldScore[0] = oldScore[0] + (0.2 * win_score);
        }

        // Writing the new score at the same file offset
        fd = open("file.txt", O_WRONLY);
        if (playerId == 1) // TATA
        {
            lseek(fd, 0, SEEK_SET);
            write(fd, oldScore, sizeof(int));
        }
        else if (playerId == 2) // TITI
        {
            lseek(fd, sizeof(int), SEEK_SET);
            write(fd, oldScore, sizeof(int));
        }
        else // TOTO
        {
            lseek(fd, 2 * sizeof(int), SEEK_SET);
            write(fd, oldScore, sizeof(int));
        }
        close(fd);
        // Sleeping for 2 seconds and signaling the referee before pausing
        sleep(2);
        kill(getppid(), SIGUSR1);
    }
}

void checkWinner(int fd, char *name)
{
    int currentScore[1];
    // reading the new totals from sharedFile.bin
    read(fd, currentScore, sizeof(int));
    // printing player's name and total points so far
    if (strcmp(name, name1) == 0)
        printf("Arbitrator: %s's current score: ", name1);
    else if (strcmp(name, name2) == 0)
        printf("Arbitrator: %s's current score: ", name2);
    else
        printf("Arbitrator: %s's current score: ", name3);
    printf("%d\n", currentScore[0]);
    sleep(2);
    // checking if there's a winner and terminating all processes in case there is
    if (currentScore[0] >= win_score)
    {
        printf("Arbitrator: %s won the game\n", name);
        kill(0, SIGTERM);
    }
}