#include "../coefficient.h"

// оператор для возврата значения
double Coefficient::operator()() const
{
	return *mPointer * mSign;													// вычисляем произведение разыменованного указателя на знак (+-1)
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

	mSign = sign;																		// иначе переинициализируем знак
}

// конструктор с параметрами
Coefficient::Coefficient(const double* pointer, int sign) :								// список инициализации
	mPointer{ pointer }, mSign{ sign }													// по умолчанию valueSign = 1
{
}
