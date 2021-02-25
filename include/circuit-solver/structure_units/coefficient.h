#pragma once

// класс для представления коэффициентов системы уравнений
// функтор (функциональный объект) вычисляет произведение значения, получаемого по
// указателю valuePointer, и знака valueSign
// введение таких объектов необходимо для того, чтобы при изменении значений
// параметров схемы коэффициенты системы уравнений изменялись бы напрямую через
// указатель. при этом одно и то же значение параметра может быть задействовано 
// в нескольких уравнениях схемы, при этом входить в них с разным знаком
// что не отражается на самом значении параметра, а учитывается лишь при вычислении 
// соответствующего коэффициента coefficient[i][j]()

namespace CS
{
    
class Coefficient
{
public:
	Coefficient(const double* valuePointer, int valueSign = 1, double term = 0);        // конструктор с параметрами (по умолчанию знак положительный)
	double operator()() const;															// перегруженный оператор скобок для вычисления значения
	Coefficient& operator=(const double* valuePointer);								    // метод изменения хранимого указателя на значение
	void setSign(int valueSign = 1);													// метод изменения хранимого знака
    void setTerm(double term = 0);                                                      // метод изменения дополнительного слагаемого
	Coefficient operator+=(double term);                                                // перегруженные операторы для работы с дополнительным слагаемым
    Coefficient operator-=(double term);                                                //
    Coefficient operator+(double term);                                                 //
    Coefficient operator-(double term);                                                 //

private:
	const double* pointer = nullptr;   													// указатель на значение
    double term = 0;                                                                    // дополнительное слагаемое
	int sign = 1;																		// знак (+-1)
};

// конструктор с параметрами
inline Coefficient::Coefficient(const double* pointer, int sign, double term) :         // список инициализации
	pointer{ pointer }, sign{ sign }, term{ term } {}   								// по умолчанию valueSign = 1, term = 0

// оператор для возврата значения
inline double Coefficient::operator()() const
{
	return *pointer * sign + term;  										        	// вычисляем произведение разыменованного указателя на знак (+-1)
}

// метод изменения хранимого указателя на значение
inline Coefficient& Coefficient::operator=(const double* pointer)
{
	this->pointer = pointer;															// присваиваем значение указателя
	return *this;																		// возвращаем ссылку на текущий объект
}

// метод изменения хранимого знака
inline void Coefficient::setSign(int sign)
{
	if (sign != 1 && sign != -1)														// фильтриуем все кроме единиц
	{
		return;																			// если не единица, то ничего не делаем
	}

	this->sign = sign;																	// присваиваем знаку новое значение
}

// метод изменения дополнительного слагаемого
inline void Coefficient::setTerm(double term)
{
    this->term = term;                                                                  // присваиваем слагаемому новое значение 
}

// оператор для добавления слагаемого
inline Coefficient Coefficient::operator+=(double term)
{
    this->term += term;                                                                 // увеличиваем слагаемое
    return *this;                                                                       // возвращаем объект по значению
}

// оператор для вычитания слагаемого
inline Coefficient Coefficient::operator-=(double term)
{
    this->term -= term;                                                                 // уменьшаем слагаемое
    return *this;                                                                       // возвращаем объект по значению
}

// оператор для сложения объекта Coefficient и числа
inline Coefficient Coefficient::operator+(double term)
{
    Coefficient coefficient = *this;                                                    // создаем копию текущего объекта
    coefficient.term += term;                                                           // увеличиваем слагаемое копии
    return coefficient;                                                                 // возвращаем копию по значению
}

// оператор для сложения объекта Coefficient и числа
inline Coefficient Coefficient::operator-(double term)
{
    Coefficient coefficient = *this;                                                    // создаем копию текущего объекта
    coefficient.term -= term;                                                           // уменьшаем слагаемое копии
    return coefficient;                                                                 // возвращаем копию по значению
}

} // namespace CS
