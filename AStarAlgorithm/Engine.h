#include "GUICheckBox.h"
#include "Node.h"
#include <vector>
#pragma once


class TheEngine {
public:
	const int PixelSize = 64; //imageFile is 64 by 64
	const int TileSize = 64; //Size of the Tiles
	const int width = 16, height = 12;

	RECT MouseCoordRect; //Create Rectangle 

	HBITMAP ImageFile;
	BITMAP ImageFileCurrent;

	std::vector<Node> cGrid;

	std::vector<AddCheckBox>CheckBoxButton;

	int iGridX;
	int iGridY;
	int LastX = 0, LastY = 0; // temp hold Last Grid Coord

	int CurrentState; // 0 setupMode    1  startMode    2 Completed     4 AutoCompleteMode   5 DynamicCursor
	bool GoalReached = false;

	bool bDraggingStart = false;
	bool bDraggingFinish = false;

	bool bRefreshMouseDisplay = false;

	bool CheckBoxDisplayScores = false;
	bool CheckBoxDisplaySearch = false;

	bool bWholeScreen = false;
	bool bSmallRect = false;

	WCHAR cStartPos[6] = { 0 };
	WCHAR cFinishPos[6] = { 0 };
	int holdStartPos[2];
	int holdFinishPos[2];


	int V2(int x, int y);

	void GetImageFile(LPWSTR _default);

	//Resets the grid
	void newgame(int iWidth, int iHeight);

	//Display score
	void DisplayScore(HDC hdc, Node _node);

	//Display the score for available tiles
	void DisplayPathScore(HDC hdc);

	//Draws the Screen
	void DrawScreen(HDC hdc, RECT* prc);

	//Set tiles
	void SetTileType(int _x, int _y, int _tileType);

	//Allows to rotate between which titles during setup mode
	void NextTileState(int X, int Y);

	//Stores the tiles x and y pos of Start and Finish
	int FindStart();
	int FindFinish();
	bool FindStartFinish();

	//Display areas that can be move to during Manual mode
	void UpdateNeighbour(int XX, int YY);

	//Check if path has reached the finish tile
	bool CheckPath(int XX, int YY);


	///////////////////////////// SCORE  //////////////////////////////////

	int FindScoreStart(int XX, int YY);

	int FindScoreFinish(int XX, int YY);

	void UpdateScore(int XX, int YY);


	//////////////////////////Display Shortest Path ////////////////

	void FindMinStartScore(int &XX, int &YY);

	//Starts from finish and retraces shortest path to start tile
	void DisplayShortestPath();


	/////////////////////////// Auto Find //////////////////////////////

	//Find the Position of the lowest Total score on grid
	void AutoNextTile(std::vector<Node> MyVec, int &XX, int &YY);

	//Auto find the shortest path 1 period at a time

	void AutoFindMode(HWND hWnd, int _startX, int _startY, int _finishX, int _finishY);

	///////////////////////////////////////////////////////////////
};