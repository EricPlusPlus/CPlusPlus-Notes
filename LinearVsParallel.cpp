/***********************************************
Operating Systems #12.27
By Eric Pratt

Write programs that enter randomly generated short 
strings into an array and then can search the array 
for a given string using (a) a simple linear search 
(brute force), and (b) a more sophisticated method 
of your choice. Recompile your programs for array 
sizes ranging from small to as large as you can 
handle on your system. Evaluate the performance of 
both approaches. Where is the break-even point?

Method a is a simple linear search.
Method b is a parallel linear search.

Change the position of the target word "needle"
in the function generateWords().

Change the size of the vector in main().

The break even point is around 500k words with
the needle positioned at the back.
***********************************************/

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <thread>
#include <chrono>
#include <random>

int numDigits(int x)
{
	x = abs(x);
	return (x < 10 ? 1 :
		(x < 100 ? 2 :
			(x < 1000 ? 3 :
				(x < 10000 ? 4 :
					(x < 100000 ? 5 :
						(x < 1000000 ? 6 :
							(x < 10000000 ? 7 :
								(x < 100000000 ? 8 :
									(x < 1000000000 ? 9 :
										10)))))))));
}


std::default_random_engine e(std::random_device{}());

void generateWords(std::vector<std::string> &words, int minLength = 6, int maxLength = 12) {
	for (auto &word : words) {
		int wordLength = (maxLength - minLength) + (e() % minLength); 

		//add a random character one by one to word
		for (int i = 0; i < wordLength; i++) {
			char c = 'a' + (e() % 26);
			word.push_back(c);
		}
	}

	//Uncomment the line below to place the needle in a random position.
	///words[e() % words.size()] = "needle";
	*(words.end() - 1) = "needle";
}


void linearSearch(std::vector<std::string> &words) {
	using namespace std::chrono;

	auto start = high_resolution_clock::now();
	decltype(start) end;

	for (auto &word : words) {
		if (word == "needle") {
			end = high_resolution_clock::now();
			break;
		}
	}
	
	duration<double> dur = end - start;
	auto timespan = duration_cast<microseconds> (dur);
	std::cout << "The regular linear search took " << timespan.count() << " microseconds\n";
}

void threadSearch(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::chrono::high_resolution_clock::time_point start) {
	using namespace std::chrono;
	auto word = begin;

	while (word != end) {
		if (*word == "needle") {
			auto dur = high_resolution_clock::now() - start;
			auto timespan = duration_cast<microseconds> (dur);

			std::cout << "The parallel search took " << timespan.count() << " microseconds\n";
			break;
		}

		word++;
	}
}



void parallelSearch(std::vector<std::string> &words) {
	int threadCount = numDigits(words.size());
	std::cout << "There will be " << threadCount << " threads\n";

	std::vector<std::thread> threads(threadCount);

	int i = 0;
	auto time = std::chrono::high_resolution_clock::now();

	for (auto &thread : threads) {
		//Divide elements among each thread equally
		auto begin = words.begin() + (i * words.size() / threadCount);
		auto end = words.begin() + ((++i) * words.size() / threadCount);

		thread = std::thread(threadSearch, begin, end, time);
		thread.detach();
	}
}


int main() {
	std::vector<std::string> words(1'000'000,""); 
	generateWords(words);
	std::cout << "Finished creating words\n";

	parallelSearch(words);
	linearSearch(words);

	system("pause");
	return 0;
}
