#pragma once
enum {
	_path, _block, _start, _finish, _Checked, _Available, _Shortest,
	setupMode, startMode, Completed, AutoCompleteMode, DynamicCursor
};

struct Vector2{
	int Vec2X;
	int Vec2Y;

	Vector2() {
		Vec2X = 0;
		Vec2Y = 0;
	}

	Vector2(int _x, int _y) {
		Vec2X = _x;
		Vec2Y = _y;
	}

	int GetVec2X() { return Vec2X; }
	int GetVec2Y() { return Vec2Y; }
};

struct Node{
	int StartScore;
	int FinishScore;
	int TileType; //1 path  2 block   3 start   4 finish    5 Checked   6 Available   7 Shortest

	int GridX;
	int GridY;

	Vector2 XYCoord;

	Node() {
		StartScore = 0;
		FinishScore = 0;
		TileType = _path;
	}

	Node(int _x, int _y, int _Type = 1, int _start = 0, int _finish = 0) {
		GridX = _x;
		GridY = _y;
		TileType = _Type;
		Vector2 XYCoord(_x, _y);
		StartScore = _start;
		FinishScore = _finish;
	}

	int GetTotalScore() { return StartScore + FinishScore; }

	int x() { return XYCoord.GetVec2X() > GridX ? XYCoord.GetVec2X() : GridX; }
	int y() { return XYCoord.GetVec2Y() > GridY ? XYCoord.GetVec2Y() : GridY; }
};
