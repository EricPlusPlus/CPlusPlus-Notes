#include <unordered_map>
#include <string>

class Trie {
private:
	bool search(const std::string&, Trie*);
	void remove(Trie*);

public:
	Trie() = default; //root
	Trie(char c) : val(c) {}
	~Trie();

	char val;
	bool isEndPoint = false;

	std::unordered_map<char, Trie*> children;

	void addWord(const std::string&);
	bool search(const std::string&);
};

Trie::~Trie() {
	for (auto &node : children) {
		remove(node.second);
	}
}

void Trie::remove(Trie* root) {
	if (root->children.size() == 0) {
		delete root;
		return;
	}

	for (auto &node : root->children) {
		remove(node.second);
	}
}

void Trie::addWord(const std::string& word) {
	if (word.empty()) return;

	Trie* runner = this;

	for (auto &c : word) {
		if (runner->children.find(c) != runner->children.end()) {
			runner = runner->children[c];
		}
		else {
			runner->children[c] = new Trie(c);
			runner = runner->children[c];
		}
	}

	runner->isEndPoint = true;
}

bool Trie::search(const std::string& word, Trie* root) {
	if (word.empty() && root->isEndPoint) return true;

	char c = word.front();

	if (root->children.find(c) != root->children.end()) {
		return search(word.substr(1), root->children[c]);
	}
	return false;
}

bool Trie::search(const std::string &word) {
	return search(word, this);
}
