#pragma once

#include <gsl/gsl>
#include <gsl/string_span>
#include <gsl/span>

using CStringView = gsl::cstring_span<>;

template <class T>
using ArrayView = gsl::span<T>;

size_t getFirstNonEmptySymbol(CStringView str);

char lastIdentificator(CStringView str);