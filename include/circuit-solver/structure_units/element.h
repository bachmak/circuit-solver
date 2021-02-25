#pragma once

namespace CS
{
// класс для представления элемента схемы
// каждый элемент характеризуется:
// 1) типом type (резистивный элемент, источник напряжения, источник тока)
// 2) значением электрического параметра (сопротивление, напряжение, ток)
// 3) двумя номерами пинов, с которыми соединены пины этого элемента
// замечание: собственные номера пинов в объекте не хранятся, так как предполагается,
// что элементы будут храниться в последовательном списке, порядок хранения в котором
// совпадает с порядком создания. поэтому номер первого пина элемента можно вычислить
// путем умножения номера элемента в списке на 2
class Element
{
public:
    enum class Type
    {
        R, E, J
    };

private:
	Type type;																			// тип
	double value;																		// значение
	int linkedPin[2];																	// номера соседних пинов

public:
	Element(Type type, double value, int linkedPin0, int linkedPin1);					// конструктор
	Type getType() const;																// геттер типа
	double getValue() const;															// геттер значения
	void setValue(double value);														// сеттер значения
	const double* getValuePointer() const;												// геттер указателя на значение
	int getLinkedPin(size_t number) const;												// геттер номера присоединенного пина
};

// конструктор
inline Element::Element(Type type, double value, int linkedPin0, int linkedPin1) :
	type{ type }, value{ value }, linkedPin{ linkedPin0, linkedPin1 } {}

// геттер типа
inline Element::Type Element::getType() const
{
	return type;
}

// геттер значения
inline double Element::getValue() const
{
	return value;
}

// сеттер значения
inline void Element::setValue(double value)
{
	this->value = value;
}

// геттер указателя на значение
inline const double* Element::getValuePointer() const
{
	return &value;																		// возвращаем адрес значения
}

// геттер номера присоединенного пина
inline int Element::getLinkedPin(size_t number) const
{
	return linkedPin[number % 2];														// фильтр всего, кроме 0 и 1
}

} // namespace CS
