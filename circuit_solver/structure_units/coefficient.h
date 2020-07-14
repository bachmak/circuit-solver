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
class Coefficient
{
public:
	Coefficient(const double* valuePointer, int valueSign = 1, double term = 0);        // конструктор с параметрами (по умолчанию знак положительный)
	double operator ()() const;															// перегруженный оператор скобок для вычисления значения
	Coefficient& operator= (const double* valuePointer);								// метод изменения хранимого указателя на значение
	void setSign(int valueSign = 1);													// метод изменения хранимого знака
    void setTerm(double term = 0);                                                      // метод изменения дополнительного слагаемого
	Coefficient operator+= (double term);                                               // перегруженные операторы для работы с дополнительным слагаемым
    Coefficient operator-= (double term);                                               //
    Coefficient operator+ (double term);                                                //
    Coefficient operator- (double term);                                                //

private:
	const double* mPointer = nullptr;													// указатель на значение
    double mTerm = 0;                                                                   // дополнительное слагаемое
	int mSign = 1;																		// знак (+-1)
};