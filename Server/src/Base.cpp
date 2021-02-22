#include "Base.h"

Base::Base(std::unordered_multimap<uint64_t, Record>&& base)
{
	this->base = std::move(base);
}

Base::Base(Base&& other)
{
	this->base = std::move(other.base);
}

Base& Base::operator=(Base&& other)
{
	this->base = std::move(other.base);
	return *this;
}

