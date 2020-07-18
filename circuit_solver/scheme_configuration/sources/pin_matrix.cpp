#include "../pin_matrix.h"

void PinMatrix::update(const Elements& elements)
{
	size_t pinCount = elements.size() * 2;												// считаем пины по количеству элементов

	mMatrix = bool_matr_t(pinCount);													// создаем двумерный вектор размера pinCount
	for (size_t i = 0; i < pinCount; i++)												// создаем треугольную матрицу нижу главной диагонали, заполненную нулями
	{
		mMatrix[i] = bool_vect_t(i, false);
	}
																						// первая часть: соединяем то, что соединено напрямую
	for (size_t i = 0; i < pinCount; i++)												// перебираем по пинам схемы (их ровно в 2 раза больше, чем элементов)
	{	
		if (elements[i / 2].getLinkedPin(i % 2) >= 0)									// если i-ый пин соединен с каким-нибудь другим пином (-1, если не соединен)
		{																				// целая часть от деления на два - номер элемента, остаток - номер пина элемента
			mMatrix[i][elements[i / 2].getLinkedPin(i % 2)] = true;						// симметрично отмечаем соединение пинов в матрице соединений 
		}
	}
																						// вторая часть: соединяем то, что соединено через другие пины
	for (size_t i = 0; i < pinCount; i++)												// перебираем матрицу пинов по строкам
	{
		for (size_t j = 0; j < i; j++)													// перебираем матрицу пинов по столбцам (смотрим ниже главной диагонали)
		{
			if (mMatrix[i][j])															// если i-ый пин соединен с j-ым
			{
				for (size_t k = 0; k < j; k++)											// перебираем до главной диагонали строку j, пин из которой соединен с i-ым  
				{																		// ищем пины, которые через j-ый соединены с i-ым
					if (mMatrix[j][k])													// если j-ый пин соединен с k-ым
					{
						mMatrix[i][k] = true;											// то i-ый пин так же соединен с k-ым
					}
				}
				for (size_t k = i + 1; k < pinCount; k++)								// перебираем от главноц диагонали столбец j, пин из которой соединен с i-ым
				{																		// ищем пины, которые через j-ый соединены с i-ым
					if (mMatrix[k][j])													// если k-ый пин соединен с j-ым
					{
						mMatrix[k][i] = true;											// то k-ый пин так же соединен с i-ым
					}																	// т.е. дополняем отношение до транзитивного, каким оно и должно быть
				}
			}
		}
	}
}

const size_t PinMatrix::size() const
{
	return mMatrix.size();
}

const bool_vect_t& PinMatrix::operator[](size_t index) const
{
	return mMatrix[index];
}

std::string PinMatrix::toString() const
{
	using namespace std;

	stringstream stream;

	stream << "Матрица соединений пинов:\n\n";

	stream << setw(6) << ' ';

	for (size_t i = 0; i < mMatrix.size(); i++)
	{
		stream << setw(4) << i << ' ';
	}

	stream << "\n\n";

	for (size_t i = 0; i < mMatrix.size(); i++)
	{
		stream << left << setw(4) << i << right << ": ";

		for (size_t j = 0; j < mMatrix[i].size(); j++)
		{
			stream << setw(4) << mMatrix[i][j] << ' ';
		}

		stream << "\n\n";
	}

	return stream.str();
}
