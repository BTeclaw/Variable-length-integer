#include "BigInt.h"

int main()
{
	BigInt test_big_int;
	BigInt test_big_int1;
	test_big_int.defineBigInt();
	test_big_int1.defineBigInt();
	std::cout << "Printing numbers by using Print() method:\n";
	test_big_int.Print();
	test_big_int1.Print();
	BigInt sum = test_big_int + test_big_int1;
	std::cout << "Printing sum of those two numbers by using operator overriden method:\n";
	std::cout << sum;
	BigInt difference = sum - test_big_int1;
	std::cout << "Printing difference of those two numbers by using operator overriden method:\n";
	std::cout << difference;

	return 0;
}