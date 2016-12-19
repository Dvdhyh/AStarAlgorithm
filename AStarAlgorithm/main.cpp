#include "Globals.h"

Globals globals;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT iCmdShow)
{
	WNDCLASS wcex;
	//wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("GettingStarted");

	RegisterClass(&wcex);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HWND hWnd = CreateWindow(
		TEXT("GettingStarted"),   // window class name
		TEXT("Getting Started"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		0,            // initial y position
		CW_USEDEFAULT,            // initial x size
		0,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(9001));

	globals.hInst = hInstance;

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		MoveWindow(hWnd, 10, 10, 500, 500, true);

		//Create Menu bar
		globals.hMenu = CreateMenu();

		globals.hSubMenu = CreatePopupMenu();
		AppendMenu(globals.hMenu, MF_STRING | MF_POPUP, (UINT)globals.hSubMenu, L"&File");
		AppendMenu(globals.hSubMenu, MF_STRING, CM_MENU_FILE_NEW, L"New Game");
		AppendMenu(globals.hSubMenu, MF_STRING, CM_MENU_FILE_START, L"Start");
		AppendMenu(globals.hSubMenu, MF_STRING, CM_MENU_FILE_FIND, L"Auto Find");
		AppendMenu(globals.hSubMenu, MF_STRING, CM_MENU_FILE_DYNA, L"Dynamic");
		AppendMenu(globals.hSubMenu, MF_STRING, CM_MENU_FILE_EXIT, L"E&xit");
		SetMenu(hWnd, globals.hMenu);

		SetTimer(hWnd, TIMERID, 100, NULL);


		//Make reference to texture
		globals.theEngine.GetImageFile(L"TileColors.bmp");

		//Set Current state to edit mode
		globals.theEngine.CurrentState = setupMode;

		//Set perimeter of MouseCoordRect rect
		globals.theEngine.MouseCoordRect.left = globals.theEngine.TileSize * 5 + 20;
		globals.theEngine.MouseCoordRect.top = globals.theEngine.TileSize * globals.theEngine.height + 120;
		globals.theEngine.MouseCoordRect.right = globals.theEngine.TileSize * 5 + 80;
		globals.theEngine.MouseCoordRect.bottom = globals.theEngine.TileSize * globals.theEngine.height + 140;

		//Override the default window size, and Set Window size (on creation)
		MoveWindow(hWnd, 0, 0, globals.theEngine.width * globals.theEngine.TileSize + 50, globals.theEngine.height * globals.theEngine.TileSize + 204, true);
		// width * (16 pixel is width of each tile) + 16     Window size is always 16 pixel smaller
		//height * (16 pixel is height of each tile) + 32 (for red number and) + 60 (for menu bar and standard title bar)

		//Setup game and Reset
		globals.theEngine.newgame(globals.theEngine.width, globals.theEngine.height);

		//Toggle display
		globals.theEngine.CheckBoxButton.emplace_back(AddCheckBox());
		globals.theEngine.CheckBoxButton.emplace_back(AddCheckBox());
		globals.theEngine.CheckBoxButton[0].CreateGUI(hWnd, 470, globals.theEngine.height * globals.theEngine.TileSize + 10, 130, 20, L"Display Scores");
		globals.theEngine.CheckBoxButton[1].CreateGUI(hWnd, 470, globals.theEngine.height * globals.theEngine.TileSize + 50, 130, 20, L"Display Search");


		break;
	}
	case WM_LBUTTONUP:{
		if (globals.theEngine.CurrentState == setupMode) { //Setup mode
			if ((globals.theEngine.iGridX < globals.theEngine.width) && (globals.theEngine.iGridY < globals.theEngine.height)) //If grid coord are within the game grid
			{
				//Rotate to next tile
				globals.theEngine.NextTileState(globals.theEngine.iGridX, globals.theEngine.iGridY);

				InvalidateRect(hWnd, NULL, false);
			}
		}
		else if (globals.theEngine.CurrentState == startMode) { //Start pathfinding
			if ((globals.theEngine.iGridX < globals.theEngine.width) && (globals.theEngine.iGridY < globals.theEngine.height)) //If grid coord are within the game grid
			{
				if (globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType == _Available) { //Check if path is available
					globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType = _Checked; //Set path as checked

					globals.theEngine.UpdateNeighbour(globals.theEngine.iGridX, globals.theEngine.iGridY); //Display available path around path just selected

					globals.theEngine.UpdateScore(globals.theEngine.iGridX, globals.theEngine.iGridY);

					InvalidateRect(hWnd, NULL, false);
				}
			}
		}
		else if (globals.theEngine.CurrentState == DynamicCursor) {
			globals.theEngine.bDraggingStart = false;
			globals.theEngine.bDraggingFinish = false;
			globals.theEngine.FindStart();
			globals.theEngine.FindFinish();
		}

		//Check if path has reached the finish  (In DynamicMode changing state to complete, will stop)
		if (globals.theEngine.CheckPath(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1]) == true && globals.theEngine.CurrentState != DynamicCursor) {
			globals.theEngine.CurrentState = Completed;

			globals.theEngine.GoalReached = true;

			//Highlights shortest path when it reaches the finish
			globals.theEngine.DisplayShortestPath();

			InvalidateRect(hWnd, NULL, false);
		}

		break;
	}
	case WM_LBUTTONDOWN:{
		if (globals.theEngine.CurrentState == DynamicCursor) {
			if (globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType == _start)
				globals.theEngine.bDraggingStart = true;
			else if (globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType == _finish)
				globals.theEngine.bDraggingFinish = true;
		}
		break;
	}
	case WM_TIMER:{
		switch (wParam)
		{
		case TIMERID:
		{
			if (globals.theEngine.CurrentState == DynamicCursor) {

				if (globals.theEngine.bDraggingStart == true) { //Allows click and drag of Start Tile
					if (globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType != _block && globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType != _finish) { //Only be dragged onto a TileType 1
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1])].StartScore = 0;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1])].FinishScore = 0;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1])].TileType = _path;

						globals.theEngine.holdStartPos[0] = globals.theEngine.iGridX;
						globals.theEngine.holdStartPos[1] = globals.theEngine.iGridY;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1])].TileType = _start;

						//Reset Score when Start or Finish Tile is moved
						for (auto& a : globals.theEngine.cGrid)
							if (a.TileType == _Checked || a.TileType == _Available || a.TileType == _Shortest) {
								a.TileType = _path;
								a.StartScore = 0;
								a.FinishScore = 0;
							}

						//Update Screen
						InvalidateRect(hWnd, NULL, false);

						globals.theEngine.GoalReached = false;
					}
				}
				else if (globals.theEngine.bDraggingFinish == true) { //Allows click and drag of Finish Tile
					if (globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType != _block && globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.iGridX, globals.theEngine.iGridY)].TileType != _start) {
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1])].StartScore = 0;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1])].FinishScore = 0;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1])].TileType = _path;

						globals.theEngine.holdFinishPos[0] = globals.theEngine.iGridX;
						globals.theEngine.holdFinishPos[1] = globals.theEngine.iGridY;
						globals.theEngine.cGrid[globals.theEngine.V2(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1])].TileType = _finish;

						//Reset Score when Start or Finish Tile is moved
						for (auto& a : globals.theEngine.cGrid)
							if (a.TileType == _Checked || a.TileType == _Available || a.TileType == _Shortest) {
								a.TileType = _path;
								a.StartScore = 0;
								a.FinishScore = 0;
							}

						//Update Screen
						InvalidateRect(hWnd, NULL, false);

						globals.theEngine.GoalReached = false;
					}
				}


				//Find Shortest path
				while (!globals.theEngine.GoalReached) {
					int iGridPosX = 0;
					int iGridPosY = 0;
					// Display avialable path around the start tile
					globals.theEngine.UpdateNeighbour(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

					// Add Scores on the tiles
					globals.theEngine.UpdateScore(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

					globals.theEngine.AutoNextTile(globals.theEngine.cGrid, iGridPosX, iGridPosY);

					if (globals.theEngine.cGrid[globals.theEngine.V2(iGridPosX, iGridPosY)].TileType == _Available) { //Check if path is available
						globals.theEngine.SetTileType(iGridPosX, iGridPosY, _Checked); //Set path as checked

						globals.theEngine.UpdateNeighbour(iGridPosX, iGridPosY); //Display available path around path just selected

						globals.theEngine.UpdateScore(iGridPosX, iGridPosY);

						InvalidateRect(hWnd, NULL, false);
					}

					if (globals.theEngine.CheckPath(globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1]) == true) {

						//Highlights shortest path when it reaches the finish
						globals.theEngine.DisplayShortestPath();

						InvalidateRect(hWnd, NULL, false);

						//Stop while loop when path is found
						globals.theEngine.GoalReached = true;
					}
				}

			}
		}
		break;
		}
	}
	case WM_MOUSEMOVE:{
		POINT p;
		//Get mouse Coordinates
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);

		int tempX = p.x / globals.theEngine.TileSize;
		int tempY = p.y / globals.theEngine.TileSize;

		//Only Register Mouse coordinates if that are in Grid
		if (tempX < globals.theEngine.width && tempX >= 0 &&
			tempY < globals.theEngine.height && tempY >= 0) {

			globals.theEngine.iGridX = tempX;
			globals.theEngine.iGridY = tempY;
		}

		//Only refresh Mouse Text screen if Display changes
		if (globals.theEngine.LastX != globals.theEngine.iGridX || globals.theEngine.LastY != globals.theEngine.iGridY) {

			globals.theEngine.LastX = globals.theEngine.iGridX, globals.theEngine.LastY = globals.theEngine.iGridY;

			//Update Mouse Pos Display
			//InvalidateRect(hWnd, &MouseCoordRect, false);
			InvalidateRect(hWnd, NULL, false);
		}

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		if (wmId == globals.theEngine.CheckBoxButton[0].gLabelID) {
			if (wmEvent == BN_CLICKED) {
				LRESULT checkState = SendMessage(globals.theEngine.CheckBoxButton[0].GUIHandle, BM_GETCHECK, 0, 0);
				if (checkState == BST_CHECKED)
					globals.theEngine.CheckBoxDisplayScores = true;
				else if (checkState == BST_UNCHECKED)
					globals.theEngine.CheckBoxDisplayScores = false;
			}
			InvalidateRect(hWnd, NULL, false);
		}
		else if (wmId == globals.theEngine.CheckBoxButton[1].gLabelID) {
			if (wmEvent == BN_CLICKED) {
				LRESULT checkState = SendMessage(globals.theEngine.CheckBoxButton[1].GUIHandle, BM_GETCHECK, 0, 0);
				if (checkState == BST_CHECKED)
					globals.theEngine.CheckBoxDisplaySearch = true;
				else if (checkState == BST_UNCHECKED)
					globals.theEngine.CheckBoxDisplaySearch = false;
			}
			InvalidateRect(hWnd, NULL, false);
		}

		switch (wmId) {
		case CM_MENU_FILE_NEW:
			globals.theEngine.newgame(globals.theEngine.width, globals.theEngine.height);

			InvalidateRect(hWnd, NULL, true);
			break;
		case CM_MENU_FILE_START:
			//If start tile and finish tile have been poorly setup, stop
			if (globals.theEngine.FindStartFinish() == false)
				break;

			//Score and Search mode should seen as default
			SendMessage(globals.theEngine.CheckBoxButton[0].GUIHandle, BM_SETCHECK, (WPARAM)true, (LPARAM)0);
			globals.theEngine.CheckBoxDisplayScores = true;

			SendMessage(globals.theEngine.CheckBoxButton[1].GUIHandle, BM_SETCHECK, (WPARAM)true, (LPARAM)0);
			globals.theEngine.CheckBoxDisplaySearch = true;

			globals.theEngine.CurrentState = startMode; //Switch from setup mode to start mode

											//Display avialable path around the start tile
			globals.theEngine.UpdateNeighbour(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

			// Add Scores on the tiles
			globals.theEngine.UpdateScore(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

			InvalidateRect(hWnd, NULL, false);
			break;
		case CM_MENU_FILE_FIND:
			if (globals.theEngine.FindStartFinish() == false)
				break;

			globals.theEngine.AutoFindMode(hWnd, globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1], globals.theEngine.holdFinishPos[0], globals.theEngine.holdFinishPos[1]);

			InvalidateRect(hWnd, NULL, false);
			break;
		case CM_MENU_FILE_DYNA:
			//Find only single Start and finish Tile 
			if (globals.theEngine.FindStartFinish() == false)
				break;

			globals.theEngine.CurrentState = DynamicCursor;

			//Score and Search mode should be off as default
			SendMessage(globals.theEngine.CheckBoxButton[0].GUIHandle, BM_SETCHECK, (WPARAM)false, (LPARAM)0);
			globals.theEngine.CheckBoxDisplayScores = false;

			SendMessage(globals.theEngine.CheckBoxButton[1].GUIHandle, BM_SETCHECK, (WPARAM)false, (LPARAM)0);
			globals.theEngine.CheckBoxDisplaySearch = false;

			// Display avialable path around the start tile
			globals.theEngine.UpdateNeighbour(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

			// Add Scores on the tiles
			globals.theEngine.UpdateScore(globals.theEngine.holdStartPos[0], globals.theEngine.holdStartPos[1]);

			InvalidateRect(hWnd, NULL, false);
			break;
		case CM_MENU_FILE_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	}
	case WM_ERASEBKGND:
		return 1;
		break;
	case WM_PAINT: {
		PAINTSTRUCT  ps;

		HDC hdc = BeginPaint(hWnd, &ps);
		//// TODO: Add any drawing code that uses hdc here...
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		globals.theEngine.DrawScreen(hdc, &rcClient);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}