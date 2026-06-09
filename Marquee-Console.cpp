#include <iostream>
#include <conio.h>
#include <deque>
#include <Windows.h>
#include <string>
#include <cmath>

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };

const int CUBE_CENTER_Y = 13;
const int MSG_START_ROW = 24;
const int CONSOLE_W = 80;
const int CONSOLE_H = 34;

void printHeader();

Vec3 rotateY(Vec3 v, float angle);
Vec2 project(Vec3 v);
void toScreenCoords(Vec2 p, int& screenX, int& screenY);
void drawEdge(Vec2 start, Vec2 end);

void setCursorPosition(int x, int y);

int main()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cursorInfo);

	std::string input = "";
	std::deque<std::string> messages;

	Vec3 vertices[8] = {
		{ -1.0, 1.0, -1.0 },
		{ 1.0, 1.0, -1.0 },
		{ 1, -1.0, -1.0 },
		{ -1, -1.0, -1.0 },
		{ -1.0, 1.0,  1.0 },
		{ 1.0, 1.0, 1.0 },
		{ -1.0, -1.0, 1.0 },
		{ 1.0, -1.0, 1.0 }
	};

	Vec3 edges[12][2]
	{
		{ {-1.0f, 1.0f, -1.0f } , { 1.0f, 1.0f, -1.0f } },
		{ {-1.0f, -1.0f, -1.0f } , { 1.0f, -1.0f, -1.0f } },
		{ {-1.0f, 1.0f, 1.0f } , { 1.0f, 1.0f, 1.0f } },
		{ {-1.0f, -1.0f, 1.0f } , { 1.0f, -1.0f, 1.0f } },

		{ {1.0f, -1.0f, -1.0f } , { 1.0f, -1.0f, 1.0f } },
		{ {-1.0f, -1.0f, -1.0f } , { -1.0f, -1.0f, 1.0f } },
		{ {1.0f, 1.0f, -1.0f } , { 1.0f, 1.0f, 1.0f } },
		{ {-1.0f, 1.0f, -1.0f } , { -1.0f, 1.0f, 1.0f } },

		{ {-1.0f, 1.0f, -1.0f } , { -1.0f, -1.0f, -1.0f } },
		{ {-1.0f, 1.0f, 1.0f } , { -1.0f, -1.0f, 1.0f } },

		{ {1.0f, 1.0f, -1.0f } , { 1.0f, -1.0f, -1.0f } },
		{ {1.0f, 1.0f, 1.0f } , { 1.0f, -1.0f, 1.0f } },
	};

	float angle = 0.0f;
	int posX = 0;
	int posY = 0;

	system("cls");
	while (true)
	{
		angle++;

		system("cls");

		// header
		printHeader();

		// draw vertices
		for (int i = 0; i < 8; i++)
		{
			Vec3 rotated = rotateY(vertices[i], angle * 0.01f);
			Vec2 projected = project(rotated);
			toScreenCoords(projected, posX, posY);
			setCursorPosition(posX, posY);
			std::cout << "*";
		}

		// draw edges
		for (int i = 0; i < 12; i++)
		{
			Vec3 rotatedStart = rotateY(edges[i][0], angle * 0.01f);
			Vec3 rotatedEnd = rotateY(edges[i][1], angle * 0.01f);
			Vec2 projectedStart = project(rotatedStart);
			Vec2 projectedEnd = project(rotatedEnd);
			int startX, startY, endX, endY;

			toScreenCoords(projectedStart, startX, startY);
			toScreenCoords(projectedEnd, endX, endY);
			drawEdge({ (float)startX, (float)startY }, { (float)endX, (float)endY });
		}

		setCursorPosition(0, MSG_START_ROW - 1);
		std::cout << "Write a message: " << input;

		int i = 1;
		for (std::string message : messages)
		{
			setCursorPosition(0, MSG_START_ROW + i);
			std::cout << "You entered: " << message;
			i++;
		}

		
		Sleep(10);

		if (_kbhit())
		{
			char key = _getch();
			if (key == '\b' && !input.empty())
				input.pop_back();
			else if (key == '\r' && !input.empty())
			{
				messages.push_back(input);
				input.clear();

				if (messages.size() >= 10)
					messages.pop_front();
			}
			else if (key >= 32 && key <= 126 && input.size() < 70)
				input += key;
		}
	}
}

void printHeader()
{
	std::cout << "++++++++++++++++++++++++++++++++++++++++\n";
	std::cout << "      Displaying a marquee console      \n";
	std::cout << "++++++++++++++++++++++++++++++++++++++++\n";
}

Vec3 rotateY(Vec3 v, float angle)
{
	float x = (v.x * cos(angle)) + (v.z * sin(angle));
	float z = -(v.x * sin(angle)) + (v.z * cos(angle));
	return { x, v.y, z };
}

Vec2 project(Vec3 v)
{
	float offset = 3.0f;
	float screenX = v.x / (v.z + offset);
	float screenY = v.y / (v.z + offset);
	return { screenX, screenY };
}

void toScreenCoords(Vec2 p, int& screenX, int& screenY)
{
	float xScale = 7.5f;
	float yScale = 15.0f;
	Vec2 center = { CONSOLE_W / 2, CUBE_CENTER_Y };
	float aspectRatio = 5.0f;

	screenX = static_cast<int>(p.x * xScale * aspectRatio + center.x);
	screenY = static_cast<int>(p.y * yScale + center.y);
}

void setCursorPosition(int x, int y)
{
	COORD c = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawEdge(Vec2 start, Vec2 end)
{
	float dx = end.x - start.x;
	float dy = end.y - start.y;

	int stepX = (dx > 0) ? 1 : -1;
	int stepY = (dy > 0) ? 1 : -1;

	bool swapped = false;

	dy = abs(dy);
	dx = abs(dx);

	if (dy > dx)
	{
		std::swap(dy, dx);
		swapped = true;
	}

	float d = 2 * dy - dx;
	float posX = start.x;
	float posY = start.y;

	if (swapped)
	{
		std::swap(posX, posY);
		std::swap(stepX, stepY);
	}

	for (int i = 0; i < dx; i++)
	{
		if (d < 0)
		{
			d = d + 2 * dy;
			posX = posX + stepX;
		}
		else
		{
			d = d + 2 * (dy - dx);
			posX = posX + stepX;
			posY = posY + stepY;
		}

		int wx = swapped ? (int)posY : (int)posX;
		int wy = swapped ? (int)posX : (int)posY;


		setCursorPosition(wx, wy);
		std::cout << "*";
	}
}