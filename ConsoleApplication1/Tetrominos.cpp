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
