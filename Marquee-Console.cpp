#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <string>

CONSOLE_SCREEN_BUFFER_INFO csbi;

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

bool checkXBound(int x)
{
	return x <= GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
}

bool checkYBound(int y)
{
	return y <= 13;
}


int main()
{
	int posX = 0;
	int posY = 2;
	std::string message = "Hello, World!";
	while (true)
	{
		//Sleep(100)
		// 1. Print header layout
		setCursorPosition(0, 0);
		printHeader();
		// 2. Increment (x,y) cursor position of marquee text;
		//posX++;
		//setCursorPosition(posX, posY);

		// 3. Check (x,y) bounds
		// 4. Print command input field
		setCursorPosition(0, 14);
		std::cout << "Enter command: ";

		// 5. Poll keyboard event
		// 6. Check keyboard hit

		if (_kbhit())
		{
			char key = _getch();
			std::cout << "Key pressed: " << key << std::endl;
		}
		system("cls");
	}

	return 0;
}
