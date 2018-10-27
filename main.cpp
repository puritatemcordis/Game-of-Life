#include <cstdlib>
#include <iostream>
#include <string>
#include <signal.h>

//type defition for integer pointer
typedef int* IntPtr;

//global constant integers
const int MAX_C = 169; // max size of my terminal window
const int MAX_R = 43; //max size of my terminal window
volatile sig_atomic_t flag = 0; //used to determine state of sig_int

//Function prototypes
char Greeting();
void ClearScreen();
void PrintMatrix(IntPtr Matrix[], int row, int column);
void InitializeMatrix(IntPtr Matrix[], int row, int column);
void RandomMatrix(IntPtr Matrix[], int row, int column);
void CopyMatrix(IntPtr Matrix[], IntPtr Matrix2[], int row, int column);
bool CompareMatrix(IntPtr Matrix[], IntPtr Matrix2[], int row, int column);
bool DetermineStatus(IntPtr Matrix[], int row, int column);
void FreeArrays(IntPtr Matrix[], int row);
void Alert(int sig);

int main(){

  signal(SIGINT, Alert); //registers interruption signal

  //declaration and initialization of answer, Matrix[MAX_R], generationNum, r, c, r_cell, and c_cell
  char answer = ' ';
  IntPtr Matrix[MAX_R]; //2D dynamic array
  int r = MAX_R, c = MAX_C; //user inputted values for their rows and columns
  int r_cell = 1, c_cell = 1; //used to iterate through the rows and columns of the 2D Array
  bool status;

  ClearScreen();
  answer = Greeting(); //asks the user if they want to play the game
  while (answer == 'Y'){
    int generationNum = 1; //used to display the generation of the cell
    flag = 0; //resets flag, so everytime game restarts, game will continue

    //do while loop that asks the user to input their the row and column of the matrix; boundary check to ensure cell matrix is within the terminal display
    do {
      printf("\nEnter number of rows for the matrix (CANNOT be greater than %d.): ", 39);
      std::cin >> r;
      printf("Enter number of columns for the matrix (CANNOT be greater than %d.): ", 168);
      std::cin >> c;
    } while(r < 0 || r > MAX_R || c < 0 || c > MAX_C);

    // creates border for checking all of the cells - stops core dump for checking out of bounds
    r = r+2;
    c = c+2;

    // initializes matrix - borders and cells
    InitializeMatrix(Matrix, r, c);

    // decrement by two, so user and computer only initializes within their cells and not in the boundaries
    r = r-2;
    c = c-2;

    printf("\nWould you like to create your own matrix of cells. If not, a random generated matrix of cells will be allotted. (Y/N): ");
    std::cin >> answer;
    answer = toupper(answer);
      //conditional: if user creates own matrix then select the coordinate, else a random generator selects the cells
      if (answer == 'Y'){
        printf("\n**ENTRY WILL END IF COORDINATE IS OUT OF THE BOUNDARIES**");
        //while loops until user enters row/column that goes out of the boundaries of the row size or column size
        while ((r_cell > 0 && r_cell <= r) && (c_cell > 0 && c_cell <= c)){
          printf("\nEnter desired row coordinate for cell (CANNOT be greater than %d.): ", r);
          std::cin >> r_cell;
          printf("Enter desired column coordinate for cell (CANNOT be greater than %d.): ", c);
          std::cin >> c_cell;
          if((r_cell > 0 && r_cell <= r) && (c_cell > 0 && c_cell <= c)) Matrix[r_cell][c_cell] = 1;
        }
        // ClearScreen();
      }
      else{
        //creates cells from a random number generator
        RandomMatrix(Matrix, r, c);
        //ClearScreen();
      }

      // increments by two to add the boundaries back to the matrix
      r = r+2;
      c = c+2;

    //while loop that prints out the generation number, matrix, and the living and dead cells
    while (true){
      printf("Generation %d:\n", generationNum);
      PrintMatrix(Matrix, r, c);
      status = DetermineStatus(Matrix, r, c); //iterates through the matrix and determines which cells have been born, survived, or died
      if(status == true){
        std::cout << "From this generation onward, the matrix will not change." << std::endl;
        std::cout << "Game Over" << std::endl;
        break; //previous generation and current generation will be the same thus, game is over
      }
      std::cout << "Press any key to continue ... " << std::endl;
      std::cout << "To end program, hit \"ctrl + c\" and press any key ..." << std::endl;
      std::cin.get();
      generationNum++; //increments generation number, so user can see how many generations have passed
      //if user attempts to terminate program, dynamic array is freed and program is exited
      if(flag){
        FreeArrays(Matrix, r); //calls destructor to free the array
        break;
      }
    }

    std::cout << "Do you want to play again? (Y/N): ";
    std::cin >> answer;
    answer = toupper(answer);
    ClearScreen();
  }

  std::cout << "Goodbye!" << std::endl;

  return 0;
}


//Function Definitions

//Returns char to determine whether the user wants to play game
//Greets player and explains rules of the game
char Greeting(){
  //declaration and initialization of answer to ' '
  char answer = ' ';

  //explains the rules of Conway's Game of Life
  std::cout << "\t\t\t\t\t\t\t\t \"Welcome to the Game of Life\"" << std::endl;
  std::cout << "Rules of the Game:" << std::endl;
  std::cout << "1. Survival: Each cell that has two or three adjacent cells survives until the next generation." << std::endl;
  std::cout << "2. Death: Each cell that has at least four adjacent cells disappears (or dies) by overpopulation." << std::endl
            << "Also, each cell tht has at most one adjacent cell die by isolation." << std::endl;
  std::cout << "3. Birth: Each empty square(i.e. dead cell) that is adjacent to exactly three cells gives birth " << std::endl;
  std::cout << "to a new cell for the next generation." << std::endl;

  std::cout << std::endl;

  //while loop to ensure player either selects Y or N, which determines whether they play or not
  while (answer != 'Y' && answer != 'N'){
    std::cout << "Would you like to play? (Y/N): ";
    std::cin >> answer;
    answer = toupper(answer); //capitalizes the char inputted by user
  }

  return answer;
}


//Sends 100 newlines to the console, which is equivalent to UNIX command "clear"
//Used to show changes in matrix without stacking each display on top of one another
void ClearScreen(){
  for(int n = 0; n < 10; n++){
    std::cout << "\n\n\n\n\n\n\n\n\n\n";
  }
}

/*
Prints the matrix; value of 0 in array prints "-", value of 1 in array prints "*"
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
void PrintMatrix(IntPtr Matrix[], int row, int column){
  //for loop that loops through the 2D array and prints "-" if the value in coordinates is 0 and "*" if the value in the coordinates is 1
  for(int i = 1; i < row-1; i++){
    for(int j = 1; j < column-1; j++){
      if (Matrix[i][j] == 0) std::cout << "-";
      else if(Matrix[i][j] == 1) std::cout << "*";
    }
    std::cout << std::endl;
  }
}

/*
In the case that the user does not want to enter cells, function randomly assigns 2/5 of the matrix with values of 1
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
void RandomMatrix(IntPtr Matrix[], int row, int column){
  //declaration and initialization of matrix_size, which is equivalent to 2/5 of the size of the matrix
  int matrix_size = row*column*(2.0/5.0); //amount of characters

  //argument that passes a seed, so the random number generator generates different numbers for every time program is executed
  std::srand(time(0)); //different numbers per round

  //for loop that iterates through the sizes of the matrix and generates a random row and column that will assign the value of "1" to those random coordinates
  for(int i = 0; i <= matrix_size; i++){
    int r = (rand() % row) +1; //random number generator that goes from 0 - value of row
    int s = (rand() % column) + 1; //random number generator that goes from 0 - value of column
    Matrix[r][s] = 1; //initializes the random coordinates to "1"
  }

}

/*
Dynamically allocates memory and initializes the values of matrix to 0's
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
void InitializeMatrix(IntPtr Matrix[], int row, int column){
  //nested for loop that dynamically allocates memory and initializes values of matrix to 0's
  for(int i=0; i < row; i++){
    Matrix[i] = new int [column];
    for(int j=0; j < column; j++){
      Matrix[i][j] = 0;
    }
  }
}

/*
Copies the contents from the original Matrix to Matrix2
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param Matrix2[]: integer pointer array that has dynamically allocated memory for the values in the array, which is empty with zeros
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
void CopyMatrix(IntPtr Matrix[], IntPtr Matrix2[], int row, int column){
  for(int i = 1; i < row-1; i++){
    for(int j = 1; j < column-1; j++){
      Matrix2[i][j] = Matrix[i][j];
    }
  }
}
/*
Checks to see if both matrixes are equivalent; if they are, end the game
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param Matrix2[]: integer pointer array that has dynamically allocated memory for the values in the array, which is empty with zeros
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
bool CompareMatrix(IntPtr Matrix[], IntPtr Matrix2[], int row, int column){
  for(int i = 1; i < row-1; i++){
    for(int j = 1; j < column-1; j++){
      if(Matrix2[i][j] != Matrix[i][j]) return false;
    }
  }
  return true;
}

/*
Determines whether a cell is born, survives, or dies based on matrix values
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param row: user input of the amount of desired rows
@param column: user input of the amount of desired columns
*/
bool DetermineStatus(IntPtr Matrix[], int row, int column){
  IntPtr Matrix2[MAX_R];
  InitializeMatrix(Matrix2, row, column);
  CopyMatrix(Matrix, Matrix2, row, column);

  //nested for loop: i and j are the coordinates for the current matrix value
  for(int i = 1; i < row-1; i++){
    for(int j = 1; j < column-1; j++){
      int adjacentCells = 0; //used to determine the amount of adjacent cells around the current matrix value

      //nested for loop: k and l are the coordinates that iterate around the current matrix value and determines whether there are any adjacent cells
      for(int k = -1; k < 2; k++){
        for(int l = -1; l < 2; l++){
          if(k == 0 && l == 0) continue; //skips pass current matrix value
          else if (Matrix2[i+k][j+l] == 1) adjacentCells++; //increments adjacent cells if the matrix[k][l] = 1 (1 = cell; 0 = no cell)
        }
      }

      //conditions based upon the rules of the game
      if(Matrix2[i][j] == 0 && adjacentCells == 3) Matrix[i][j] = 1; //cell is born in the cell itself has the value of "0" and it has 3 adjacent cells around it
      else if (Matrix2[i][j] == 1 && (adjacentCells == 3 || adjacentCells == 2)) Matrix[i][j] = 1; //cell survives if the cell itself has the value of "1" and it has 2-3 adjacent cells around it
      else if (Matrix2[i][j] == 1 && (adjacentCells > 3 || adjacentCells < 2)) Matrix[i][j] = 0; //cell dies if it has more than 4 cells around it or less than 2 cells around it
      else Matrix[i][j] = 0; //default: assigns 0 to coordinate of matrix if none of the previous conditions apply
    }
  }

  return CompareMatrix(Matrix, Matrix2, row, column);
}

/*
Delete the contents of the dynamically allocated values inside the static array
@param Matrix[]: integer pointer array that has dynamically allocated memory for the values in the array, which will later be freed
@param row: user input of the amount of desired rows
*/
void FreeArrays(IntPtr Matrix[], int row){
  for(int i=0; i < row; i++){
    delete [] Matrix[i];
  }
}

void Alert(int sig){
  flag = 1;
}
