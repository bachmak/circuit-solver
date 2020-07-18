#pragma once

#include "../type_definitions.h"
#include "elements.h"

// класс для представления матрицы соединений пинов
class PinMatrix
{
public:
	void update(const Elements& elements);	    											// метод обновления матрицы
	const size_t size() const;																// геттер размера матрицы
	const bool_vect_t& operator[] (size_t index) const;										// геттер строки матрицы (одномерного вектора)
	std::string toString() const;															// метод получения строкового представления матрицы

private:
	bool_matr_t mMatrix;																	// логическая двумерная матрица для хранения информации о соединениях пинов
};
