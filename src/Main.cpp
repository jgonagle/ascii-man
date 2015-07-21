/*
Main.cpp
Written by John McGonagle on May 8, 2010.
*/
#include "GameHandler.h"
#include "Board.h"
#include "GameGraphics.h"
#include <windows.h>

int main()
{
	HANDLE display_handler = GetStdHandle(STD_OUTPUT_HANDLE);
	Board board("MapReg.txt");
	GameGraphics game_graphics(display_handler, board.GetMaxAttributes(),
							   board.GetBoardDim(), board.GetBoardOffset());
	GameHandler ascii_man_game(board, game_graphics);
	ascii_man_game.Run();

	return 0;
}