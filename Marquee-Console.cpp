#include <iostream>
#include <conio.h>
#include <deque>
#include <Windows.h>
#include <string>
#include <sstream>

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };

const int CUBE_CENTER_Y = 13;
const int MSG_START_ROW = 24;
const int CONSOLE_W = 80;
const int CONSOLE_H = 34;

CHAR_INFO backBuffer[CONSOLE_H][CONSOLE_W];

void clearBuffer();
void writeToBuffer(int x, int y, const std::string& text);
void flushBuffer();

void printHeader();
std::string handleCommand(std::string input, int& sleepMs);

Vec3 rotateY(Vec3 v, float angle);
Vec2 project(Vec3 v);
void toScreenCoords(Vec2 p, int& screenX, int& screenY);
void drawEdge(Vec2 start, Vec2 end);

int main()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cursorInfo);

	int posMessage = CONSOLE_H - 5;
	std::string input = "";
	std::string feedback = "";

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
	int sleepMs = 16; // ~60 FPS

	while (true)
	{		
		clearBuffer();

		// 1. Print header layout
		printHeader();

		// 2. Increment cube rotation angle
		angle++;
		
		// 3a. Draw vertices
		for (int i = 0; i < 8; i++)
		{
			Vec3 rotated = rotateY(vertices[i], angle * 0.01f);
			Vec2 projected = project(rotated);
			toScreenCoords(projected, posX, posY);
			writeToBuffer(posX, posY, "*");
		}

		// 3b. Draw edges
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

		// 4. Print command input field
		writeToBuffer(0, MSG_START_ROW - 1, "Enter command: " + input);

		writeToBuffer(0, MSG_START_ROW + 1, feedback);

		flushBuffer();

		Sleep(sleepMs);

		// 5. Poll keyboard event

		if (_kbhit())
		{
			char key = _getch();

			// 6. Check keyboard hit
			if (key == '\b' && !input.empty())
				input.pop_back();
			else if (key == '\r')
			{
				if (!input.empty())
				{
					feedback = handleCommand(input, sleepMs);
				}
				input.clear();
			}
			else if (key >= 32 && key <= 126)
				input += key;
		}
	}
}

// Clears the back buffer by filling it with spaces and default attributes
void clearBuffer()
{
	for (int y = 0; y < CONSOLE_H; y++)
	{
		for (int x = 0; x < CONSOLE_W; x++)
		{
			backBuffer[y][x].Char.AsciiChar = ' ';
		}
	}
}

// Writes a string to the back buffer at the specified (x, y) position
// params: x and y coordinates, text to write
void writeToBuffer(int x, int y, const std::string& text)
{
	for (int i = 0; i < (int)text.size(); i++)
	{
		int bx = x + i;
		if (bx < 0 || bx >= CONSOLE_W || y < 0 || y >= CONSOLE_H) continue;
		backBuffer[y][bx].Char.AsciiChar = text[i];
		backBuffer[y][bx].Attributes = 0x07;
	}
}

// Flushes the back buffer to the console output using WriteConsoleOutput API
void flushBuffer()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD bufferSize = { CONSOLE_W, CONSOLE_H };
	COORD bufferCoord = { 0, 0 };
	SMALL_RECT writeRegion = { 0, 0, CONSOLE_W - 1, CONSOLE_H - 1 };
	WriteConsoleOutput(hOut, (CHAR_INFO*)backBuffer, bufferSize, bufferCoord, &writeRegion);
}

void printHeader()
{
	writeToBuffer(0, 0, "++++++++++++++++++++++++++++++++++++++++");
	writeToBuffer(0, 1, "      Displaying a marquee console      ");
	writeToBuffer(0, 2, "++++++++++++++++++++++++++++++++++++++++");
}

// Rotates a 3D vector around the Y-axis by a given angle
// params: 3D vector, rotation angle in radians
// returns: rotated 3D vector
Vec3 rotateY(Vec3 v, float angle)
{
	float x = (v.x * cos(angle)) + (v.z * sin(angle));
	float z = -(v.x * sin(angle)) + (v.z * cos(angle));

	return { x, v.y, z };
}

// Projects 3D coordinates to 2D screen coordinates using perspective projection
// params: 3D point
// returns: projected 2D point
Vec2 project(Vec3 v)
{
	float offset = 3.0f;
	float screenX = v.x / (v.z + offset);
	float screenY = v.y / (v.z + offset);

	return { screenX, screenY };
}

// Converts projected 2D coordinates to screen coordinates
// params: projected 2D point, output screen coordinates (screenX, screenY)
void toScreenCoords(Vec2 p, int& screenX, int& screenY)
{
	float xScale = 7.5f;
	float yScale = 15.0f;
	Vec2 center = { CONSOLE_W / 2, CUBE_CENTER_Y };
	float aspectRatio = 5.0f;

	screenX = static_cast<int>(p.x * xScale * aspectRatio + center.x);
	screenY = static_cast<int>(p.y * yScale + center.y);
}

//draws edge between two points using Bresenham's line algorithm
// params: start and end points of the edge
void drawEdge(Vec2 start, Vec2 end)
{
	//calculate delta x and y
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

	//calculate decision parameter
	float d = 2 * dy - dx;
	float posX = start.x;
	float posY = start.y;

	if (swapped)
	{
		std::swap(posX, posY);
		std::swap(stepX, stepY);
	}

	//decision parameter loop
	for (int i = 0; i < dx; i++)
	{
		if (d < 0)
		{
			d = d + 2 * dy;
			posX = posX + stepX;
		}
		else if (d >= 0)
		{
			d = d + 2 * (dy - dx);
			posX = posX + stepX;
			posY = posY + stepY;
		}

		if (swapped)
			writeToBuffer(posY, posX, "*");
		else
			writeToBuffer(posX, posY, "*");
	}
}

std::string handleCommand(std::string input, int& sleepMs)
{
	std::stringstream ss(input);
	std::string command;
	ss >> command;
	if (command == "speed")
	{
		int ms = 0;
		ss >> ms;
		sleepMs = ms;
		return "Speed set to " + std::to_string(ms) + " ms";
	}

	return "Message: " + input;
}