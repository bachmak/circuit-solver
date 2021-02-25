#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/structure_units/element.h"

namespace CS
{
// класс для хранения массива элементов схемы
class Elements
{
public:
	void add(const Element& element);
	const Element& operator[] (size_t index) const;
    Element& operator[] (size_t index);
	size_t size() const;
    size_t getCurrentSourceCount() const;
    size_t getVoltageSourceCount() const;
    size_t getResistanceCount() const;

private:
	ElemVect elements;
	size_t currentSourceCount = 0;
	size_t voltageSourceCount = 0;
    size_t resistanceCount = 0;
};

inline void Elements::add(const Element& element)
{
	elements.push_back(element);

	switch (element.getType())
	{
	case Element::Type::J:
		currentSourceCount++;
		break;
	case Element::Type::E:
		voltageSourceCount++;
        break;
    case Element::Type::R:
        resistanceCount++;
	}
}

inline const Element& Elements::operator[](size_t index) const
{
	return elements[index];
}

inline Element& Elements::operator[](size_t index)
{
    return const_cast<Element&>(
        static_cast<const Elements&>(*this)[index]
    );
}

inline size_t Elements::size() const
{
	return elements.size();
}

inline size_t Elements::getCurrentSourceCount() const
{
    return currentSourceCount;
}

inline size_t Elements::getVoltageSourceCount() const
{
    return voltageSourceCount;
}

inline size_t Elements::getResistanceCount() const
{
    return resistanceCount;
}

} // namespace CS
