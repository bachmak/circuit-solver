#pragma once

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include "structure_units/coefficient.h"
#include "structure_units/element.h"

namespace CS
{
// переопределение названий типов для краткости
using BoolMatr = std::vector<std::vector<bool>>;
using SizeMatr = std::vector<std::vector<size_t>>;
using IntMatr = std::vector<std::vector<int>>;
using DoublePtrMatr = std::vector<std::vector<double*>>;
using CoeffMatr = std::vector<std::vector<Coefficient>>;
using CoeffVect = std::vector<Coefficient>;
using IntVect = std::vector<int>;
using SizeVect = std::vector<size_t>;
using BoolVect = std::vector<bool>;
using ElemVect = std::vector<Element>;

// константы для коэффициентов системы уравнений
static const double ZERO = 0.0;
static const double ONE = 1.0;

static const double* ZERO_PTR = &ZERO;
static const double* ONE_PTR = &ONE;

} // namespace CS
