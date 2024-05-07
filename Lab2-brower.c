#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    //need to input board size
    if (argc != 2) {
        printf("Usage: %s <board_size>\n", argv[0]);
        return 1;
    }

    //seeing if board is possible
    int board_size = atoi(argv[1]);
    if (board_size <= 0) {
        printf("Invalid board size\n");
        return 1;
    }
    
    
    key_t k = ftok("lab2", 1);
    
    
    //adding shared board arr
    int shmid_board = shmget(k, sizeof(char *) * board_size, IPC_CREAT | 0666);
    char **board = (char **)shmat(shmid_board, NULL, 0);
    for (int i = 0; i < board_size; i++) {
        int shmid_row = shmget(k + i + 1, sizeof(char) * board_size, IPC_CREAT | 0666);
        board[i] = (char *)shmat(shmid_row, NULL, 0);
    }
    
    //adding turn flag shared
    int shmid_turn = shmget(k + board_size + 1, sizeof(int), IPC_CREAT | 0666);
    int *turn = shmat(shmid_turn, NULL, 0);
    
    
    //adding win flag shared
    int shmid_win_flag = shmget(k + board_size + 2, sizeof(bool), IPC_CREAT | 0666);
    bool *win_flag = shmat(shmid_win_flag, NULL, 0);
    *win_flag = false; //Initialize win flag to false
    
    
    //adding draw flag shared
    int shmid_full_flag = shmget(k + board_size + 3, sizeof(bool), IPC_CREAT | 0666);
    bool *full_flag = shmat(shmid_full_flag, NULL, 0);
    *full_flag = false; //Initialize win flag to false


    //setting turn
    *turn = 1;
    
    //welcome
    printf("Welcome to Tic-Tac-Toe!\n");
    printf("----------------------------------------");
    sleep(1);



    //Initialize board
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            board[i][j] = '7';
        }
    }

    //formatting
    printf("\n");
    printf("----------------------------------------\n");

    //creating pid
    pid_t pid;
    
    //as long as these conditions arent met keep going
    while (!(*win_flag) && !(*full_flag)) {
        pid = fork();

        if (pid == 0) {
            //Child
            int row, col;
            char symbol = 'x';

            //do loop for making move and checking win
            do {
                
                //print out the board
                for (int i = 0; i < board_size; i++) {
                  for (int j = 0; j < board_size; j++) {
                    printf("%c ", board[i][j]);
                  }
                  printf("\n");
                }
                printf("----------------------------------------\n");
                //sleep to make it look better
                sleep(2);
                
                //stating whos turn
                printf("Player %d's turn. (%c)\n", *turn, symbol);
                
                //checking if integer
                printf("Enter a row: ");
                if (scanf("%d", &row) != 1) {
                  printf("Not an Integer! Choose again.\n");
                  while (getchar() != '\n'); 
                  sleep(1);
                  continue;
                }
                
                
                //checking if integer
                printf("Enter a column: ");
                if (scanf("%d", &col) != 1) {
                  printf("Not an Integer! Choose again.\n");
                while (getchar() != '\n'); 
                sleep(1);
                continue;
                }
                
                //reducing since its not the correct placement
                row--;
                col--;
                
                //checking already moved
                if(board[row][col] == symbol){
                  printf("You already played here! Choose again.\n");
                  sleep(1);
                }
                
                //checking already moved
                if(board[row][col] != symbol && board[row][col] != '7'){
                  printf("The other player has already played here! Choose again.\n");
                  sleep(1);
                }
                
                //checking out of bounds
                if(row > board_size || col > board_size){
                  printf("Out of bounds! Choose again.\n");
                  sleep(1);
                }
                
           
            } while (board[row][col] != '7' && *turn == 1);
            board[row][col] = symbol; //put in player move
            
            //just for showing location
            int adjustR = row+1;
            int adjustC = col+1;
            
            //show move
            printf("\nPlayer %d's played (%d, %d)\n", *turn, adjustR, adjustC);
            
                for (int i = 0; i < board_size; i++) { //print out updated board
                  for (int j = 0; j < board_size; j++) {
                    printf("%c ", board[i][j]);
                  }
                printf("\n");
                }
    

//------------------------------------------------------------------------- WIN CHECKING            
            for (int i = 0; i < board_size; i++) { //Horizontal check
              int counter = 0;
              for (int j = 0; j < board_size; j++) {
                if (board[i][j] == symbol) {
                   counter++;
                }
              }
              if (counter == board_size) { //Horizontal Win
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0); 
              }
            }
            
            for (int i = 0; i < board_size; i++) { //Vert check
              int counter = 0;
              for (int j = 0; j < board_size; j++) {
                if (board[j][i] == symbol) {
                   counter++;
                }
              }
              if (counter == board_size) { //Vertical win
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0);
              }
            }
            
            int dCounter = 0; //diag check
            for (int i = 0; i < board_size; i++) {
              if (board[i][i] == symbol) {
                dCounter++;
              }
            }
            
            if(dCounter == board_size) { //Diag win
                printf("\nI won! [%d]\n", *turn);
                printf("\n");
                *win_flag = true;
                exit(0);
              }
              
             //Diag check
            int adCounter = 0;
            for (int i = 0; i < board_size; i++) {
              if (board[i][board_size - 1 - i] == symbol) {
                adCounter++;
              }
            }        
            //diag win
            if(adCounter == board_size) {
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0);
              }  
            //draw
            int full = 0;
            for (int i = 0; i < board_size; i++) {
              for (int j = 0; j < board_size; j++) {
                if (board[i][j] == '7') {
                   full++;
                }
              }
            }  
            //draw end
            if(full == 0){
              printf("\nThe game ended in a draw.[%d]\n", 1);
              *full_flag = true;
            }
            
            //formatting
            printf("\n");
            printf("----------------------------------------\n");
            sleep(2);
            
            //switch turns
            *turn = 2;
            
            //detatching data
            shmdt(board[row]);
            shmdt(turn);
            
            
            //if win end
            if(*win_flag == true){
            exit(0);
            }
        } else if (pid > 0) {
            //Parent
            //waiting
            wait(NULL);
            do{
            //check if draw
              if(*full_flag == true){
                printf("It's a draw. [%d]\n", 2);
                exit(0);
              }
              //check if child won
              if(*win_flag == true){
                printf("I lost. [%d]\n", 2);
                exit(0);
              }
            }while(*turn !=2);    
            
            //initialize some variables
            char symbol = 'o';
            int row, col;
            
            //take turn
            do {
                
                
                
                printf("Player %d's turn. (%c)\n", *turn, symbol);
                
                printf("Enter a row: ");
                if (scanf("%d", &row) != 1) {
                  printf("Not an Integer! Choose again.\n");
                  while (getchar() != '\n'); 
                  sleep(1);
                  continue;
                }
    
                printf("Enter a column: ");
                if (scanf("%d", &col) != 1) {
                  printf("Not an Integer! Choose again.\n");
                while (getchar() != '\n'); 
                sleep(1);
                continue;
                }
                
            
                row--;
                col--;
                
                if(board[row][col] == symbol){
                  printf("You already played here! Choose again.\n");
                  sleep(1);
                }
                
                if(board[row][col] != symbol && board[row][col] != '7'){
                  printf("The other player has already played here! Choose again.\n");
                  sleep(1);
                }
                
                if(row > board_size || col > board_size){
                  printf("Out of bounds! Choose again.\n");
                  sleep(1);
                }
                
           
            } while (board[row][col] != '7' && *turn == 2  && !(*full_flag));
            board[row][col] = symbol;
            
            //same as before, copied
            int adjustR = row+1;
            int adjustC = col+1;
            
            printf("\nPlayer %d's played (%d, %d)\n", *turn, adjustR, adjustC);
            
            for (int i = 0; i < board_size; i++) {
              int counter = 0;
              for (int j = 0; j < board_size; j++) {
                if (board[i][j] == symbol) {
                   counter++;
                }
              }
              if (counter == board_size) {
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0); 
              }
            }
            
            for (int i = 0; i < board_size; i++) {
              int counter = 0;
              for (int j = 0; j < board_size; j++) {
                if (board[j][i] == symbol) {
                   counter++;
                }
              }
              if (counter == board_size) {
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0); 
              }
            }
            
            int dCounter = 0;
            for (int i = 0; i < board_size; i++) {
              if (board[i][i] == symbol) {
                dCounter++;
              }
            }
            
            if(dCounter == board_size) {
                printf("\nI won! [%d]\n", *turn);
                printf("\n");
                *win_flag = true;
                exit(0); 
              }
              
             // Check anti-diagonal
            int adCounter = 0;
            for (int i = 0; i < board_size; i++) {
              if (board[i][board_size - 1 - i] == symbol) {
                adCounter++;
              }
            }        
              
            if(adCounter == board_size) {
                printf("\nI won! [%d]\n", *turn);
              
                *win_flag = true;
                exit(0); 
              }  
            
            
            if(*full_flag == true){
            printf("It's a draw. [%d]\n", 2);
            exit(0);
            }
            
            *turn = (*turn == 1) ? 2 : 1; // Switch turn
            
            

            
     
            
        } else {
            //Error
            printf("Fork failed\n");
            return 1;
        }
    }

    //Cleanup
    for (int i = 0; i < board_size; i++) {
        shmctl(shmid_board + i + 1, IPC_RMID, NULL);
    }
    shmctl(shmid_turn, IPC_RMID, NULL);
    shmctl(shmid_win_flag, IPC_RMID, NULL);
    shmctl(shmid_full_flag, IPC_RMID, NULL);

    return 0;
}

