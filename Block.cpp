#include "Block.h"

/***************************************************************
The Round Key consists of 11, 13 or 15, 16-byte blocks.
Each block is constructed linearly. For example,
											
{a,b,c,e,f,g,h,y,j,k,i,l,m,n,o,p}

is transformed into the 4x4 matrix:

		[a,b,c,d]
		[e,f,g,h]
		[i,j,k,l]
		[m,n,o,p]

This is stored using a 4x4 matrix vs a 16 x 1 matrix to make 
operations easier.
***************************************************************/
Block::Block(std::string key) {
	for (size_t i = 0, z = 0; i < NUM_COL; i++) {
		for (size_t j = 0; j < NUM_ROW; j++) {
			cells[i][j] = (BYTE)key[z++];
		}
	}
}
Block& Block::operator^=(const Block &rhs) { // S = {(i,j)| A(i,j) ^ B(i,j)} 
	for (size_t i = 0, z = 0; i < NUM_COL; i++) {
		for (size_t j = 0; j < NUM_ROW; j++) {
			this->cells[i][j] ^=  rhs.cells[i][j];
		}
	}

	return *this;
}

std::vector<BYTE> Block::column(size_t id) {
	std::vector<BYTE> column;

	for (size_t j = 0; j < NUM_ROW; j++) {
		column.push_back(cells[j][id]);
	}

	return column;
}

void Block::printMatrix() { // great for debugging
	std::cout << "Block: \n";
	for (size_t i = 0; i < NUM_COL; i++) {
		std::cout << "[ ";
		for (size_t j = 0; j < NUM_ROW; j++) {
			std::cout << std::hex << (int)cells[i][j] << " "; //change orientation of matrix by swapping i & j.
		}
		std::cout << "]" << std::dec << std::endl;
	}
	std::cout << std::endl;
}

/***************************************************************
The ciphertext consists of n 16-byte blocks.
Each state block is contructed in a matrix fashion.
It's filled vertically and then advances to the next column.

{a,b,c,e,f,g,h,y,j,k,i,l,m,n,o,p}

is transformed into the 4x4 matrix:

		[a,e,i,m]
		[b,f,j,n]
		[c,g,k,o]
		[d,h,l,p]
***************************************************************/
StateBlock::StateBlock(std::vector<BYTE>::iterator it) {
	for (size_t i = 0, z = 0; i < NUM_COL; i++) {
		for (size_t j = 0; j < NUM_ROW; j++) {
			cells[i][j] = *it++;
		}
	}
}

void StateBlock::subBytes() {
	for (size_t i = 0; i < NUM_COL; i++) {
		for (size_t j = 0; j < NUM_ROW; j++) {
			cells[j][i] = sbox[cells[j][i]];
		}
	}
}
void StateBlock::invSubBytes(){
	for (size_t i = 0; i < NUM_COL; i++) {
		for (size_t j = 0; j < NUM_ROW; j++) {
			cells[j][i] = rsbox[cells[j][i]];
		}
	}
}

void StateBlock::shiftRows() {
	// Rotate first row 1 columns to left  
	size_t temp = cells[0][1];
	cells[0][1] = cells[1][1];
	cells[1][1] = cells[2][1];
	cells[2][1] = cells[3][1];
	cells[3][1] = temp;

	// Rotate second row 2 columns to left  
	temp = cells[0][2];
	cells[0][2] = cells[2][2];
	cells[2][2] = temp;

	temp = cells[1][2];
	cells[1][2] = cells[3][2];
	cells[3][2] = temp;

	// Rotate third row 3 columns to left
	temp = cells[0][3];
	cells[0][3] = cells[3][3];
	cells[3][3] = cells[2][3];
	cells[2][3] = cells[1][3];
	cells[1][3] = temp;
}
void StateBlock::invShiftRows(){

	size_t temp;
	// Rotate first row 1 columns to right  
	temp = cells[3][1];
	cells[3][1] = cells[2][1];
	cells[2][1] = cells[1][1];
	cells[1][1] = cells[0][1];
	cells[0][1] = temp;

	// Rotate second row 2 columns to right 
	temp = cells[0][2];
	cells[0][2] = cells[2][2];
	cells[2][2] = temp;

	temp = cells[1][2];
	cells[1][2] = cells[3][2];
	cells[3][2] = temp;

	// Rotate third row 3 columns to right
	temp = cells[0][3];
	cells[0][3] = cells[1][3];
	cells[1][3] = cells[2][3];
	cells[2][3] = cells[3][3];
	cells[3][3] = temp;
}

BYTE StateBlock::xtime(BYTE x){
	return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}
BYTE StateBlock::multiply(BYTE x, BYTE y){
	return (((y & 1) * x) ^
		((y >> 1 & 1) * xtime(x)) ^
		((y >> 2 & 1) * xtime(xtime(x))) ^
		((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^
		((y >> 4 & 1) * xtime(xtime(xtime(xtime(x))))));
}

void StateBlock::mixColumns(){
	size_t i;
	size_t Tmp, Tm, t;
	for (i = 0; i < 4; ++i){

		t = cells[i][0];
		Tmp = cells[i][0] ^ cells[i][1] ^ cells[i][2] ^ cells[i][3];
		Tm = cells[i][0] ^ cells[i][1]; Tm = xtime(Tm);  cells[i][0] ^= Tm ^ Tmp;
		Tm = cells[i][1] ^ cells[i][2]; Tm = xtime(Tm);  cells[i][1] ^= Tm ^ Tmp;
		Tm = cells[i][2] ^ cells[i][3]; Tm = xtime(Tm);  cells[i][2] ^= Tm ^ Tmp;
		Tm = cells[i][3] ^ t;        Tm = xtime(Tm);  cells[i][3] ^= Tm ^ Tmp;
	}
}
void StateBlock::invMixColumns(){
	size_t a, b, c, d;
	for (size_t i = 0; i<4; ++i){
		a = cells[i][0];
		b = cells[i][1];
		c = cells[i][2];
		d = cells[i][3];

		cells[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
		cells[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
		cells[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
		cells[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
	}
}

void StateBlock::writeToFile(std::ofstream &out) {
	for (auto row : cells) { //output from left to right and top to bottom
		for (auto cell : row) {
			out << (char)cell;
		}
	}
}



