#pragma once

#include "../type_definitions.h"
#include "inc_matrix.h"
#include "../structure_units/node.h"

class Loops
{
public:
	void update(const IncMatrix& matrix);
	size_t size() const;
	const int_vect_t& operator[] (size_t index) const;
	std::string toString() const;

private:
	int_matr_t mLoops;
};
