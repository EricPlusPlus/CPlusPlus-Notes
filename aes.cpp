#include <iostream>
#include <vector>
#include <fstream>

#include "Block.h"

void keyCoreSchedule(std::vector<BYTE> &t, size_t iter) {
	BYTE temp = 0;

	//rotate 8 bits (one byte) left
	temp = t[0];
	t[0] = t[1];
	t[1] = t[2];
	t[2] = t[3];
	t[3] = temp;

	//apply sbox to all 4 bytes
	for (size_t i = 0; i < NUM_ROW; i++) {
		t[i] = sbox[t[i]];
	}

	//rcon
	t[0] = t[0] ^ rcon[(iter * 4) / NUM_COL];
}

size_t getNumRounds(size_t keylength) {
	size_t rounds = 0;
	switch (keylength) {
	case 16:
		rounds = 10;
		break;
	case 24:
		rounds = 12;
		break;
	case 32:
		rounds = 14;
		break;
	}

	return rounds;
}

std::vector<Block> rijindaelKeySchedule(const std::string key) {
	std::vector<Block> RoundKey;
	RoundKey.push_back(Block(key)); //initial key

	size_t rounds = getNumRounds(key.size());
	//RoundKey[0].printMatrix();

	size_t i = 1;
	while (i < rounds + 1) { //iterate an additional round for the intial key
		std::vector<BYTE> t = RoundKey[i - 1].cells[3];

		keyCoreSchedule(t, i);
		Block keyBlock;

		for (int j = 0; j < BLOCKSIZE; j++) { // it takes 4 iterations to XOR t.
			t[j % 4] = keyBlock.cells[j / NUM_COL][j % NUM_ROW] = RoundKey[i - 1].cells[j / NUM_COL][j % NUM_ROW] ^ t[j % NUM_ROW];
		}

		if (rounds == 14) { // 256 bit key
			//apply sbox to all 4 bytes
			for (size_t i = 0; i < 4; i++) {
				t[i] = sbox[t[i]];
			}

			for (int j = 0; j < 12; j++) {
				t[j % 4] = keyBlock.cells[j / NUM_COL][j % NUM_ROW] = RoundKey[i - 1].cells[j / NUM_COL][j % NUM_ROW] ^ t[j % NUM_ROW];
			}
		}
		else if (rounds == 12) {// 192 bit key
			for (int j = 0; j < 8; j++) {
				t[j % 4] = keyBlock.cells[j / NUM_COL][j % NUM_ROW] = RoundKey[i - 1].cells[j / NUM_COL][j % NUM_ROW] ^ t[j % NUM_ROW];
			}
		}

		//keyBlock.printMatrix();
		RoundKey.push_back(keyBlock);
		i++;
	}
	return RoundKey;
}

void cipher(StateBlock &state, std::vector<Block> &RoundKeys) {
	auto key = RoundKeys.front();
	state ^= key;

	for (size_t i = 1; i < RoundKeys.size() - 1; i++) {
		state.subBytes();
		state.shiftRows();
		state.mixColumns();
		state ^= RoundKeys[i];
	}

	state.subBytes();
	state.shiftRows();

	state ^= RoundKeys.back();
}
void invCipher(StateBlock &state, std::vector<Block> &RoundKeys) {
	auto key = RoundKeys.back();
	state ^= key;

	for (size_t round = RoundKeys.size() - 2; round > 0; round--) {
		state.invShiftRows();
		state.invSubBytes();
		state ^= RoundKeys[round];
		state.invMixColumns();
	}

	state.invShiftRows();
	state.invSubBytes();
	state ^= RoundKeys.front();
}

void addPadding(std::vector<BYTE> &plaintext) {
	size_t remainder = plaintext.size() % 16;

	if (remainder == 0) return;
	int size = (plaintext.size() + (16 - remainder));

	for (int j = plaintext.size(); plaintext.size() < size; j++) { // add 0s for padding
		plaintext.push_back(0);
	}
}

void aesEncrypt(std::string filename, std::string key) {
	if (!(key.length() == 16 || key.length() == 24 || key.length() == 32)) {
		std::cout << "Key is not a valid length\n";
		return;
	}

	auto RoundKeys = rijindaelKeySchedule(key); //todo fix 192bit and 256bit keys in schedule.

	std::ifstream input(filename, std::ios::binary);

	if (input.fail()) {
		std::cerr << "Failed to open file\n";
		return;
	}

	std::vector<BYTE> plaintext((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	input.close();

	addPadding(plaintext);
	std::ofstream out(filename + "_aes_enc", std::ios::binary);

	for (auto it = plaintext.begin(); it != plaintext.end(); it += 16) {
		StateBlock state(it);
		cipher(state, RoundKeys);
		state.writeToFile(out);
	}

	out.close();
}
void aesDecrypt(std::string filename, std::string key) {
	if (!(key.length() == 16 || key.length() == 24 || key.length() == 32)) {
		std::cout << "Key is not a valid length\n";
		return;
	}

	auto RoundKeys = rijindaelKeySchedule(key);

	std::ifstream input(filename, std::ios::binary);

	if (input.fail()) {
		std::cerr << "Failed to open file\n";
		return;
	}

	std::vector<BYTE> ciphertext((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	input.close();

	std::ofstream out(filename + "_decrypted.txt", std::ios::binary);

	for (size_t i = 0; i < ciphertext.size(); i += 16) {
		StateBlock state(ciphertext.begin() + i);
		invCipher(state, RoundKeys);
		state.writeToFile(out);
	}

	out.close();
}


int main() { // todo add cli commands [-e encrypt][-d decrypt][-i filename][-k key][-o outfilename][-p print]
	std::string key("YELLOW SUBMARINE");

	aesEncrypt("eric.txt", key);
	system("pause");
	return 0;
}
