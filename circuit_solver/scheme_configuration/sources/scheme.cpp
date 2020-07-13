#include "../scheme.h"

// метод добавления нового элемента в схему (перегрузка)
void Scheme::add(const Element& element)
{
	mElements.push_back(element);															// добавляем элемент в вектор элементов

	switch (element.getType())																// в зависимости от типа элемента
	{
	case Type::J:																			// инкрементируем соответствующий счетчик
		mCurrentSourceCount++;
		break;
	case Type::E:																			// 
		mVoltageSourceCount++;
	}
}

// метод изменения значения элемента с индексом index в списке элементов схемы
void Scheme::setElementValue(size_t index, double value)
{
	if (index >= mElements.size())															// если индекс за пределами списка
	{
		return;																				// игнорируем
	}
	mElements[index].setValue(value);														// изменяем значение элемента
}

// метод получения строкового представления схемы
std::string Scheme::toString(size_t precision) const
{
	return mPinMatrix.toString() +
		mNodes.toString() +
		mBranches.toString() +
		mIncMatrix.toString() +
		mLoops.toString() +
		mEquations.toString(precision);
}

// метод обновления схемы
void Scheme::update()
{
	mPinMatrix.update(mElements);															// обновляем матрицу пинов
	mNodes.update(mPinMatrix);																// обновляем узлы и все, что с ними связано
	mBranches.update(mPinMatrix, mNodes, mElements);										// обновляем ветви и все, что с ними связано
	mIncMatrix.update(mNodes, mBranches, mElements);										// обновляем матрицу инцидентности
	mLoops.update(mIncMatrix);																// обновляем узлы
	mEquations.update(mIncMatrix, mLoops, mBranches, mElements);							// обновляем систему уравнений
}
