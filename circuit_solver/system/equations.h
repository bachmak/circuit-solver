#pragma once

#include "../type_definitions.h"
#include "../scheme_configuration/nodes.h"
#include "../scheme_configuration/branches.h"
#include "../scheme_configuration/inc_matrix.h"
#include "../scheme_configuration/loops.h"

class Equations
{
public:
	void update(const IncMatrix& matrix, const Loops& loops, const Branches& branches, const elem_vect_t& elements);
	const coeff_matr_t& left() const;
	const coeff_matr_t& right() const;
	std::string toString(size_t precision) const;

private:
	coeff_matr_t mLeftPart;
	coeff_matr_t mRightPart;
	size_t mUnknownCurrentCount = 0;
	size_t mCurrentSourceCount = 0;
	size_t mVoltageSourceCount = 0;
	size_t mKnownSourceCount = 0;
	size_t mFirstLawCount = 0;
	size_t mSecondLawCount = 0;

	void update1stLawEquations(const IncMatrix& matrix, const elem_vect_t& elements);
	void update2ndLawEquations(const Loops& loops, const Branches& branches, const elem_vect_t& elements);

	size_t getCurrentSourceElemIndex(const size_t currentSourceIndex, const elem_vect_t& elements) const;
	size_t getVoltageSourceIndex(const size_t elemIndex, const elem_vect_t& elements) const;
};
