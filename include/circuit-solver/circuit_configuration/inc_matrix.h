#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/nodes.h"
#include "circuit-solver/circuit_configuration/branches.h"

namespace CS
{
// класс для представления матрицы инцидентности схемы
//////////////////////////////////////////////////////////////////////////////////////////
class IncMatrix
{
public:
    void update(const Nodes& nodes, const Branches& branches,
                const Elements& elements);                                              // метод обновления матрицы
    size_t size() const;                                                                // геттер размера матрицы
    size_t getUnknownCurrentCount() const;                                              // геттер счетчика неизвестных токов
    const IntVect& operator[] (size_t index) const;                                     // геттер строки матрицы (одномерного вектора)
    std::string toString() const;                                                       // метод получения строкового представления матрицы

private:
    IntMatr matrix;                                                                     // двумерная матрица для хранения информации об инцидентности ветвей узлам
    size_t unknownCurrentCount = 0;                                                     // счетчик неизвестных токов

    void orient(const Nodes& nodes, const Branches& branches,
                const Elements& elements);                                              // метод выбора направления токов (ориентирования матрицы)
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления матрицы-отношения между множествами ветвей и узлов такой, что:
// если matrix[i][j] равно -1, ток в i-ой ветви вытекает из j-го узла;
// если matrix[i][j] равно 1, ток в i-ой ветви втекает в j-ый узел;
// принимает на вход:
// 1) объект nodes для информации об узлах схемы
// 2) объект branches для информации о ветвях схемы
// 3) последовательность элементов для информации об источниках тока
inline void IncMatrix::update(const Nodes& nodes, const Branches& branches,
                              const Elements& elements)
{
    matrix = IntMatr(branches.size(), IntVect(nodes.size(), 0));                        // создаем нулевую матрицу размера branchesSize x nodesSize, заполненную нулями

    const BoolMatr& branchesOnPins = branches.getBranchesOnPins();                      // сохраняем матрицы-отношения в отдельные переменные для краткости
    const BoolMatr& nodesOnPins = nodes.getNodesOnPins();                               //

    for (size_t i = 0; i < branchesOnPins.size(); i++)                                  // перебираем строки первого отношения (ветви)
    {
        for (size_t j = 0; j < nodesOnPins.size(); j++)                                 // перебираем строки второго отношения (узлы)
        {
            for (size_t k = 0; k < nodesOnPins[j].size(); k++)                          // перебираем столбцы отношений (пины)
            {
                matrix[i][j] |= 
                    nodesOnPins[j][k] && branchesOnPins[i][k];                          // дизъюнкция всех конъюнкций элемента из первого отношения с элементом из второго
            }                                                                           // (если у узла j найдется хоть один общий пин с ветвью i, то соединяем их)
        }
    }

    orient(nodes, branches, elements);                                                  // ориентируем матрицу инцидентности
}

// геттер размера матрицы
size_t IncMatrix::size() const
{
    return matrix.size();
}

// геттер строки матрицы (одномерного вектора)
const IntVect& IncMatrix::operator[](size_t index) const
{
    return matrix[index];
}

// метод получения строкового представления матрицы
inline std::string IncMatrix::toString() const
{
    using namespace std;

    stringstream stream;

    stream << "Матрица инцидентности:\n\n";

    if (!matrix.empty())
    {
        stream << setw(6) << ' ';

        for (size_t i = 0; i < matrix[0].size(); i++)
        {
            stream << setw(4) << i << ' ';
        }

        stream << "\n\n";

        for (size_t i = 0; i < matrix.size(); i++)
        {
            stream << left << setw(4) << i << right << ": ";

            for (size_t j = 0; j < matrix[i].size(); j++)
            {
                stream << setw(4) << matrix[i][j] << ' ';
            }

            stream << "\n\n";
        }
    }

    return stream.str();
}

// геттер счетчика неизвестных токов
inline size_t IncMatrix::getUnknownCurrentCount() const
{
    return unknownCurrentCount;
}

// метод выбора направления токов (ориентирования матрицы):
// - для ветвей с неизвестными токами направление тока выбирается произвольно (от меньшего узла)
// - для ветвей с источниками тока направление тока выбирается в соответствии с полярностью источника
inline void IncMatrix::orient(const Nodes& nodes, const Branches& branches,
                              const Elements& elements)
{
    unknownCurrentCount = branches.size() - branches.getCurrentSourceCount();           // сохраняем количество неизвестных токов

    for (size_t i = 0; i < unknownCurrentCount; i++)                                    // перебор по ветвям с неизвестными токами (строки в матрице сортированы)
    {
        size_t nodeNumber = nodes.getNodesForPin().at(branches[i][0]);                  // промежуточная переменная - номер узла, с которым соединен первый пин i-ой ветви
        matrix[i][nodeNumber] = -1;                                                     // инициализируем -1 позицию, соответствующую соединению i-ой ветви с узлом nodeNumber 
    }                                                                                   // т.е. направляем ток от узла с меньшим номером к узлу с большим

    for (size_t i = unknownCurrentCount; i < branches.size(); i++)                      // перебор по ветвям с источниками тока
    {
        for (size_t j = 0; ; j += 2)                                                    // перебираем пины ветви - ищем источник тока
        {
            if (elements[branches[i][j] / 2].getType() == Element::Type::J)             // если элемент является источником тока
            {
                if (branches[i][j] % 2)                                                 // если рассматривается нечетный (положительный) вывод источника 
                {
                    matrix[i][nodes.getNodesForPin().at(branches[i].back())] = -1;      // инициализируем последний элемент ветви значением -1 (ток от конца ветви) 
                }
                else                                                                    // если рассматривается четный (отрицательный) вывод источника
                {
                    matrix[i][nodes.getNodesForPin().at(branches[i][0])] = -1;          // инициализируем первый элемент ветви значением -1 (ток от начала ветви)
                }
                break;                                                                  // выходим из цикла перебора по элементам
            }
        }
    }
}

} // namespace CS
