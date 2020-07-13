#pragma once

#include "../type_definitions.h"

class Elements
{
public:
	void add(const Element& element);
	const Element& operator[] (size_t index) const;
	const size_t size() const;
private:
	elem_vect_t mElements;
	size_t mCurrentSourceCount = 0;
	size_t mVoltageSourceCount = 0;
};
