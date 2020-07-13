#pragma once

#include "../type_definitions.h"
#include "pin_matrix.h"

// класс для представления узлов схемы
class Nodes
{
public:
	void update(const PinMatrix& pinMatrix);												// метод обновления всех структур
	const size_vect_t& operator[] (size_t index) const;										// геттер строки списка узлов mNodes (одномерного вектора)
	const int_vect_t& getNodesForPin() const;												// геттер вектора mNodesForPin
	const bool_matr_t& getNodesOnPins() const;												// геттер матрицы mNodesOnPins
	size_t size() const;																	// геттер размера списка узлов mNodes
	std::string toString() const;															// метод получения строкового представления списка узлов mNodes

private:
	size_matr_t mNodes;																		// вектор векторов номеров пинов, представляющий список узлов схемы
	int_vect_t mNodesForPin;																// вектор номеров узлов, в которые входят пины
	bool_matr_t mNodesOnPins;																// матрица-отношение между множеством узлов и пинов

	void updateNodesOnPins(const size_t pinCount);											// метод обновления матрицы mNodesOnPins
};
