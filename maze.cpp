#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace std;

const int MAX_ROWS = 20;
const int MAX_COLS = 20;
const char WALL = '#';
const char EMPTY = ' ';
const char PLAYER = '^';
const char PLAYERUP = '^';
const char PLAYERDOWN = 'v';
const char PLAYERRIGHT = '>';
const char PLAYERLEFT = '<';
const char SNAKE = '~';
const char GOAL = 'X';
const char OBSTACLE = 'O';
int moves = 0;

int keypress() {                        //KEYPRESS FUNCTION TO TAKE INPUT WITHOUT HITTING ENTER 

  system ("/bin/stty raw");
  int c;
  system ("/bin/stty -echo");
  c = getc(stdin);
  system ("/bin/stty echo");
  system ("/bin/stty cooked");
  return c;

}

struct Location {                     //DEFINING A STRUCTURE LOCATION TO STORE ADDRESS IN 2D ARRAY

    int row;
    int col;

};


template <typename T>                 //DEFINING A STRUCTURE NODE FOR LINKED LIST
struct Node {
    T data;
    Node* next;
};


template <typename T>                       //DEFINING A CLASS LINKEDLIST TO USE AS STACK
class LinkedList {        

    private:
    
        Node<T>* head;                      //POINTER TO HEAD OF LINKED LIST

    public:

        LinkedList() : head(nullptr) {}     //CONSTRUCTOR CALL

        void push(const T& value) {         //PUSH OPERATION OF STACK

            Node<T>* newNode = new Node<T>;
            newNode->data = value;
            newNode->next = head;
            head = newNode;

        }

        T pop() {                           //POP OPERATION OF STACK
            if (head) {

                T value = head->data;
                Node<T>* temp = head;
                head = head->next;
                delete temp;
                return value;

            } else {
                
                return T(); 

            }
        }

        bool isEmpty() {                    //CHECKING IF STACK IS EMPTY

            return head == nullptr;

        }

        ~LinkedList() {                     //DESTRUCTOR
            while (!isEmpty()) {
                pop();
            }
        }

};

//MAZE GAME CLASS

class MazeGame {
    public:

        MazeGame(const string& mapFileName); // DECLARATION OF CONSTRUCTOR TAKING MAP FILE NAME AS ARGUMENT

        void play();                        // DECLARATION OF PLAY FUNCTION

    private:
        char gameMap[MAX_ROWS][MAX_COLS];   //2D ARRAY FOR THE GAME FIELD
        int numRows;
        int numCols;
        int numObj;

        Location player;                            //STRUCTURE CONTAINING LOCATION OF PLAYER
        Location snake;                             //STRUCTURE CONTAINING LOCATION OF SNAKE
        Location goal;                              //STRUCTURE CONTAINING LOCATION OF GOAL

        LinkedList<Location> moveHistory;           //LINKEDLIST OF TEMPLATE LOCATION TO STORE MOVEHISTORY OF PLAYER
        LinkedList<Location> moveSnakeHistory;      //LINKEDLIST OF TEMPLATE LOCATION TO STORE MOVEHISTORY OF SNAKE

        void loadMap(const string& mapFileName); 
        void printMap();
        bool isValidMove(int newRow, int newCol);
        bool isGameOver();
        bool movePlayer(char move);
        bool moveSnake();
        void undo();
        bool isWin();
        
};



MazeGame::MazeGame(const string& mapFileName) {        // CONSTRUCTOR DEFINITION

    loadMap(mapFileName);                             //CALLING FUNCTION LOADMAP
   
}

void MazeGame::loadMap(const string& mapFileName) {   // LOADING MAP FILE USING FSTREAM

    ifstream mapFile(mapFileName);                    //CREATING INPUT FILE STREAM NAMED mapFile
    if (!mapFile) {                                   //IF MAPFILE IS EMPTY, DISPLAY ERROR
        cerr << "Error: File not found." << endl;
        exit(1);
    }
   
    mapFile >> numObj >> numRows >> numCols;          //READING MAP DIMENSIONS (FIRST LINE OF MAP)

   
    for (int i = 0; i < MAX_ROWS; i++) {              //INITIALIZING 2D ARRAY TO BLANK SPACE
        for (int j = 0; j < MAX_COLS; j++) {
            gameMap[i][j] = EMPTY;
        }
    }

    for (int i = 0; i < numObj; i++) {
        int r, c, code;
        mapFile >> r >> c >> code;                   //READING SECOND AND OTHER LINES OF MAP

        if (code == 0) {                             //PLAYER 

            player.row = r;                          //STORING LOCATION OF PLAYER
            player.col = c;

            gameMap[r][c] = PLAYER;                  //UPDATING IN THE 2D ARRAY

        } else if (code == 1) {                      //SNAKE

            snake.row = r;                           //STORING LOCATION OF SNAKE
            snake.col = c;

            gameMap[r][c] = SNAKE;                   //UPDATING IN THE 2D ARRAY

        } else if (code == 2) {                      //GOAL

            goal.row = r;                            //STORING LOCATION OF SNAKE
            goal.col = c;

            gameMap[r][c] = GOAL;                    //UPDATING IN THE 2D ARRAY

        } else if (code == 3) {                      //OBSTACLE

            gameMap[r][c] = OBSTACLE;                //UPDATING IN THE 2D ARRAY
            
        }
    }

    for (int i = 0; i < numRows; i++) {              //TURNING BOUNDARY OBSTACLES INTO WALL

        gameMap[i][0] = WALL;
        gameMap[i][numCols - 1] = WALL;

    }

    for (int i = 0; i < numCols; i++) {

        gameMap[0][i] = WALL;
        gameMap[numRows - 1][i] = WALL;

    }

    mapFile.close();                               //CLOSING MAP FILE 

    //2D ARRAY gameMap IS INITIALISED ACC TO GIVEN MAP

}

void MazeGame::printMap() {                                                             //PRINT MAP FUNCTION DEFINITION

    system("clear");                                                                    //CLEARING OUTPUT SCREEN FIRST

    for (int row = 0; row < numRows; ++row) {                                           //PRINTING 2D ARRAY
        for (int col = 0; col < numCols; ++col) {
            cout << gameMap[row][col]<< ' ';
        }
        cout << endl;
    }

}

bool MazeGame::isValidMove(int newRow, int newCol) {                                    //ISVALIDMOVE FUNCTION DEFINITION

    return (newRow >= 0 && newRow < numRows && newCol >= 0 && newCol < numCols &&       //CONDITION FOR NEW LOCATION TO BE VALID
            gameMap[newRow][newCol] != OBSTACLE && gameMap[newRow][newCol] != WALL);

}


bool MazeGame::movePlayer(char move) {                                                  //MOVE PLAYER FUNCTION DEFINITION

    int newRow = player.row;                                                            //STORING CURRENT PLAYER LOCATION      
    int newCol = player.col;

    if (move == 'w') newRow--;                                                          //DETERMING NEW PLAYER LOCATION
    else if (move == 's') newRow++;
    else if (move == 'a') newCol--;
    else if (move == 'd') newCol++;

    if (isValidMove(newRow, newCol)) {                                                  //CALLING ISVALIDMOVE FUNCTION

        moveHistory.push(player);                                                       //PUSH THE CURRENT PLAYER LOCATION TO STACK

        gameMap[player.row][player.col] = EMPTY;                                        //DELETING CURRENT PLAYER LOCATION

        player.row = newRow;                                                            //UPDATING PLAYER LOCATION
        player.col = newCol;

        if(move=='w')gameMap[player.row][player.col] = PLAYERUP;                        //UPDATING PLAYER FACE
        else if(move=='s')gameMap[player.row][player.col] = PLAYERDOWN;
        else if(move=='a')gameMap[player.row][player.col] = PLAYERLEFT;
        else if(move=='d')gameMap[player.row][player.col] = PLAYERRIGHT;

        return true;

    } else {

        moveHistory.push(player);                                                       //IF INVALID MOVE PUSH CURRENT PLAYER LOCATION AS IT IS

    }

    return false;
}

bool MazeGame::moveSnake() {                                                            //MOVESNAKE FUNCTION DEFINITION
    
    int newRow = snake.row;                                                             //STORING CURRENT LOCATION FO SNAKE TO UPDATE
    int newCol = snake.col;

    int i = abs(player.row - newRow) ;
    int j = abs(player.col - newCol) ;

    if(i<j){                                                                            //ALGORITHM FOR MOVING SNAKE                                             

        if(player.row < newRow && gameMap[newRow-1][newCol]!=OBSTACLE) newRow--;
        else if (player.row > newRow && gameMap[newRow+1][newCol]!=OBSTACLE) newRow++;
        else if (player.col < newCol && gameMap[newRow][newCol-1]!=OBSTACLE) newCol--;
        else if (player.col > newCol&&gameMap[newRow][newCol+1]!=OBSTACLE) newCol++;


    } else {
    
        if (player.col < newCol && gameMap[newRow][newCol-1]!=OBSTACLE) newCol--;
        else if (player.col > newCol&&gameMap[newRow][newCol+1]!=OBSTACLE) newCol++;
        else if(player.row < newRow && gameMap[newRow-1][newCol]!=OBSTACLE) newRow--;
        else if (player.row > newRow && gameMap[newRow+1][newCol]!=OBSTACLE) newRow++;

    }    

    if (isValidMove(newRow,newCol)) {                                                   //UPDATING SNAKE LOCATION

        moveSnakeHistory.push(snake);                                                   //PUSH CURRENT LOCATION OF SNACK TO STACK
        gameMap[snake.row][snake.col] = EMPTY;                                          //UPDATE THE 2D ARRAY AND SNAKE LOCATION
        snake.row = newRow;
        snake.col = newCol;
        gameMap[snake.row][snake.col] = SNAKE;

    }

    if (player.row == snake.row && player.col == snake.col) {                           //CHECKING IF SNAKE BIT THE PLAYER
        return true;
    } else {
        return false;
    }
}

void MazeGame::undo() {                                         //UNDO FUNCTION DEFINITION

    if (!moveHistory.isEmpty()) {                               //CHECKING MOVE HISTORY OF PLAYER

        Location previous = moveHistory.pop();                  //USING POP OPERATION OF STACK AND GETTING PREVIOUS OPERATION
        gameMap[player.row][player.col] = EMPTY;                //DELETING CURRENT PLAYER LOCATION FROM 2D ARRAY
        player = previous;                                      //UPDATING PLAYER LOCATION TO PREVIOUS
        gameMap[player.row][player.col] = PLAYER;               //UPDATING 2D ARRAY TO PREVIOUS PLAYER LOCATION

    }

    if (!moveSnakeHistory.isEmpty()) {                          //CHECKING MOVE HISTORY OF SNAKE

        Location prev = moveSnakeHistory.pop();                 //USING POP OPERATION OF STACK AND GETTING PREVIOUS OPERATION                
        gameMap[snake.row][snake.col] = EMPTY;                  //DELETING CURRENT PLAYER LOCATION FROM 2D ARRAY
        snake = prev;                                           //UPDATING SNAKE LOCATION TO PREVIOUS
        gameMap[snake.row][snake.col] = SNAKE;                  //UPDATING 2D ARRAY TO PREVIOUS SNAKE LOCATION

    }
}

bool MazeGame::isWin() {                                                            //ISWIN FUNCTION DEFINITION

    return (player.row == goal.row && player.col == goal.col);                      //CONDITION FOR WINNING

}

bool MazeGame::isGameOver() {
    return (player.row == snake.row && player.col == snake.col);
}

void MazeGame::play(){                                                              //PLAY FUNCTION DEFINITION

    while (true) {

        printMap();                                                                 //CALLING PRINT MAP FUNCTION

        //2D ARRAY IS PRINTED

        if (isWin()) {                                                              //CHECKING FOR WIN CODITION BY CALLING ISWIN FUNCTION

            cout << "YOU WON" << endl;

            break;                                                                  //EXITING FUNCTION AFTER WINNING

        }
        

        char move;                                                                 //TAKING CHAR AS INPUT FROM USER TO MOVE
        move = keypress() ;                                                        //CALLING KEYPRESS FUNCTION

        if (move == 'u') {         

            undo();                                                                //CALLING UNDO FUNCTION IF U IS INPUT

        } else if (move == 'w' || move == 'a' || move == 's' || move == 'd') {

            movePlayer(move);                                                       //CALLING MOVEPLAYER FUNCTION

            if (moveSnake()) {                                                      //CALLING MOVESNAKE FUNCTION

                cout << "GAME OVER" << endl;                                        //EXITING GAME IF SNAKE BIT PLAYER              
                break;

            }

        }
    }
}

int main(int argc, char *argv[]) {

    if (argc > 1) {                                 //CHECKING IF MAP FILE IS PROVIDED OR NOT

        const string mapFileName = argv[1];         //STORING FILE NAME OF MAP
        MazeGame game(mapFileName);                 //CREATING CLASS NAMED GAME AND CALLING CONSTRUCTOR

        //AFTER THIS 2D ARRAY gameMap IS INITITALISED ACC TO GIVEN MAP

        game.play();                                //CALLING PLAY FUNCTION

    } else {

        cout << "PLEASE PROVIDE MAP FILE." << endl;

    }

    return 0;

}







