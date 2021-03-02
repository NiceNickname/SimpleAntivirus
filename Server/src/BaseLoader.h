#pragma once
#include "Base.h"

class BaseLoader
{
public:
	static Base* Load(const std::u16string& path);
};