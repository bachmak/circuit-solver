#pragma once

#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/pin_matrix.h"
#include "circuit-solver/circuit_configuration/nodes.h"
#include "circuit-solver/circuit_configuration/branches.h"
#include "circuit-solver/circuit_configuration/inc_matrix.h"
#include "circuit-solver/circuit_configuration/loops.h"
#include "circuit-solver/equation_system/equations.h"

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <stdexcept>

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
    friend std::ostream& operator<<(std::ostream& os, const Circuit& circuit);          // перегрузка оператора вывода в поток
    friend std::istream& operator>>(std::ostream& is, Circuit& circuit);                // перегрузка оператора ввода из потока
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

// перегрузка оператора вывода в поток
inline std::ostream& operator<<(std::ostream& os, const Circuit& circuit)
{
    return os << circuit.pinMatrix << circuit.nodes 
              << circuit.branches << circuit.incMatrix
              << circuit.loops << circuit.equations;
}

// перегрузка оператора ввода из потока
inline std::istream& operator>>(std::istream& is, Circuit& circuit)
{
    std::string line;

    for (int lineCount = 1; std::getline(is, line); lineCount++)
    {
        boost::algorithm::trim(line);
        boost::algorithm::to_lower(line);

        if (line.empty() || line.front() == '#')
        {
            continue;
        }

        std::istringstream iss(line);

        Element::Type type;

        std::string typeString; 
        if (!(iss >> typeString))
        {
            throw std::runtime_error("Parsing error at line: " + std::to_string(lineCount));
        }

        if (typeString == "resistance" || typeString == "r")
        {
            type = Element::Type::R;
        }
        else if (typeString == "current_source" || typeString == "j")
        {
            type = Element::Type::J;
        }
        else if (typeString == "voltage_source" || typeString == "e")
        {
            type = Element::Type::E;
        }
        else
        {
            throw std::runtime_error("Unknown element type: " + typeString);
        }

        double value;
        
        if (!(iss >> value))
        {
            throw std::runtime_error("Parsing error at line: " + std::to_string(lineCount));
        }

        int linkedPins[2];

        std::string linkedPinString[2];

        for (size_t i = 0; i < 2; i++)
        {
            if (!(iss >> linkedPinString[i]))
            {
                throw std::runtime_error("Parsing error at line: " + std::to_string(lineCount));
            }

            if (linkedPinString[i] == "none")
            {
                linkedPins[i] = -1;
            }
            else
            {
                try
                {
                    linkedPins[i] = std::stoi(linkedPinString[i]);
                }
                catch(const std::exception& e)
                {
                    throw std::runtime_error("Parsing error at line: " + std::to_string(lineCount) + "\n" + e.what());
                }
            }
        }
        
        circuit.add(Element(type, value, linkedPins[0], linkedPins[1]));
    }
    
    return is;
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
