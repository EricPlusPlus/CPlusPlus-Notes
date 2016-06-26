int main() {
	std::shared_ptr<int> p(new int(42)); // a shared_ptr 'p' is created that points to a new int with value 42.
	
	std::cout << "Count: " << p.use_count() << std::endl; // there is one pointer that points to the same address as p (itself), so use_count returns 1. 
	{
		std::shared_ptr<int> local(p); // local points to the same address as p; it's constructed using a copy of p

		*local = 50; 
		std::cout << "Count: " << local.use_count() << std::endl; // local is a copy of 'p' and since smart pointers keep track of the # of copies, use_count() returns 2

		//at this instant in the program, the reference count (use_count) is 2
	} //once this block has ended, local is destroyed

	//At the time local was destroyed, the reference count was 2. This means and another shared_ptr (in this case 'p') were pointing to the same location.
	//Since multiple pointers are using memory at that address, it will NOT be freed.

	std::cout << "Count: " << p.use_count() << std::endl;
	std::cout << "x = " << *p << std::endl; // x has been modified as expected.



	system("pause");

	return 0; //When the program ends, the reference count of p is 1, so the memory p is pointing to is freed just before p is destroyed. 
}