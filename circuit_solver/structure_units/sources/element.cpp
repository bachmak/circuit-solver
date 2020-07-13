#include "../element.h"

// конструктор
Element::Element(Type type, double value, int linkedPin0, int linkedPin1) :
	mType { type },
	mValue { value },
	mLinkedPin { linkedPin0, linkedPin1 }
{
}

// геттер типа
Type Element::getType() const
{
	return mType;
}

// геттер значения
double Element::getValue() const
{
	return mValue;
}

// сеттер значения
void Element::setValue(double value)
{
	mValue = value;
}

// геттер указателя на значение
const double* Element::getValuePointer() const
{
	return &mValue;																			// возвращаем адрес значения
}

// геттер номера присоединенного пина
int Element::getLinkedPin(size_t number) const
{
	return mLinkedPin[number % 2];															// фильтр всего, кроме 0 и 1
}