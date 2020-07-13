#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>
#include <map>

#include "structure_units/coefficient.h"
#include "structure_units/element.h"

// переопределение названий типов для краткости
typedef std::vector<std::vector<bool>> bool_matr_t;
typedef std::vector<std::vector<size_t>> size_matr_t;
typedef std::vector<std::vector<int>> int_matr_t;
typedef std::vector<std::vector<double*>> doublep_matr_t;
typedef std::vector<std::vector<Coefficient>> coeff_matr_t;
typedef std::vector<Coefficient> coeff_vect_t;
typedef std::vector<int> int_vect_t;
typedef std::vector<size_t> size_vect_t;
typedef std::vector<bool> bool_vect_t;
typedef std::vector<Element> elem_vect_t;

// константы для типов элементов схемы
const int RESISTANCE_TYPE = 0;
const int VOLTAGE_SOURCE_TYPE = 1;
const int CURRENT_SOURCE_TYPE = 2;

static const double ZERO = 0.0;
static const double ONE = 1.0;

static const double* ZERO_PTR = &ZERO;
static const double* ONE_PTR = &ONE;
