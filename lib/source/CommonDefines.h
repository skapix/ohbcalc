#pragma once

#include <string_view>

using CStringView = std::string_view;


size_t getFirstNonEmptySymbol(CStringView str);

char lastIdentificator(CStringView str);