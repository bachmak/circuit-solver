#include "../coefficient.h"

// конструктор с параметрами
Coefficient::Coefficient(const double* pointer, int sign, double term) :                // список инициализации
	mPointer{ pointer }, mSign{ sign }, mTerm{ term }									// по умолчанию valueSign = 1, term = 0
{
}

// оператор для возврата значения
double Coefficient::operator()() const
{
	return *mPointer * mSign + mTerm;										        	// вычисляем произведение разыменованного указателя на знак (+-1)
}

// метод изменения хранимого указателя на значение
Coefficient& Coefficient::operator=(const double* pointer)
{
	mPointer = pointer;																	// присваиваем значение указателя
	return *this;																		// возвращаем ссылку на текущий объект
}

// метод изменения хранимого знака
void Coefficient::setSign(int sign)
{
	if (sign != 1 && sign != -1)														// фильтриуем все кроме единиц
	{
		return;																			// если не единица, то ничего не делаем
	}

	mSign = sign;																		// присваиваем знаку новое значение
}

// метод изменения дополнительного слагаемого
void Coefficient::setTerm(double term)
{
    mTerm = term;                                                                       // присваиваем слагаемому новое значение 
}

// оператор для добавления слагаемого
Coefficient Coefficient::operator+= (double term)
{
    mTerm += term;                                                                      // увеличиваем слагаемое
    return *this;                                                                       // возвращаем объект по значению
}

// оператор для вычитания слагаемого
Coefficient Coefficient::operator-= (double term)
{
    mTerm -= term;                                                                      // уменьшаем слагаемое
    return *this;                                                                       // возвращаем объект по значению
}

// оператор для сложения объекта Coefficient и числа
Coefficient Coefficient::operator+ (double term)
{
    Coefficient coefficient = *this;                                                    // создаем копию текущего объекта
    coefficient.mTerm += term;                                                          // увеличиваем слагаемое копии
    return coefficient;                                                                 // возвращаем копию по значению
}

// оператор для сложения объекта Coefficient и числа
Coefficient Coefficient::operator- (double term)
{
    Coefficient coefficient = *this;                                                    // создаем копию текущего объекта
    coefficient.mTerm -= term;                                                          // уменьшаем слагаемое копии
    return coefficient;                                                                 // возвращаем копию по значению
}