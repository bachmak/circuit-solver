#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/structure_units/element.h"

namespace CS
{
// класс для хранения массива элементов схемы
//////////////////////////////////////////////////////////////////////////////////////////
class Elements
{
public:
    void add(const Element& element);                                                   // метод для добавления нового элемента
    const Element& operator[] (size_t index) const;                                     // константный оператор обращения к элементу по индексу
    Element& operator[] (size_t index);                                                 // неконстантный оператор обращения к элементу по индексу
    size_t size() const;                                                                // геттер размера массива элементов
    size_t getCurrentSourceCount() const;                                               // геттер счетчика источников тока
    size_t getVoltageSourceCount() const;                                               // геттер счетчика источников напряжения
    size_t getResistanceCount() const;                                                  // геттер счетчика сопротивлений

private:
    ElemVect elements;                                                                  // вектор объектов типа Element
    size_t currentSourceCount = 0;                                                      // счетчик источников тока
    size_t voltageSourceCount = 0;                                                      // счетчик источников напряжения
    size_t resistanceCount = 0;                                                         // счетчик сопротивлений
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод добавления нового элемента в последовательность элементов
inline void Elements::add(const Element& element)
{
    elements.push_back(element);                                                        // добавляем новый элемент в конец вектора элементов 

    switch (element.getType())                                                          // в зависимости от типа элемента
    {
    case Element::Type::J:                                                              // инкрементируем соответствующий счетчик
        currentSourceCount++;                                                           //
        break;
    case Element::Type::E:                                                              //
        voltageSourceCount++;                                                           //
        break;
    case Element::Type::R:                                                              //
        resistanceCount++;                                                              //
    }
}

// константный оператор обращения к элементу по индексу
inline const Element& Elements::operator[](size_t index) const
{
    return elements[index];
}

// неконстантный оператор обращения к элементу по индексу
inline Element& Elements::operator[](size_t index)
{
    return elements[index];
}

// геттер размера массива элементов
inline size_t Elements::size() const
{
    return elements.size();
}

// геттер счетчика источников тока
inline size_t Elements::getCurrentSourceCount() const
{
    return currentSourceCount;
}

// геттер счетчика источников напряжения
inline size_t Elements::getVoltageSourceCount() const
{
    return voltageSourceCount;
}

// геттер счетчика сопротивлений
inline size_t Elements::getResistanceCount() const
{
    return resistanceCount;
}

} // namespace CS
