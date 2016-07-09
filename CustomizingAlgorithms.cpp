#include <iostream>
#include <algorithm>
#include <string>

bool isShorter(const std::string& a, const std::string& b) {
	return a.size() < b.size();
}

int main() {

  std::vector<std::string> words = { "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "turtle" };

	std::cout << "The words in the vector are...\n";
	for (auto &word : words) {
		std::cout << word << std::endl;
	}

	std::sort(words.begin(), words.end()); //words is sorted using operator '<' by default, which sorts the words alphabetically
	
	std::cout << "After sorting the words in the vector...\n";
	for (auto &word : words) {
		std::cout << word << std::endl;
	}

	std::sort(words.begin(), words.end(), isShorter); // call sort using custom predicate...
	
	/*
	Instead of calling sort and having it sort our strings alphabetically, it'll use the predicate and sort the strings
	by length. I wouldn't call this overloading, but it's a similar concept to customize some of the standard algorithms.
	*/

	std::cout << "After sorting the words in the vector...\n";
	for (auto &word : words) {
		std::cout << word << std::endl;
	}

	return 0;
}
