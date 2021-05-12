#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

#define ROW 8
#define COL 8

#define SIDE 8
#define MAXSIDE 8

#define MINES 8
#define MAXMINES 8

#define JOYX A0
#define JOYY A1
#define BUTTON 10

#define LED 8

#define BRIGHTNESS 8
#define NUMMATRIX (ROW*COL)

CRGB myBoardColor [ROW] [COL]; 
CRGB realBoardColor[ROW] [COL]; 

CRGB notClicked = CRGB::DarkGreen;
CRGB empty = CRGB::Black;
CRGB haveBomb = CRGB::DarkBlue;
CRGB myPos = CRGB::White;

CRGB leds [NUMMATRIX]; 

CRGB mineColor [9] = {CRGB::Black, CRGB::Yellow, CRGB::Chocolate, CRGB::Red, CRGB::Magenta, CRGB::DarkRed, CRGB::DeepPink, CRGB::DarkMagenta, CRGB::FireBrick};
CRGB fieldColor[3] = {CRGB::Black, CRGB::DarkGreen, CRGB::White};
CRGB bomb = {CRGB::DarkBlue};
CRGB flag = {CRGB::Cyan}; // No flag yet... Next Version

int xMap, yMap, xValue, yValue, xPos, yPos; // Joystick Coord

// Minefield is the Led Board

FastLED_NeoMatrix *minefield = new FastLED_NeoMatrix(leds, 8, 8, 1, 1 );

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println ("Iniciando Jogo");
  FastLED.addLeds<NEOPIXEL, LED>(  leds, NUMMATRIX  );

  pinMode (BUTTON, INPUT_PULLUP);

  Serial.println ("Iniciando Tabuleiro");
  minefield->begin();
  minefield->setBrightness(BRIGHTNESS);

  Serial.println ("Fim do Setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  playMinesweeper ();
  delay(200);
}

void gameOver() {
  minefield->clear();
  for (int ray = 0; ray < 10; ray++) {
    minefield->drawCircle(xMap, yMap, ray, CRGB::Blue);
    minefield->show();
    delay(100);
  }
  delay(2000);
}


void victory() {
  minefield->clear();
  for (int ray = 0; ray < 10; ray++) {
    minefield->drawCircle(xMap, yMap, ray, CRGB::White);
    minefield->show();
    delay(100);
  }
  delay(2000);
}

bool isValid(int row, int col)
{
  // Returns true if row number and column number
  // is in range
  return (row >= 0) && (row < SIDE) &&
         (col >= 0) && (col < SIDE);
}

bool isMine (int row, int col, char board[][MAXSIDE])
{
  if (board[row][col] == '*')
    return (true);
  else
    return (false);
}

void printBoard(char myBoard[][MAXSIDE])
{
  Serial.println ("Current Status of Board :");
  int i, j;
  for (i = 0; i < SIDE; i++)
  {
    for (j = 0; j < SIDE; j++) {
      Serial.print(myBoard[i][j]);
      minefield->drawPixel(i , j , myBoardColor[i][j]); // Draw board in led array
    }
    Serial.println();
  }
  minefield->show();
  return;
}

int countAdjacentMines(int row, int col, int mines[][2],
                       char realBoard[][MAXSIDE])
{

  int i;
  int count = 0;


  if (isValid (row - 1, col) == true)
  {
    if (isMine (row - 1, col, realBoard) == true)
      count++;
  }

  if (isValid (row + 1, col) == true)
  {
    if (isMine (row + 1, col, realBoard) == true)
      count++;
  }

  if (isValid (row, col + 1) == true)
  {
    if (isMine (row, col + 1, realBoard) == true)
      count++;
  }

  if (isValid (row, col - 1) == true)
  {
    if (isMine (row, col - 1, realBoard) == true)
      count++;
  }

  if (isValid (row - 1, col + 1) == true)
  {
    if (isMine (row - 1, col + 1, realBoard) == true)
      count++;
  }


  if (isValid (row - 1, col - 1) == true)
  {
    if (isMine (row - 1, col - 1, realBoard) == true)
      count++;
  }

  if (isValid (row + 1, col + 1) == true)
  {
    if (isMine (row + 1, col + 1, realBoard) == true)
      count++;
  }

  if (isValid (row + 1, col - 1) == true)
  {
    if (isMine (row + 1, col - 1, realBoard) == true)
      count++;
  }

  return (count);
}

// A Recursive Fucntion to play the Minesweeper Game
bool playMinesweeperUtil(char myBoard[][MAXSIDE], char realBoard[][MAXSIDE],
                         int mines[][2], int row, int col, int *movesLeft)
{

  // Base Case of Recursion
  if (myBoard[row][col] != 'o') {
    return (false);
  }
  int i, j;

  // You opened a mine
  // You are going to lose
  if (realBoard[row][col] == '*')
  {
    myBoard[row][col] = '*';
    myBoardColor [row][col] = bomb;
    minefield->drawPixel(row, col,  myBoardColor [row][col] ); // Detona a mina

    for (i = 0; i < MINES; i++) {
      myBoard[mines[i][0]][mines[i][1]] = '*';
      myBoardColor[mines [i][0]] [mines[i][1]] = bomb;
      minefield->drawPixel(mines[i][0], mines[i][1], bomb ); // Cor da mina

    }
    // minefield->show();
    printBoard (myBoard);
    Serial.println ("You lost!");
    delay(2000);
    gameOver();
    return (true) ;
  }

  else
  {
    // Calculate the number of adjacent mines and put it
    // on the board
    int count = countAdjacentMines(row, col, mines, realBoard);
    (*movesLeft)--;

    myBoard[row][col] = count + '0';

    if (!count)
    {

      if (isValid (row - 1, col) == true)
      {
        if (isMine (row - 1, col, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row - 1, col, movesLeft);
      }

      if (isValid (row + 1, col) == true)
      {
        if (isMine (row + 1, col, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row + 1, col, movesLeft);
      }

      if (isValid (row, col + 1) == true)
      {
        if (isMine (row, col + 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row, col + 1, movesLeft);
      }

      if (isValid (row, col - 1) == true)
      {
        if (isMine (row, col - 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row, col - 1, movesLeft);
      }

      if (isValid (row - 1, col + 1) == true)
      {
        if (isMine (row - 1, col + 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row - 1, col + 1, movesLeft);
      }

      if (isValid (row - 1, col - 1) == true)
      {
        if (isMine (row - 1, col - 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row - 1, col - 1, movesLeft);
      }

      if (isValid (row + 1, col + 1) == true)
      {
        if (isMine (row + 1, col + 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row + 1, col + 1, movesLeft);
      }

      if (isValid (row + 1, col - 1) == true)
      {
        if (isMine (row + 1, col - 1, realBoard) == false)
          playMinesweeperUtil(myBoard, realBoard, mines, row + 1, col - 1, movesLeft);
      }
    }
    myBoardColor [row][col] = mineColor[count];
    minefield->drawPixel(mines[i][0], mines[i][1], myBoardColor [row][col] );
    //minefield->show();
    return (false);
  }
}

void placeMines(int mines[][2], char realBoard[][MAXSIDE])
{
  bool mark[MAXSIDE * MAXSIDE];

  memset (mark, false, sizeof (mark));

  // Continue until all random mines have been created.
  for (int i = 0; i < MINES; )
  {
    int random = rand() % (SIDE * SIDE);
    int x = random / SIDE;
    int y = random % SIDE;

    // Add the mine if no mine is placed at this
    // position on the board
    if (mark[random] == false)
    {
      // Row Index of the Mine
      mines[i][0] = x;
      // Column Index of the Mine
      mines[i][1] = y;

      // Place the mine
      realBoard[mines[i][0]][mines[i][1]] = '*';
      mark[random] = true;
      i++;
    }
  }

  return;
}

// A Function to initialise the game
void initialise(char realBoard[][MAXSIDE], char myBoard[][MAXSIDE])
{
  // Initiate the random number generator so that
  // the same configuration doesn't arises
  randomSeed(analogRead(5));

  // Assign all the cells as mine-free
  for (int i = 0; i < SIDE; i++)
  {
    for (int j = 0; j < SIDE; j++)
    {
      myBoard[i][j] = realBoard[i][j] = 'o';
      myBoardColor [i] [j] =  realBoardColor [i] [j] = notClicked;
      minefield->drawPixel (i, j, myBoardColor [i] [j]);
      minefield->show();
    }
  }

  return;
}

// A Function to cheat by revealing where the mines are
// placed.
void cheatMinesweeper (char realBoard[][MAXSIDE])
{
  Serial.println ("The mines locations are");
  printBoard (realBoard);
  return;
}

// A function to replace the mine from (row, col) and put
// it to a vacant space
void replaceMine (int row, int col, char board[][MAXSIDE])
{
  for (int i = 0; i < SIDE; i++)
  {
    for (int j = 0; j < SIDE; j++)
    {
      // Find the first location in the board
      // which is not having a mine and put a mine
      // there.
      if (board[i][j] != '*')
      {
        board[i][j] = '*';
        board[row][col] = 'o';
        return;
      }
    }
  }
  return;
}

// A Function to play Minesweeper game
void playMinesweeper ()
{
  // Initially the game is not over
  bool gameOver = false;

  // Actual Board and My Board
  char realBoard[ROW][COL], myBoard[ROW][COL];

  int movesLeft = SIDE * SIDE - MINES, x, y;
  int mines[MAXMINES][2]; // stores (x,y) coordinates of all mines.

  initialise (realBoard, myBoard);

  // Place the Mines randomly
  placeMines (mines, realBoard);

  /*
    If you want to cheat and know
    where mines are before playing the game
    then uncomment this part


  */
  cheatMinesweeper(realBoard);
  // You are in the game until you have not opened a mine
  // So keep playing

  int currentMoveIndex = 0;
  while (gameOver == false)
  {

    printBoard (myBoard);
    xValue = analogRead(JOYX);
    yValue = analogRead(JOYY);

    xPos = map(xValue, 0, 1024, -4, 4);
    yPos = map(yValue, 0, 1024, -4, 4);

    if (yPos == 1 || yPos == -1) {
      yPos = 0;
    }

    if (xPos == 1 || xPos == -1) {
      xPos = 0;
    }
    
    xMap = xMap + xPos;
    yMap = yMap + yPos;

    if (xMap < 0) {
      xMap = 0;
    }

    if (xMap > 7) {
      xMap = 7;
    }

    if (yMap < 0) {
      yMap = 0;
    }

    if (yMap > 7) {
      yMap = 7;
    }

    minefield->drawPixel(xMap , yMap , myPos ); // Isso aqui é que vai desenhar o tabuleiro
    minefield->show();
    delay(100);

    if (digitalRead(BUTTON) == LOW) {

      if (currentMoveIndex == 0)
      {

        if (isMine (xMap, yMap, realBoard) == true)
          replaceMine (xMap, yMap, realBoard);
      }

      currentMoveIndex ++;

      minefield->drawPixel(xMap, yMap,  myBoardColor[xMap][yMap] ); // Isso nao ta dando certo.
      //minefield->show();
      delay(100);

      gameOver = playMinesweeperUtil (myBoard, realBoard, mines, xMap, yMap, &movesLeft);
    }


    if ((gameOver == false) && (movesLeft == 0))
    {
      Serial.println ("You won !");
      delay(2000);
      victory();
      gameOver = true;
    }
  }
  minefield->show();
  return;
}
