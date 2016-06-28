struct X {
	X(const std::string &s = std::string("Lulz")):server(new std::string(s)), cost(0) { 
	std::cout << "Calling default constructor... X()" << std::endl; 
	} // default constructor
	X(const X& temp):cost(temp.cost), server(new std::string(*temp.server)) { std::cout << "X(const X&)" << std::endl; } //copy constructor
	X& operator=(const X& temp) {
		this->cost = temp.cost;

		std::cout << "Before deletion, server points to: " << server << " which is non-nullptr.\nTherefore we must deallocate it.\n";
		delete server; //when a struct is created, server is default initialized with (new std::string()), making it non-nullptr
		server = new std::string(*temp.server); //server points to a new copy of *temp.server
		return *this;
	
	} //copy assignment operator
	~X() {
		std::cout << "Calling destructor..." << std::endl; 
		delete server;
	
	} //destructor

	std::string* server = nullptr;
private:
	int cost;

};


int main() {

	X structA;
	X structB;

	//structA = structB;

	std::cout << "structA->server points to: " << structA.server << std::endl;
	std::cout << "structB->server points to: " << structB.server << std::endl;

	X structC(structA);
	std::cout << "structC->server points to: " << structC.server << std::endl;


	system("pause");
	return 0;
}
