#include "Tetrominos.h"

//Une liste d'entiers 16-bit représentant les tétrominos et leurs different
std::array<TetrominoBits, 4 * 7> tetrominos = {
			(TetrominoBits)0b0100010001000100,
			(TetrominoBits)0b1100011000000000,
			(TetrominoBits)0b1000100011000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100111000000000,
			(TetrominoBits)0b0110110000000000,
			(TetrominoBits)0b0100010011000000,

			(TetrominoBits)0b0000111100000000,
			(TetrominoBits)0b0100110010000000,
			(TetrominoBits)0b1110100000000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100011001000000,
			(TetrominoBits)0b1000110001000000,
			(TetrominoBits)0b1000111000000000,

			(TetrominoBits)0b0010001000100010,
			(TetrominoBits)0b1100011000000000,
			(TetrominoBits)0b0110001000100000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0000111001000000,
			(TetrominoBits)0b0110110000000000,
			(TetrominoBits)0b1100100010000000,

			(TetrominoBits)0b0000000011110000,
			(TetrominoBits)0b0100110010000000,
			(TetrominoBits)0b0010111000000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100110001000000,
			(TetrominoBits)0b1000110001000000,
			(TetrominoBits)0b1110001000000000,
};

TetrominoBits Tetromino::getData()
{
	return tetrominos.at(type + (rotation * 7));
}

#include "Boards.h"

void Tetromino::resolveBlockCollisionRotation(TetrisBoard& board)
{
	if (board.isColliding(*this))
	{
		while (true) //shitty collision detection for rotations
		{
			pos.x--; //try moving the block left 
			if (!board.isColliding(*this)) break; //if it works, break
			pos.x += 2; //try moving to the right
			if (!board.isColliding(*this)) break; //if it works, break
			pos.x++; //try 2 to the right
			if (!board.isColliding(*this)) break; //if it works, break
			pos.x -= 4; //try 2 to the left
			if (!board.isColliding(*this)) break; //if it works, break
			pos.x += 2;

			pos.y++;
			if (!board.isColliding(*this)) break; //if it works, break
			pos.y++;
			if (!board.isColliding(*this)) break; //if it works, break
			pos.y++;
			if (!board.isColliding(*this)) break; //if it works, break
			pos.y -= 3;

			while (board.isColliding(*this)) //is all else fails, rotate until it works
				rotate(1, board, true);
			break;
		}
	}
}

void Tetromino::rotate(bool left, TetrisBoard& board, bool noCollision)
{
	if (left)
		rotation--;
	else
		rotation++;

	if (rotation < 0)
		rotation = 3;
	if (rotation > 3)
		rotation = 0;

	data = getData();
	if(!noCollision)
		resolveBlockCollisionRotation(board);
}

void Tetromino::move(bool left, TetrisBoard& board)
{
	int dir = left ? -1 : 1;
	pos.x += dir;

	while (board.isColliding(*this))
		pos.x += -dir;
}
