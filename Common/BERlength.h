#pragma once

#ifdef _WIN32
#else
	#include <sys/types.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#pragma stdlib_stddef_h
#endif

class BERlength
{
public:
	BERlength();
	BERlength(unsigned long value);
	BERlength(const char* data, size_t length);
	BERlength(const BERlength& value);
	size_t getLength() const;
	const char* getBytes() const;
	bool isZero() const;
	bool isShort() const;
	unsigned long getValue() const;
	BERlength operator+(unsigned long number);
	BERlength operator+(BERlength number);
	BERlength operator-(unsigned long number);
	BERlength operator-(BERlength number);
private:
	void normalize();
	size_t getNumberLength(unsigned long number);
	BERlength shortToLong(unsigned char input);
	char _storage[128];
	size_t _size;
};

