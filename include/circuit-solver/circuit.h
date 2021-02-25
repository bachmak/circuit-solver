#pragma once

#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/pin_matrix.h"
#include "circuit-solver/circuit_configuration/nodes.h"
#include "circuit-solver/circuit_configuration/branches.h"
#include "circuit-solver/circuit_configuration/inc_matrix.h"
#include "circuit-solver/circuit_configuration/loops.h"
#include "circuit-solver/equation_system/equations.h"

namespace CS
{
// класс для представления электрической схемы
//////////////////////////////////////////////////////////////////////////////////////////
class Circuit
{
private:                                                                                // префикс m указывает на то, что переменная является членом класса
    Elements elements;                                                                  // объект для списка элементов
    PinMatrix pinMatrix;                                                                // объект для матрицы соединений пинов
    Nodes nodes;                                                                        // объект для узлов и вспомогательных структур
    Branches branches;                                                                  // объект для ветвей и вспомогательных структур
    IncMatrix incMatrix;                                                                // объект для матрицы инцидентности
    Loops loops;                                                                        // объект для контуров
    Equations equations;                                                                // объект для системы уравнений

    size_t currentSourceCount = 0;                                                      // счетчик источников тока
    size_t voltageSourceCount = 0;                                                      // счетчик источников напряжения

public:
    void add(const Element& element);                                                   // метод добавления в схему нового элемента
    void update();                                                                      // метод обновления схемы
    void setElementValue(size_t index, double value);                                   // метод изменения значения элемента
    std::string toString(size_t precision = 3) const;                                   // метод получения строкового представления схемы с точностью значений precision
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод добавления нового элемента в схему (перегрузка)
inline void Circuit::add(const Element& element)
{
    elements.add(element);                                                              // добавляем элемент в список элементов

    switch (element.getType())                                                          // в зависимости от типа элемента
    {
    case Element::Type::J:                                                              // инкрементируем соответствующий счетчик
        currentSourceCount++;
        break;
    case Element::Type::E:                                                              // 
        voltageSourceCount++;
    }
}

// метод изменения значения элемента с индексом index в списке элементов схемы
inline void Circuit::setElementValue(size_t index, double value)
{
    if (index >= elements.size())                                                       // если индекс за пределами списка
    {
        return;                                                                         // игнорируем
    }
    elements[index].setValue(value);                                                    // изменяем значение элемента
}

// метод получения строкового представления схемы
inline std::string Circuit::toString(size_t precision) const
{
    return pinMatrix.toString() +
        nodes.toString() +
        branches.toString() +
        incMatrix.toString() +
        loops.toString() +
        equations.toString(precision);
}

// метод обновления схемы
inline void Circuit::update()
{
    pinMatrix.update(elements);                                                         // обновляем матрицу пинов
    nodes.update(pinMatrix);                                                            // обновляем узлы и все, что с ними связано
    branches.update(pinMatrix, nodes, elements);                                        // обновляем ветви и все, что с ними связано
    incMatrix.update(nodes, branches, elements);                                        // обновляем матрицу инцидентности
    loops.update(incMatrix);                                                            // обновляем узлы
    equations.update(incMatrix, loops, branches, elements);                             // обновляем систему уравнений
}

} // namespace CS
