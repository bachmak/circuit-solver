#pragma once

#include "../type_definitions.h"
#include "pin_matrix.h"
#include "nodes.h"

// класс для представления ветвей схемы
class Branches
{
public:
	void update(const PinMatrix& matrix, const Nodes& nodes, const elem_vect_t& elements);	// метод обновления всех структур
	const size_matr_t& get() const;															// геттер для списка ветвей mBranches
	const size_vect_t& operator[] (size_t index) const;										// геттер строки списка ветвей mBranches (одномерного вектора)
	const bool_matr_t& getBranchesOnPins() const;											// геттер матрицы mBranchesOnPins
	size_t size() const;																	// геттер размера списка ветвей mBranches
	size_t getCurrentSourceCount() const;													// геттер счетчика источников тока
	std::string toString() const;															// метод получения строкового представления списка ветвей mBranches

private:
	size_matr_t mBranches;																	// вектор векторов номеров пинов, представляющий список ветвей схемы
	bool_matr_t mBranchesOnPins;															// матрица-отношение между множеством ветвей и пинов
	size_t mCurrentSourceCount = 0;															// счетчик источников тока

	void sortBranches(bool_vect_t branchesWithCurrentSource);								// метод сортировки ветвей в списке mBranches по наличию источника тока
	void updateBranchesOnPins(const size_t pinCount);										// метод обновления матрицы mBranchesOnPins
	bool_vect_t findBranchesWithCurrentSource(const elem_vect_t& elements) const;			// метод построения вектора индикации наличия в i-ой ветви источника тока 
};
