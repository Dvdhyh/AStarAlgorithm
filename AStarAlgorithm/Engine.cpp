#include "Engine.h"

int TheEngine::V2(int x, int y) {
	return x * height + y;
}

void TheEngine::GetImageFile(LPWSTR _default) {
	ImageFile = (HBITMAP)::LoadImage(NULL, _default, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//Get ImageFileCurrent.bmWidth ImageFileCurrent.bmHeight
	ImageFileCurrent;
	GetObject(reinterpret_cast<HGDIOBJ>(ImageFile), sizeof(BITMAP), reinterpret_cast<LPVOID>(&ImageFileCurrent));
}

//Resets the grid
void TheEngine::newgame(int iWidth, int iHeight)
{
	//Set game state to Edit mode
	CurrentState = setupMode;

	//Finish tile reached
	GoalReached = false;

	//Reset Grid details
	cGrid.clear();

	//Reset the int arrays
	for (int x = 0;x < iWidth;x++)
		for (int y = 0;y < iHeight;y++)
			cGrid.emplace_back(x, y, _path);
}

//Display text
void TheEngine::DisplayScore(HDC hdc, Node _node) {
	WCHAR theScore[10] = { 0 };

	//Offset for each text inside a Tile
	int sX = 5, fX = 35, tX = 20, y1 = 5, y2 = 25;

	//Print Start score for each tile
	wsprintf(theScore, L"%i", _node.StartScore);
	TextOut(hdc, _node.x()*TileSize + sX, _node.y()*TileSize + y1, theScore, wcslen(theScore)); //From Start

	//Print Finish score for each tile
	wsprintf(theScore, L"%i", _node.FinishScore);
	TextOut(hdc, _node.x()*TileSize + fX, _node.y()*TileSize + y1, theScore, wcslen(theScore)); //From Finish

	//Print Total score for each tile
	wsprintf(theScore, L"%i", _node.GetTotalScore());
	TextOut(hdc, _node.x()*TileSize + tX, _node.y()*TileSize + y2, theScore, wcslen(theScore)); //Total
}

//Display the score for available tiles
void TheEngine::DisplayPathScore(HDC hdc) {
	
	for (auto&a : cGrid){
		if (!CheckBoxDisplaySearch) {
			if((a.TileType == _Shortest))
				DisplayScore(hdc, a);
		}
		else {
			if ((a.TileType != _block && a.TileType != _path))
				DisplayScore(hdc, a);
		}
		
	}
}

//Draws the Screen
void TheEngine::DrawScreen(HDC hdc, RECT* prc) {
	HDC hdcMem = CreateCompatibleDC(hdc); // Actual screen DC

											//Select HDC hdcMen
	SelectObject(hdcMem, ImageFile);

	//Legend
	BitBlt(hdc, 0, height * TileSize + 30, TileSize, TileSize, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdc, TileSize, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 1, 0, SRCCOPY);
	BitBlt(hdc, TileSize * 2, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 2, 0, SRCCOPY);
	BitBlt(hdc, TileSize * 3, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 3, 0, SRCCOPY);
	BitBlt(hdc, TileSize * 4, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 4, 0, SRCCOPY);
	BitBlt(hdc, TileSize * 5, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 5, 0, SRCCOPY);
	BitBlt(hdc, TileSize * 6, height * TileSize + 30, TileSize, TileSize, hdcMem, PixelSize * 6, 0, SRCCOPY);

	TextOut(hdc, 0 + 10, height * TileSize + 10, L"Path", 4);
	TextOut(hdc, TileSize + 10, height * TileSize + 10, L"Block", 5);
	TextOut(hdc, TileSize * 2 + 10, height * TileSize + 10, L"start", 5);
	TextOut(hdc, TileSize * 3 + 10, height * TileSize + 10, L"finish", 6);
	TextOut(hdc, TileSize * 4 + 10, height * TileSize + 10, L"Travel", 6);
	TextOut(hdc, TileSize * 5 + 10, height * TileSize + 10, L"Avail", 5);
	TextOut(hdc, TileSize * 6 + 10, height * TileSize + 10, L"Done", 4);


	// Display location for Start and Finish Tile
	TextOut(hdc, 0, height * TileSize + 120, L"Start:", 6);
	TextOut(hdc, TileSize * 2, height * TileSize + 120, L"Finish:", 6);

	TextOut(hdc, TileSize, height * TileSize + 120, cStartPos, 6);
	TextOut(hdc, TileSize * 3, height * TileSize + 120, cFinishPos, 6);


	TextOut(hdc, TileSize * 4, height * TileSize + 120, L"MousePos:", 10);

	//Displays text, Mouse Coordinates
	if (iGridX >= 0 && iGridX < width && iGridY >= 0 && iGridY < height) {
		wchar_t mouseCoord[6];
		wsprintf(mouseCoord, L"%.2i,%.2i", iGridX, iGridY);
		TextOut(hdc, TileSize * 5 + 20, height * TileSize + 120, mouseCoord, 6);
	}

	//Display Message Target Reached when shortest path is found
	if (CurrentState == Completed)
		TextOut(hdc, TileSize * 7, height * TileSize + 120, L"Target Reached", 15);
	else
		TextOut(hdc, TileSize * 7, height * TileSize + 120, L"Searching", 10);

	//Refreshing the grid 
	for (auto&a : cGrid) {
		int ctemp = a.TileType;
		int x = a.GridX;
		int y = a.GridY;
		switch (ctemp) {
		case _path:
			BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, 0, 0, SRCCOPY);
			break;
		case _block:
			BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize, 0, SRCCOPY);
			break;
		case _start:
			BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize * 2, 0, SRCCOPY);
			break;
		case _finish:
			BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize * 3, 0, SRCCOPY);
			break;
		case _Checked:
			if (CheckBoxDisplaySearch == true)
				BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize * 4, 0, SRCCOPY);
			else
				BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, 0, 0, SRCCOPY);
			break;
		case _Available:
			if (CheckBoxDisplaySearch == true)
				BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize * 5, 0, SRCCOPY);
			else
				BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, 0, 0, SRCCOPY);
			break;
		case _Shortest:
			BitBlt(hdc, x * TileSize, y * TileSize, TileSize, TileSize, hdcMem, PixelSize * 6, 0, SRCCOPY);
			break;
		}
	}

	//Display score
	if (CheckBoxDisplayScores == true)
		DisplayPathScore(hdc);

	//Delete object and select default
	DeleteDC(hdcMem);
}

//Set tiles
void TheEngine::SetTileType(int _x, int _y, int _tileType) {
	switch (_tileType) {
	case _start:
		holdStartPos[0] = _x;
		holdStartPos[1] = _y;
		break;
	case _finish:
		holdFinishPos[0] = _x;
		holdFinishPos[1] = _y;
		break;
	default:
		break;
	}
	cGrid[V2(_x, _y)].TileType = _tileType;
}

//Allows to rotate between which titles during setup mode
void TheEngine::NextTileState(int X, int Y) {
	int WhichType = cGrid[V2(X, Y)].TileType;

	switch (WhichType)
	{
	case _path:
		SetTileType(X, Y, _block);
		break;
	case _block:
		SetTileType(X, Y, _start);
		break;
	case _start:
		SetTileType(X, Y, _finish);
		break;
	case _finish:
		SetTileType(X, Y, _path);
		break;
	}
}

//Stores the tiles x and y pos of Start and Finish
int TheEngine::FindStart() {
	int tempCount = 0;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (cGrid[V2(x, y)].TileType == _start) {      //Find only 1 start
				tempCount++;
				holdStartPos[0] = x;
				holdStartPos[1] = y;
				wsprintf(cStartPos, L"%i,%i", x, y);
			}


	if (tempCount == 1)
		return 1;
	return tempCount;
}
int TheEngine::FindFinish() {
	int tempCount = 0;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (cGrid[V2(x, y)].TileType == _finish) {      //Find only 1 Finish
				tempCount++;
				holdFinishPos[0] = x;
				holdFinishPos[1] = y;
				wsprintf(cFinishPos, L"%i,%i", x, y);
			}


	if (tempCount == 1)
		return 1;
	return tempCount;
}
bool TheEngine::FindStartFinish() {
	return FindStart() && FindFinish() && 1;
}

//Display areas that can be move to during Manual mode
void TheEngine::UpdateNeighbour(int XX, int YY) {
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
			if (x != 0 || y != 0)
				if (x + XX >= 0 && y + YY >= 0 && x + XX < width && y + YY < height)
					if (cGrid[V2(x + XX, y + YY)].TileType == _path) {
						SetTileType(x + XX, y + YY, _Available); //Set tile to CanBeEvaluated
					}
}

//Check if path has reached the finish tile
bool TheEngine::CheckPath(int XX, int YY) {
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
			if (x != 0 || y != 0)
				if (x + XX >= 0 && x + XX < width  &&
					y + YY >= 0 && y + YY < height)
					if (cGrid[V2(x + XX, y + YY)].TileType == _Checked)
						return true;
	return false;
}


///////////////////////////// SCORE  //////////////////////////////////

int TheEngine::FindScoreStart(int XX, int YY) {
	if (cGrid[V2(XX, YY)].TileType == _start)
		return 0;

	int tempAdjScore = 900, LowX = 900, LowY = 900;

	//Add StartScore from adjacent checked tile
	for (int y = -1, ExitLoop = 0; y < 2 && ExitLoop == 0; y++)
		for (int x = -1; x < 2 && ExitLoop == 0; x++)
			if (x != 0 || y != 0)
				if (x + XX >= 0 && y + YY >= 0 && x + XX < width && y + YY < height)
					if (cGrid[V2(x + XX, y + YY)].TileType == _start) { //if at the start tile, score is 0
						tempAdjScore = 0;
						ExitLoop = 1;
						LowX = x; LowY = y;
					}
					else if (cGrid[V2(x + XX, y + YY)].TileType == _Checked) { // Tile already be travelled
						if (cGrid[V2(x + XX, y + YY)].StartScore < tempAdjScore) {
							tempAdjScore = cGrid[V2(x + XX, y + YY)].StartScore;
							LowX = x; LowY = y;
						}
					}

					if (LowX == 0 || LowY == 0) // adj by up down left right +10
						return tempAdjScore + 10;
					else   // if adj diagonally score is +14
						return tempAdjScore + 14;
}

int TheEngine::FindScoreFinish(int XX, int YY) {
	if (cGrid[V2(XX, YY)].TileType == _finish)
		return 0;

	int tempFromFinish, tempX, tempY, tempSmall;

	//Find distance from Finish tile
	tempX = abs(XX - holdFinishPos[0]);
	tempY = abs(YY - holdFinishPos[1]);
	tempSmall = tempX < tempY ? tempX : tempY;

	if (tempX == tempY)
		tempFromFinish = 14 * tempX;
	else
		tempFromFinish = 10 * abs(tempX - tempY) + tempSmall * 14;

	return tempFromFinish;
}

void TheEngine::UpdateScore(int XX, int YY) {
	int tempStart, tempFinish;
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
			if (x != 0 || y != 0)
				if (x + XX >= 0 && y + YY >= 0 && x + XX < width && y + YY < height)
					if (cGrid[V2(x + XX, y + YY)].TileType == _Checked || cGrid[V2(x + XX, y + YY)].TileType == _Available ||
						cGrid[V2(x + XX, y + YY)].TileType == _start || cGrid[V2(x + XX, y + YY)].TileType == _finish)
					{
						tempStart = FindScoreStart(x + XX, y + YY); //Find Start score 
						cGrid[V2(x + XX, y + YY)].StartScore = tempStart;

						tempFinish = FindScoreFinish(x + XX, y + YY);
						cGrid[V2(x + XX, y + YY)].FinishScore = tempFinish;
					}
}


//////////////////////////Display Shortest Path ////////////////

void TheEngine::FindMinStartScore(int &XX, int &YY) {

	int tempInt = 900, LowX = 88, LowY = 88;
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
			if (x + XX >= 0 && y + YY >= 0 && x + XX < width && y + YY < height)
				if (cGrid[V2(x + XX, y + YY)].TileType == _Checked) {
					if (cGrid[V2(x + XX, y + YY)].StartScore < tempInt) {
						tempInt = cGrid[V2(x + XX, y + YY)].StartScore;
						LowX = x + XX;
						LowY = y + YY;
					}
				}
				else if (cGrid[V2(x + XX, y + YY)].TileType == _start) {
					LowX = holdStartPos[0];
					LowY = holdStartPos[1];
					y = 10;
				}
				XX = LowX;
				YY = LowY;
}

//Starts from finish and retraces shortest path to start tile
void TheEngine::DisplayShortestPath() {
	int XX = holdFinishPos[0], YY = holdFinishPos[1];

	//Convert rest of the shortest path tiles to Done  
	while (XX != holdStartPos[0] || YY != holdStartPos[1]) {
		FindMinStartScore(XX, YY); //Set XX YY to adjcent tile that has smallest Start Score

									//Find Start Tile
		if (cGrid[V2(XX, YY)].TileType == _start)
			break;
		SetTileType(XX, YY, _Shortest); //Turn the tile next finish with lowest Start Score to Done Tile
	}
}


/////////////////////////// Auto Find //////////////////////////////

//Find the Position of the lowest Total score on grid
void TheEngine::AutoNextTile(std::vector<Node> MyVec, int &XX, int &YY) {

	int tempInt = 900, loopCount = 0;
	for (auto& a : MyVec) {
		if (a.TileType == _Available)
			if (a.GetTotalScore() < tempInt) {
				tempInt = a.GetTotalScore();
				XX = a.x();
				YY = a.y();
			}
		loopCount++;
	}
}

//Auto find the shortest path 1 period at a time
void TheEngine::AutoFindMode(HWND hWnd, int _startX, int _startY, int _finishX, int _finishY) {
	int CurrentState = AutoCompleteMode;

	UpdateNeighbour(_startX, _startY);

	// Add Scores on the tiles
	UpdateScore(_startX, _startY);

	while (CurrentState == AutoCompleteMode) {
		int iGridPosX = 0;
		int iGridPosY = 0;

		AutoNextTile(cGrid, iGridPosX, iGridPosY);

		if (cGrid[V2(iGridPosX, iGridPosY)].TileType == _Available) { //Check if path is available
			SetTileType(iGridPosX, iGridPosY, _Checked); //Set path as checked

			UpdateNeighbour(iGridPosX, iGridPosY); //Display available path around path just selected

			UpdateScore(iGridPosX, iGridPosY);

			InvalidateRect(hWnd, NULL, false);
		}

		if (CheckPath(holdFinishPos[0], holdFinishPos[1]) == true) {
			CurrentState = Completed;

			//Highlights shortest path when it reaches the finish
			DisplayShortestPath();

			InvalidateRect(hWnd, NULL, false);
		}
	}
}

