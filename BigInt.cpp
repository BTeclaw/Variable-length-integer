#define _CRT_SECURE_NO_WARNINGS 1
#include "BigInt.h"
#define MAX_DECIMAL_PLACES 18
#define MAX_CHUNK_VALUE 1000000000000000000 // 10^18 - maximal value stored in one chunk can be equal to 999999999999999999
#define BASE_BUFFER_SIZE 32
#define EXTENSION_CONSTANT 2 
//BUFFER DEFINITION AND EXTENSION
struct buffer_t
{
	char* buffer;
	__int64 buffer_length;
	__int64 written_chars;
};

void defineBuffer(buffer_t* buffer)
{
	buffer->buffer = (char*)calloc(BASE_BUFFER_SIZE,sizeof(char));
	buffer->buffer_length = BASE_BUFFER_SIZE;
	buffer->written_chars = 0;
}
void extendBuffer(buffer_t* buffer)
{
	char* new_buffer = (char*)calloc(buffer->buffer_length * EXTENSION_CONSTANT, sizeof(char));
	strcpy(new_buffer, buffer->buffer);
	free(buffer->buffer);
	buffer->buffer_length *= EXTENSION_CONSTANT;
	buffer->buffer = new_buffer;
}
//Simple power function
__int64 Power(__int64 base, __int64 exponent)
{
	if (exponent == 0)
		return 1;
	else
	{
		__int64 power = base;
		for (__int64 i = 1; i < exponent; i++)
		{
			power *= base;
		}
		return power;
	}
}

BigInt::BigInt()
{
	this->chunk_number = 0;
	this->head_chunk = NULL;
	this->tail_chunk = NULL;
	this->minus_sign = false;
}

BigInt::BigInt(chunk_t* head, chunk_t* tail, bool minus_sign)
{
	this->head_chunk = head;
	this->tail_chunk = tail;
	this->minus_sign = minus_sign;
	this->chunk_number = 0;
	chunk_t* current = head;
	while (current != NULL)
	{
		this->chunk_number++;
		current = current->next;
	}
}


void BigInt::defineBigInt()
{
	fprintf(stdout, "Please input your big integer.\n");
	buffer_t buffer;
	defineBuffer(&buffer);
	__int64 written_chars=0;
	while (true)
	{
		char buffer_char=getchar();
		if (buffer_char == '\n')
		{
			buffer.written_chars = written_chars;
			break;
		}
		else if (buffer_char == '-')
			this->minus_sign = true;
		else if (buffer_char >= '0' && buffer_char <= '9')
		{
			if (written_chars == buffer.buffer_length - 1)
			{
				extendBuffer(&buffer);
				buffer.buffer[written_chars] = buffer_char;
				written_chars++;
			}
			else
			{
				buffer.buffer[written_chars] = buffer_char;
				written_chars++;
			}
		}
		else
		{
			fprintf(stdout, "WRONG INPUT");
			break;
		}
	}
	while (buffer.written_chars != 0)
	{
		this->chunk_number++;
		chunk_t* current_chunk=(chunk_t*)malloc(sizeof(chunk_t));
		current_chunk->chunk = 0;
		current_chunk->next = NULL;
		current_chunk->prev = NULL;
		for (__int64 i = 0; i < MAX_DECIMAL_PLACES && buffer.written_chars!=0; i++)
		{
			current_chunk->chunk += (__int64)(buffer.buffer[buffer.written_chars - 1] - '0')*Power(10,i);
			buffer.written_chars--;
		}
		if (this->head_chunk == NULL)
			this->head_chunk = current_chunk;
		else
		{
			current_chunk->next = this->head_chunk;
			this->head_chunk->prev = current_chunk;
			this->head_chunk = this->head_chunk->prev;
		}
	}
	free(buffer.buffer);
	chunk_t* tail_chunk=this->head_chunk;
	while (tail_chunk->next != NULL)
	{
		tail_chunk = tail_chunk->next;
	}
	this->tail_chunk = tail_chunk;
}

void BigInt::Print()
{
	if (this->minus_sign)
		fprintf(stdout, "-");
	fprintf(stdout, "%lld", this->head_chunk->chunk);
	chunk_t* current_chunk = this->head_chunk->next;
	while (current_chunk != NULL)
	{
		fprintf(stdout, "%018lld", current_chunk->chunk);
		current_chunk = current_chunk->next;
	}
	fprintf(stdout, "\n");
}

BigInt& BigInt::operator+(const BigInt& added_number)
{
	if (this->head_chunk!=NULL && added_number.head_chunk!=NULL)
	{
		if ((!this->minus_sign && !added_number.minus_sign)||(this->minus_sign&&added_number.minus_sign))
		{
			bool minus_sign = this->minus_sign;
			chunk_t* current_one;
			chunk_t* current_two;
			__int64 bigger_chunk_number;
			if (this->chunk_number >= added_number.chunk_number)
			{
				current_one = this->tail_chunk;
				current_two = added_number.tail_chunk;
				bigger_chunk_number = this->chunk_number;
			}
			else
			{
				current_one = added_number.tail_chunk;
				current_two = this->tail_chunk;
				bigger_chunk_number = added_number.chunk_number;
			}
			chunk_t* tail = (chunk_t*)malloc(sizeof(chunk_t));
			tail->chunk = 0;
			tail->next = NULL;
			tail->prev = NULL;
			chunk_t* current_sum = tail;
			__int64 carry = 0;
			for (__int64 i = 0; i < bigger_chunk_number; i++)
			{
				if (current_one != NULL && current_two != NULL)
				{
					current_sum->chunk = current_one->chunk + current_two->chunk + carry;
					carry = 0;
					current_one = current_one->prev;
					current_two = current_two->prev;
					if (current_sum->chunk >= MAX_CHUNK_VALUE)
					{
						carry = 1;
						current_sum->chunk -= MAX_CHUNK_VALUE;
					}
				}
				else if (current_one != NULL && current_two == NULL)
				{
					current_sum->chunk = current_one->chunk + carry;
					carry= 0;
					if (current_sum->chunk >= MAX_CHUNK_VALUE)
					{
						carry = 1;
						current_sum->chunk -= MAX_CHUNK_VALUE;
					}
					current_one = current_one->prev;
				}
				if (current_one != NULL)
				{
					current_sum->prev = (chunk_t*)malloc(sizeof(chunk_t));
					current_sum->prev->next = current_sum;
					current_sum = current_sum->prev;
					current_sum->chunk = 0;
					current_sum->prev = NULL;
				}
			}
			if (carry > 0)
			{
				current_sum->prev = (chunk_t*)malloc(sizeof(chunk_t));
				current_sum->prev->next = current_sum;
				current_sum = current_sum->prev;
				current_sum->chunk = carry;
				current_sum->prev = NULL;
			}
			BigInt* return_sum=new BigInt(current_sum,tail,minus_sign);
			return *return_sum;
		}
		else
		{
			if (this->chunk_number!=added_number.chunk_number)
			{
				bool minus_sign = (this->chunk_number > added_number.chunk_number) ? this->minus_sign : added_number.minus_sign;
				__int64 chunk_number= (this->chunk_number > added_number.chunk_number) ? this->chunk_number : added_number.chunk_number;
				chunk_t* current_one = (this->chunk_number > added_number.chunk_number) ? this->tail_chunk : added_number.tail_chunk;
				chunk_t* current_two = (this->chunk_number < added_number.chunk_number) ? this->tail_chunk : added_number.tail_chunk;
				chunk_t* tail = (chunk_t*)malloc(sizeof(chunk_t));
				tail->chunk = 0;
				tail->next = NULL;
				tail->prev = NULL;
				chunk_t* current_sum = tail;
				__int64 carry = 0;
				for (__int64 i = 0; i < chunk_number; i++)
				{
					if (current_one!=NULL && current_two!=NULL)
					{
						current_sum->chunk = current_one->chunk - current_two->chunk + carry;
						carry = 0;
						current_one = current_one->prev;
						current_two = current_two->prev;
						if (current_sum->chunk < 0)
						{
							carry = -1;
							current_sum->chunk += MAX_CHUNK_VALUE;
						}
					}
					else if (current_one != NULL && current_two == NULL)
					{
						current_sum->chunk = current_one->chunk + carry;
						current_one = current_one->prev;
						carry = 0;
						if (current_sum->chunk < 0)
						{
							carry = -1;
							current_sum->chunk += MAX_CHUNK_VALUE;
						}
					}
					if (current_one != NULL)
					{
						current_sum->prev = (chunk_t*)malloc(sizeof(chunk_t));
						current_sum->prev->next = current_sum;
						current_sum = current_sum->prev;
						current_sum->chunk = 0;
						current_sum->prev = NULL;
					}
				}
				BigInt* return_sum = new BigInt(current_sum, tail, minus_sign);
				return *return_sum;
			}
			else
			{
				chunk_t* current_one = this->tail_chunk;
				chunk_t* current_two = added_number.tail_chunk;
				__int64 carry = 0;
				chunk_t* tail = (chunk_t*)malloc(sizeof(chunk_t));
				tail->chunk = 0;
				tail->next = NULL;
				tail->prev = NULL;
				chunk_t* current_sum = tail;
				for (__int64 i = 0; i < this->chunk_number; i++)
				{
					if (current_one != NULL && current_two != NULL)
					{
						current_sum->chunk = current_one->chunk - current_two->chunk + carry;
						carry = 0;
						current_one = current_one->prev;
						current_two = current_two->prev;
						if (current_sum->chunk < 0)
						{
							carry = -1;
							current_sum->chunk += MAX_CHUNK_VALUE;
						}
					}
					if (current_one != NULL && current_two != NULL)
					{
						current_sum->prev = (chunk_t*)malloc(sizeof(chunk_t));
						current_sum->prev->next = current_sum;
						current_sum = current_sum->prev;
						current_sum->chunk = 0;
						current_sum->prev = NULL;
					}
				}
				if (carry < 0)
				{
					current_sum->prev = (chunk_t*)malloc(sizeof(chunk_t));
					current_sum->prev->next = current_sum;
					current_sum = current_sum->prev;
					current_sum->chunk = carry;
					current_sum->prev = NULL;
				}
				while (current_sum->chunk == 0 && current_sum->next != NULL)
					current_sum = current_sum->next;
				bool minus_sign = (current_sum->chunk < 0) ? added_number.minus_sign : this->minus_sign;
				if (current_sum->chunk < 0)
					current_sum->chunk *= -1;
				if (current_sum->chunk == 0)
					minus_sign = false;
				BigInt* return_sum = new BigInt(current_sum, tail, minus_sign);
				return *return_sum;
			}
		}
	}
}

BigInt& BigInt::operator-(BigInt& subtracted_number)
{
	subtracted_number.minus_sign = !subtracted_number.minus_sign;
	BigInt* return_bigint = new BigInt();
	*return_bigint = *this + subtracted_number;
	subtracted_number.minus_sign = !subtracted_number.minus_sign;
	return *return_bigint;
}

std::ostream& operator<<(std::ostream& out,const BigInt& big_int)
{
	chunk_t* current = big_int.head_chunk;
	if (big_int.minus_sign)
		out << "-";
	out << current->chunk;
	current = current->next;
	while (current != NULL)
	{
		out << std::setfill('0') << std::setw(MAX_DECIMAL_PLACES) << current->chunk;
		current = current->next;
	}
	out << "\n";
	return out;
}

BigInt::~BigInt()
{
	while (this->head_chunk != NULL)
	{
		if (this->head_chunk->next == NULL)
		{
			free(this->head_chunk);
			this->head_chunk = NULL;
		}
		else
		{
			this->head_chunk = this->head_chunk->next;
			free(this->head_chunk->prev);
		}
	}
}
