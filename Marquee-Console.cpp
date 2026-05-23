#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <cmath>

void printHeader()
{
	std::cout << "++++++++++++++++++++++++++++++++++++++++\n";
	std::cout << "      Displaying a marquee console      \n";
	std::cout << "++++++++++++++++++++++++++++++++++++++++\n";
}
void setCursorPosition(int posX, int posY)
{
	COORD coord;
	coord.X = posX;
	coord.Y = posY;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

bool checkXBound(int x, int consoleWidth, int msgLength)
{
	return x + msgLength > consoleWidth;
}

void mapY(int x, int *y)
{
	*y = std::floor(((5 * std::sin(x/2)) + 8));
}

void clearLine(int y, int width)
{
	setCursorPosition(width, y);
	std::cout << std::string(width, ' ');
}

int main()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	int consoleWidth = csbi.dwSize.X;
	int posX = 0;
	int posY = 2;
	std::string input = "";
	std::string message = "Hello, World!";

	while (true)
	{
		// 1. Print header layout
		setCursorPosition(0, 0);
		printHeader();
		// 2. Increment (x,y) cursor position of marquee text;
		posX++;
		mapY(posX, &posY); 

		// 3. Check (x,y) bounds
		if (checkXBound(posX, consoleWidth, message.length()))
			posX = 0;
		
		setCursorPosition(posX, posY);
		std::cout << message << "\n";


		// 4. Print command input field
		setCursorPosition(0, 14);
		//std::cout << "X: " << posX << std::endl;
		//std::cout << "Y: " << posY << std::endl;
		std::cout << "Enter command: " << input;

		// 5. Poll keyboard event
		// 6. Check keyboard hit

		if (_kbhit())
		{
			char key = _getch();
			if (key == '\b' && !input.empty())
				input.pop_back();
			else
				input += key;
			//std::cout << "Key pressed: " << key << std::endl;
		}
		Sleep(50);
		clearLine(posY, posX);
		for (int i = 0; i < 15; i++)
			clearLine(i, consoleWidth);
	}

	return 0;
}
