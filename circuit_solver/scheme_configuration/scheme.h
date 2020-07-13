#pragma once

#include "../type_definitions.h"
#include "pin_matrix.h"
#include "nodes.h"
#include "branches.h"
#include "inc_matrix.h"
#include "loops.h"
#include "../system/equations.h"

// класс для представления электрической схемы
class Scheme
{
private:																				// префикс m указывает на то, что переменная является членом класса
	std::vector<Element> mElements;														// вектор элементов
	PinMatrix mPinMatrix;																// объект для матрицы соединений пинов
	Nodes mNodes;																		// объект для узлов и вспомогательных структур
	Branches mBranches;																	// объект для ветвей и вспомогательных структур
	IncMatrix mIncMatrix;																// объект для матрицы инцидентности
	Loops mLoops;																		// объект для контуров
	Equations mEquations;																// объект для системы уравнений

	size_t mCurrentSourceCount = 0;														// счетчик источников тока
	size_t mVoltageSourceCount = 0;														// счетчик источников напряжения

public:
	void add(const Element& element);													// метод добавления в схему нового элемента
	void update();																		// метод обновления схемы
	void setElementValue(size_t index, double value);									// метод изменения значения элемента
	std::string toString(size_t precision = 3) const;									// метод получения строкового представления схемы с точностью значений precision
};