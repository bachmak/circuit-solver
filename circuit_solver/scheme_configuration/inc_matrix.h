#pragma once

#include "../type_definitions.h"
#include "elements.h"
#include "nodes.h"
#include "branches.h"

// класс для представления матрицы инцидентности схемы
class IncMatrix
{
public:
	void update(const Nodes& nodes, const Branches& branches, const Elements& elements);	// метод обновления матрицы
	size_t size() const;																	// геттер размера матрицы
	size_t getUnknownCurrentCount() const;													// геттер счетчика неизвестных токов
	const int_vect_t& operator[] (size_t index) const;										// геттер строки матрицы (одномерного вектора)
	std::string toString() const;															// метод получения строкового представления матрицы

private:
	int_matr_t mMatrix;																		// двумерная матрица для хранения информации об инцидентности ветвей узлам
	size_t mUnknownCurrentCount = 0;														// счетчик неизвестных токов
	void orient(const Nodes& nodes, const Branches& branches, const Elements& elements);	// метод выбора направления токов (ориентирования матрицы)
};
