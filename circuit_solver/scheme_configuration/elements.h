#pragma once

#include "../type_definitions.h"

class Elements
{
public:
	void add(const Element& element);
	const Element& operator[] (size_t index) const;
    Element& operator[] (size_t index);
	size_t size() const;
    size_t getCurrentSourceCount() const;
    size_t getVoltageSourceCount() const;
    size_t getResistanceCount() const;

private:
	elem_vect_t mElements;
	size_t mCurrentSourceCount = 0;
	size_t mVoltageSourceCount = 0;
    size_t mResistanceCount = 0;
};
