#pragma once
#include <unordered_map>
#include <string>

struct Record
{
	std::u16string name, type;
	uint64_t length, sigStart, offsetStart, offsetEnd, sha256;
};

class Base
{
public:
	Base() = default;
	Base(std::unordered_multimap<uint64_t, Record>&& base);
	Base(Base&& other);
	Base& operator=(Base&& other);
private:
	std::unordered_multimap<uint64_t, Record> base;
};