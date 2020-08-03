#pragma once
#include <iostream>
#include <iomanip>
struct chunk_t
{
	__int64 chunk;
	chunk_t* next;
	chunk_t* prev;
};

class BigInt
{
private:
	bool minus_sign;
	chunk_t* head_chunk;
	chunk_t* tail_chunk;
	__int64 chunk_number;
public:
	BigInt& operator+(const BigInt& added_number);
	BigInt& operator-(BigInt& subtracted_number);
	friend std::ostream& operator<<(std::ostream& out,const BigInt& big_int);
	BigInt();
	BigInt(chunk_t* head, chunk_t* tail,bool minus_sign);
	void defineBigInt();
	void Print();
	~BigInt();
};

