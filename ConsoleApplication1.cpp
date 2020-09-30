#include <iostream>
#include <conio.h>
#include <ctype.h>

//BEGIN CODE REQUIRED FOR GRAPHICS
//#include "stdafx.h"
//#include <gdiplus.h>
#include <windows.h>
//#include <objidl.h>
//using namespace Gdiplus;
//#pragma comment (lib,"Gdiplus.lib")
//END CODE REQUIRED FOR GRAPHICS

using namespace std;

const int NUMBEROFPLAYERS	= 2;
const int BOARD_HEIGHT		= 20;
const int BOARD_WIDTH		= 20;  //Size of total board
const int BOARD_SIZE		= BOARD_WIDTH * BOARD_HEIGHT;
const int GLOBALSUPPLYCAP	= 100;		//Global variables declarations

//Implemenent "window" to follow cursor and minimize print size
const int WINDOW_HEIGHT = 5;
const int WINDOW_WIDTH = 5;
//This is an edit.



//Attacker vs defender matrix. Attacker determines row number, while defender determines column number.
//In order they are Infantry, Armor, Artillery, Cavalry, and Rocket.
												 // I   T    A    C    R
const double ATTACK_VALUES_MATRIX[5][5] = {		0.50, 0.05,0.10,0.10,0.25,
												0.65,0.50,0.60,0.60,0.70,
												0.60,0.40,0.50,0.55,0.60,
												0.60,0.10,0.20,0.35,0.45,
												0.80,0.60,0.65,0.70,0.80};
char Input					= ' ';	
class Minion;
int turnFlag				= 1;
string eventText = "";




double consultAttackValuesChart(char attackerType, char defenderType)			//Assign numeric values for different units to access attack values matrix easier.
{
	int x, y;
	switch (defenderType)
	{
	case('i'):
		x = 0;
		break;
	case('T'):
		x = 1;
		break;
	case('A'):
		x = 2;
		break;
	case('C'):
		x = 3;
		break;
	case('R'):
		x = 4;
		break;
	}

	switch (attackerType)
	{
	case('i'):
		y = 0;
		break;
	case('T'):
		y = 1;
		break;
	case('A'):
		y = 2;
		break;
	case('C'):
		y = 3;
		break;
	case('R'):
		y = 4;
		break;
	}


	return ATTACK_VALUES_MATRIX[y][x];
}

bool isAdjacent(int input1, int input2) 
{
	if (abs(input1 - input2) == 1)		//Horizontally adjacent
		return true;
	if (abs(input1 - input2) == BOARD_WIDTH)		//Vertically adjacent
		return true;

	return false;
}

class cursor 
{	public:
	bool selectMinionFlag = false;
	Minion*  selectMinionPointer;
	int Location;
	int getX() { return Location % BOARD_WIDTH; }
	int getY() { return Location / BOARD_WIDTH; }
}cursor;

class tile 
{public:
	char symbol;
	bool hasMinionOnTop;
	Minion* minionOnTop;
	bool withinRange;
	string description;
};

class MasterBoard
{
public:
	MasterBoard();
	int selectMinion(int inputLocation);
	int moveMinion(int inputLocation);
	int deselectMinion();
	int createMinion(char inputType, int inputLocation, int inputTeam);
	tile Board[BOARD_SIZE];
	int setRangeField(int inputLocation, int inputRange);
	int setAttackField(int inputLocation, int inputRange);
	int endTurn();
	int attackMinion(int inputLocation);
	int destroyMinion(Minion * inputMinion);
	int windowLocation;
	int checkWindow();

}GameBoard;

class Minion
{	public:
	int addMinion(int inputSeniority, int inputLocation, char inputType, int inputTeam);
	Minion();
	bool isAlive;
	int Location;
	char type;
	bool isMinionSelected;
	int movementRange;
	int attackRange;
	string description;
	int team;
	bool hasMoved;
	bool hasAttacked;
	double health;
	bool artilleryCanAttack;
}minionRoster[GLOBALSUPPLYCAP];

Minion::Minion()
{
	Location = -1;
	type = ' ';
	isMinionSelected = false;
	team = 0;						//Team 0 is the neutral team.
	hasAttacked = false;
	hasMoved = false;
	health = 100;
	isAlive = false;
	
}

int Minion::addMinion(int inputSeniority, int inputLocation, char inputType, int inputTeam)
{
	Location = inputLocation;
	type = inputType;
	switch (inputType)
	{
	case('i'):
	{		
		description = "Infantry";
		movementRange = 3;
		attackRange = 1;
		break;
	}
	case('T'):
	{
		description = "Tank";
		movementRange = 6;
		attackRange = 1; 
		break;
	}
	case('A'):
	{
		description = "Artillery";
		movementRange = 5;
		attackRange = 3;
		break;
	}
	case('C'):
	{
		description = "Cavalry";
		movementRange = 8;
		attackRange = 1;
		break; 
	}
	case('R'): 
	{ 
		description = "Rocket";
		movementRange = 4;
		attackRange = 5;
		break; 
	}
	}

	team = inputTeam;
	GameBoard.Board[inputLocation].minionOnTop = &minionRoster[inputSeniority];
	GameBoard.Board[inputLocation].hasMinionOnTop = true;
	health = 100;
	isAlive = true;
	return 0;
}

MasterBoard::MasterBoard() 
{
	for (int i = 0; i < BOARD_SIZE; i++)			//Initialize with clear terrain. Need to fill with other terrain types.
	{
		Board[i].symbol = '.';
		Board[i].description = "Clear terrain.";
		Board[i].hasMinionOnTop = false;
		Board[i].withinRange = false;
	}

}

int MasterBoard::checkWindow() 
{
	if (cursor.getX() == (windowLocation % BOARD_WIDTH) && cursor.getX() != 0)						//If the cursor moves to the left edge of the window AND it's not at the edge of the board
		windowLocation--;																			//Shift the window left

	if (cursor.getX() == ((windowLocation % BOARD_WIDTH) + WINDOW_WIDTH - 1)   && windowLocation % BOARD_WIDTH + WINDOW_WIDTH != BOARD_WIDTH)			//If the cursor moves to the right edge of the window AND it's not at the edge of the board
		windowLocation++;																																//Shift the window to the right
	
	if (cursor.getY() == (windowLocation / BOARD_WIDTH) && cursor.getY() != 0)						//If the cursor moves to the top edge of the window AND it's not at the edge of the board
		windowLocation -= BOARD_WIDTH;																//Shift the window up.

	if (cursor.getY() == ((windowLocation / BOARD_WIDTH) + WINDOW_HEIGHT- 1) && (windowLocation / BOARD_WIDTH + WINDOW_HEIGHT != BOARD_HEIGHT))				//If the cursor moves to the bottom of the window AND it's not at the bottom of the board
		windowLocation += BOARD_WIDTH;																														//Shift the window down once.

	return 0;
}

int MasterBoard::setRangeField(int inputLocation, int inputRange) 
{



	if (Board[inputLocation].hasMinionOnTop == true && Board[inputLocation].minionOnTop->team != turnFlag) 
	{ 
		return 0; 
	}

	if (inputRange == 0)									//If this is the edge of the range, set within range and return.
	{ 
		Board[inputLocation].withinRange = true; 
		return 0; 
	}		

	inputRange--;
	Board[inputLocation].withinRange = true;
	
//Otherwise, perform function on all adjacent spaces without enemies.				
//Recursion on each direction, with IF statements to prevent us from leaving the matrix.
	if (inputLocation % BOARD_WIDTH != 0)					setRangeField(inputLocation - 1, inputRange);
	if (inputLocation % BOARD_WIDTH != BOARD_WIDTH - 1)		setRangeField(inputLocation + 1, inputRange);
	if (inputLocation < (BOARD_HEIGHT- 1) * (BOARD_WIDTH))		setRangeField(inputLocation + BOARD_WIDTH, inputRange);
	if (inputLocation > BOARD_WIDTH)						setRangeField(inputLocation - BOARD_WIDTH, inputRange);
	
	return 0;
}

int MasterBoard::setAttackField(int inputLocation, int inputRange)		//Primary difference between move and attack is attack range goes over all units, ignoring them.
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		int inputLocationX = inputLocation % BOARD_WIDTH;
		int inputLocationY = (inputLocation - (inputLocation % BOARD_WIDTH)) / BOARD_WIDTH;	//Convert input coordinate to x and y.

		int iX = i % BOARD_WIDTH;
		int iY = (i - (i % BOARD_WIDTH)) / BOARD_WIDTH;	//Do the same with i.

		int distanceX = abs(inputLocationX - iX);
		int distanceY = abs(inputLocationY - iY);

		if ((distanceX + distanceY) <= Board[inputLocation].minionOnTop->attackRange)
			Board[i].withinRange = true;
		else Board[i].withinRange = false;
	}
	return 0;

}

int MasterBoard::createMinion(char inputType, int inputLocation, int inputTeam)
{
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
		if (minionRoster[i].isAlive == false)
		{
			minionRoster[i].addMinion(i, inputLocation, inputType, inputTeam);
			i = GLOBALSUPPLYCAP;
		}

	return 0;

}

int MasterBoard::selectMinion(int inputLocation) 
{
	if (Board[inputLocation].hasMinionOnTop == true && Board[inputLocation].minionOnTop->team == turnFlag) 
	{
		Board[inputLocation].minionOnTop->isMinionSelected = true;
		cursor.selectMinionPointer = Board[inputLocation].minionOnTop;
		cursor.selectMinionFlag = true;
		if (cursor.selectMinionPointer->hasMoved == false)
			setRangeField(inputLocation, cursor.selectMinionPointer->movementRange);
		else if (cursor.selectMinionPointer->hasAttacked == false && (cursor.selectMinionPointer->artilleryCanAttack == true || (cursor.selectMinionPointer->type != 'A' && cursor.selectMinionPointer->type != 'R')))	
			setAttackField(inputLocation, cursor.selectMinionPointer->attackRange);
	}
	return 0;
}

int MasterBoard::moveMinion(int inputLocation)
{
	if (Board[inputLocation].withinRange == false)
		return 1;
	if (cursor.selectMinionPointer->hasMoved == true)
		return 1;

	int OLD = cursor.selectMinionPointer->Location;		//Find old address of the minion
	Board[OLD].hasMinionOnTop = false;					//Clear the old tile, set the new tile.
	Board[inputLocation].hasMinionOnTop = true;

	Board[inputLocation].minionOnTop = Board[OLD].minionOnTop;
	Board[OLD].minionOnTop = NULL;

	cursor.selectMinionPointer->Location = inputLocation;
	cursor.selectMinionPointer->hasMoved = true;
	return 0;
}

int MasterBoard::deselectMinion() 
{
	cursor.selectMinionPointer->isMinionSelected = false;
	cursor.selectMinionFlag = false;
	cursor.selectMinionPointer = NULL;
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		Board[i].withinRange = false;
	}
		
	return 0;
}

int MasterBoard::attackMinion(int inputLocation) 
{
	if (cursor.selectMinionPointer->hasAttacked == true)			//Cannot attack twice!
		return 1;

	if ((cursor.selectMinionPointer->type == 'A' || cursor.selectMinionPointer->type == 'R') && (isAdjacent(cursor.Location, cursor.selectMinionPointer->Location) ))	 //If artillery type, cannot attack adjacent (MIN range)
		return 1;																																																				//Also, if artillery type, cannot attack if it's actually moved that turn.				

	double attackFactor = consultAttackValuesChart(cursor.selectMinionPointer->type, Board[inputLocation].minionOnTop->type);		//First perform offensive fire
	Board[inputLocation].minionOnTop->health -= attackFactor * cursor.selectMinionPointer->health;				//Decrease health by attack value, for now it's 1.

	if (Board[inputLocation].minionOnTop->health <= 0)
		destroyMinion((Board[inputLocation].minionOnTop));
	else
		if (Board[inputLocation].minionOnTop->type != 'A' && Board[inputLocation].minionOnTop->type != 'R' && cursor.selectMinionPointer->type != 'A' && cursor.selectMinionPointer->type != 'R')		//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		{
			attackFactor = consultAttackValuesChart(Board[inputLocation].minionOnTop->type, cursor.selectMinionPointer->type);	//If still alive, then perform defensive counterfire.
			cursor.selectMinionPointer->health -= attackFactor * Board[inputLocation].minionOnTop->health;
		}	
	
	cursor.selectMinionPointer->hasAttacked = true;

	if (cursor.selectMinionPointer->health <= 0)			//The attacker can be destroyed too!
		destroyMinion(cursor.selectMinionPointer);

	return 0;

}

int MasterBoard::destroyMinion(Minion * inputMinion) 
{ 
	inputMinion->isAlive = false;
	Board[inputMinion->Location].hasMinionOnTop = false;		//Tell the board it has no minions associated.
	eventText += "PLAYER ";									//Create event text stream telling us it was destroyed.
	eventText += char(turnFlag-32);							//MUST FIX IMPLEMENTATION!!!!
	eventText += "'s ";
	eventText += inputMinion->description;
	eventText += " DESTROYED!";
	return 0;													
}

int MasterBoard::endTurn() {
	
	if (turnFlag < NUMBEROFPLAYERS)					//Either increment turnFlag or set it to zero, thus cycling through the players.
		turnFlag++;
	else if (turnFlag >= NUMBEROFPLAYERS)
		turnFlag = 1;

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i].hasAttacked = false;
		minionRoster[i].hasMoved = false;
		minionRoster[i].artilleryCanAttack = true;
	}
		
	
	return 0;

}

int printScreen(MasterBoard * boardToPrint) 
{
	
	int i, j;
	
	int y = GameBoard.windowLocation / BOARD_WIDTH;
	int x = GameBoard.windowLocation % BOARD_WIDTH;


	for (i = y; i < y + WINDOW_HEIGHT; i++)
	{
		for (j = x; j < x + WINDOW_WIDTH; j++)
		{
			if (i * BOARD_WIDTH + j == cursor.Location)										//Print whomever has "priority. cursor first, then unit, then terrain."
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Is there a cursor there?
				cout << '+';
			}
			else if (boardToPrint->Board[i * BOARD_WIDTH + j].hasMinionOnTop == true)						//Is there a minion there?
			{
				switch (boardToPrint->Board[i * BOARD_WIDTH + j].minionOnTop->team)							//Determine team and then set the color.
				{
				case(0):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
					break;
				case(1):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
					break;
				case(2):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
					break;
				}

				cout << boardToPrint->Board[i * BOARD_WIDTH + j].minionOnTop->type;							//Print out the minion.
			}
			else if (boardToPrint->Board[i * BOARD_WIDTH + j].withinRange == true)								
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//If no minion, see if it's "in range" is set
				cout << ':';																				//And if so print the symbol for "in range"
			}\
			else 
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Otherwise put out the terrain for that square.
				cout << boardToPrint->Board[i * BOARD_WIDTH + j].symbol; 
			}
		}
		cout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	cout << "Player " << turnFlag << "'s Turn." << endl;
	cout << boardToPrint->Board[cursor.Location].description<<endl;				//Print out description of minion and terrain.
	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
		cout << "Player " << boardToPrint->Board[cursor.Location].minionOnTop->team << "'s " << boardToPrint->Board[cursor.Location].minionOnTop->description << ": "<<boardToPrint->Board[cursor.Location].minionOnTop->health <<" Health Left." << endl;
	else cout << endl;

	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[cursor.Location].minionOnTop->hasMoved == true)
			cout << "Has moved this turn." << endl;
		else cout << "Ready to move." << endl;
	}
	else cout << endl;
	
	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[cursor.Location].minionOnTop->hasAttacked == true)
			cout << "Has attacked this turn." << endl;
		else
			if (boardToPrint->Board[cursor.Location].minionOnTop->artilleryCanAttack == true || boardToPrint->Board[cursor.Location].minionOnTop->type != 'R' || boardToPrint->Board[cursor.Location].minionOnTop->type != 'A')
				cout << "Ready to attack." << endl;
			else cout << "Cannot attack." << endl;
	
	}

	else cout << endl;
	cout << eventText << "Cursor Location: " << cursor.getX()<<cursor.getY()<<endl;
	cout << GameBoard.windowLocation<<endl;
	eventText = "";
	for (i = 0; i < 10; i++)
		cout << endl;

	return 0;
}

int userInput(char * Input, MasterBoard * boardToInput)		
{
		*Input = _getch();
	if (*Input == 'a')											//Take user input and move cursor around.
		if (cursor.getX() != 0)
		{
			cursor.Location--;
		}

	if (*Input == 'd')
		if (cursor.getX() != BOARD_WIDTH-1)
		{
			cursor.Location++;
		}

	if (*Input == 's')
		if (cursor.getY() < BOARD_HEIGHT-1)
		{
			cursor.Location += BOARD_WIDTH;
		}

	if (*Input == 'w')
		if (cursor. getY() > 0)
		{
			cursor.Location -= BOARD_WIDTH;
		}

	

	if (*Input == 't')
		if (cursor.selectMinionFlag == false && GameBoard.Board[cursor.Location].hasMinionOnTop == true)
			GameBoard.selectMinion(cursor.Location);
		else if(cursor.selectMinionFlag == true) GameBoard.deselectMinion();

	if (*Input == 'm' && cursor.selectMinionFlag == true && GameBoard.Board[cursor.Location].hasMinionOnTop == false)		//If not on top, then move the unit.
	{
		if (GameBoard.moveMinion(cursor.Location) == 0)
		{
			cursor.selectMinionPointer->artilleryCanAttack = false;			//Successful movement means artillery cannot fire this turn.
			GameBoard.deselectMinion();
		}
	}

	if(*Input == 'm' && cursor.selectMinionFlag == true && cursor.selectMinionPointer->Location == cursor.Location)			//If already on top, just "move" by not moving. This allows the user to fire without actually changing position.
	{
		cursor.selectMinionPointer->hasMoved = true;
		GameBoard.deselectMinion();
	}

	if (*Input == 'r' && cursor.selectMinionFlag == true && GameBoard.Board[cursor.Location].hasMinionOnTop == true			//Can attack if minion is selected
		&& cursor.Location != cursor.selectMinionPointer->Location															//And you're not targeting your selected minion
		&& GameBoard.Board[cursor.Location].minionOnTop->team != cursor.selectMinionPointer->team							//And it's enemy team's.
		&& GameBoard.Board[cursor.Location].withinRange == true)															//And it's within range.
	{
		if(GameBoard.attackMinion(cursor.Location) == 0)
			GameBoard.deselectMinion();
	}

	if (*Input == 'p')									//Ends the turn and passes it to the next player.
	{
		if (cursor.selectMinionFlag == true)
			GameBoard.deselectMinion();
		GameBoard.endTurn();
	}

	GameBoard.checkWindow();

	return 0;
}

int main()
{
	GameBoard.createMinion('T', 17, 1);
	GameBoard.createMinion('T', 07, 1);
	GameBoard.createMinion('C', 07, 1);

	GameBoard.createMinion('i', 46, 2);
	GameBoard.createMinion('i', 47, 2);
	GameBoard.createMinion('i', 48, 2);
	GameBoard.createMinion('R', 87, 2);
	
	printScreen(&GameBoard);
	for (int i = 0; i > -1; i++)		//Run as long as the user wants. Infinite for loop.
	{
		userInput(&Input, &GameBoard);
		printScreen(&GameBoard);
	}

}

