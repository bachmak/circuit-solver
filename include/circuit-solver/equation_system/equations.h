#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/nodes.h"
#include "circuit-solver/circuit_configuration/branches.h"
#include "circuit-solver/circuit_configuration/inc_matrix.h"
#include "circuit-solver/circuit_configuration/loops.h"

namespace CS
{
// класс для представления уравнений схемы по I и II законам Кирхгофа
////////////////////////////////////////////////////////////////////////////////////////// 
class Equations
{
public:
    void update(const IncMatrix& matrix, const Loops& loops,
                const Branches& branches, const Elements& elements);                    // метод обновления уравнений
    const CoeffMatr& left() const;                                                      // константный геттер левой части системы уравнений
    const CoeffMatr& right() const;                                                     // константный геттер правой части системы уравнений
    friend std::ostream& operator<<(std::ostream& os, const Equations& equations);      // перегрузка оператора вывода в поток

private:
    CoeffMatr leftPart;                                                                 // матрица коэффициентов левой части системы
    CoeffMatr rightPart;                                                                // матрица коэффициентов правой части системы
    size_t unknownCurrentCount = 0;                                                     // счетчик неизвестных токов
    size_t currentSourceCount = 0;                                                      // счетчие источников тока
    size_t voltageSourceCount = 0;                                                      // счетчик источников напряжения
    size_t knownSourceCount = 0;                                                        // счетчик источников энергии (тока и напряжения)
    size_t firstLawCount = 0;                                                           // счетчик уравнений, составленных по I закону Кирхгофа
    size_t secondLawCount = 0;                                                          // счетчик уравнений, составленных по II закону Кирхгофа

    void update1stLawEquations(const IncMatrix& matrix, const Elements& elements);      // метод обновления уравнений, составленных по I закону Кирхгофа
    void update2ndLawEquations(const Loops& loops, const Branches& branches,
                               const Elements& elements);                               // метод обновления уравнений, составленных по II закону Кирхгофа

    size_t getCurrentSourceElemIndex(const size_t currentSourceIndex,                   // метод вычисления индекса источника тока в списке элементов
                                     const Elements& elements) const;                   // по последовательному номеру этого источника
    size_t getVoltageSourceIndex(const size_t elemIndex,                                // метод вычисления последовательного индекса источника напряжения
                                 const Elements& elements) const;                       // по номеру этого источника в списке элементов
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления уравнений:
// выполняет построение уравнений путем заполения матриц leftPart и rightPart
// коэффициентами при неизвестных токах (leftPart) и известных источниках (rightPart)
// принимает на вход:
// 1) матрицу инцидентности для составления уравнений по I з. Кирхгофа
// 2) список контуров для составления уравнений по II з. Кирхгофа
// 3) список ветвей для составления уравнений по II з. Кирхгофа
// 4) список элементов для подстановки численных значений параметров элементов в уравнения
inline void Equations::update(const IncMatrix& matrix, const Loops& loops,
                              const Branches& branches, const Elements& elements)
{
    unknownCurrentCount = matrix.getUnknownCurrentCount();

    leftPart = CoeffMatr(unknownCurrentCount,                                           // создаем двумерную матрицу нулевых коэффициентов
        CoeffVect(unknownCurrentCount, Coefficient(ZERO_PTR)));                         // размером unknownCurrentNumber х unknownCurrentNumber

    currentSourceCount = elements.getCurrentSourceCount();                              // получаем количество источников тока 

    voltageSourceCount = elements.getVoltageSourceCount();                              // получаем количество источников напряжения 

    knownSourceCount = currentSourceCount + voltageSourceCount;                         // считаем все источники

    rightPart = CoeffMatr(matrix.getUnknownCurrentCount(),                              // создаем матрицу нулевых коэффициентов 
        CoeffVect(knownSourceCount, Coefficient(ZERO_PTR)));                            // размера unknownCurrentCount x knownSourceCount
                                                                                        // записываем в переменные количество уравнений по I и II з. Кирхгофа:
    firstLawCount = matrix.size() ? matrix[0].size() - 1 : 0;                           // I закон Кирхгофа - на единицу меньше количества узлов
    secondLawCount = loops.size();                                                      // II закон Кирхгофа - по количеству контуров

    update1stLawEquations(matrix, elements);                                            // обновляем коэффициенты уравнений по I з. Кирхгофа
    update2ndLawEquations(loops, branches, elements);                                   // обновляем коэффициенты уравнений по II з. Кирхгофа
}

// константный геттер левой части системы уравнений
inline const CoeffMatr& Equations::left() const
{
    return leftPart;
}

// константный геттер правой части системы уравнений
inline const CoeffMatr& Equations::right() const
{
    return rightPart;
}

// перегрузка оператора вывода в поток
inline std::ostream& operator<<(std::ostream& os, const Equations& equations)
{
    auto precision = os.precision();
    using namespace std;

    os << "Система уравнений:\n\n" << setw(9) << ' ';

    os << left << fixed << setprecision(precision);

    for (size_t i = 0; i < equations.unknownCurrentCount; i++)
    {
        os << "I" << setw(precision + 9) << i << ' ';
    }
    os << setw(precision + 10) << ' ';

    for (size_t i = 0; i < equations.currentSourceCount; i++)
    {
        os << "J" << setw(precision + 9) << i << ' ';
    }

    for (size_t i = 0; i < equations.voltageSourceCount; i++)
    {
        os << "E" << setw(precision + 9) << i << ' ';
    }

    os << "\n\n";

    for (size_t i = 0; i < equations.unknownCurrentCount; i++)
    {
        os << setw(4) << i << ":    ";

        for (size_t j = 0; j < equations.unknownCurrentCount; j++)
        {
            os << setw(precision + 10) << equations.leftPart[i][j]() << ' ';
        }

        os << setw(precision + 10) << ' ';

        for (size_t j = 0; j < equations.knownSourceCount; j++)
        {
            os << setw(precision + 10) << equations.leftPart[i][j]() << ' ';
        }

        os << "\n\n";
    }

    return os;
}

// метод обновления уравнений, составленных по I закону Кирхгофа
inline void Equations::update1stLawEquations(const IncMatrix& matrix,
                                             const Elements& elements)
{
    for (size_t i = 0; i < matrix[0].size() - 1; i++)                                   // перебираем столбцы матрицы инцидентности. сколько столбцов - столько узлов -
    {                                                                                   // столько уравнений (за исключением одного линейно зависимого столбца)
        for (size_t j = 0; j < unknownCurrentCount; j++)                                // перебираем строки, соответствующие неизвестным токам (здесь нет значений,
        {                                                                               // связанных с параметрами элементов - только +-1 или 0 - зависит от геометрии схемы)
            switch (matrix[j][i])                                                       // смотрим, как соотносится j-ый ток и i-ый узел 
            {
            case -1:                                                                    // если ток вытекает из узла
                leftPart[i][j] = ONE_PTR;                                               // присваиваем указатель на единицу
                leftPart[i][j].setSign(-1);                                             // знак отрицательный
                break;                                                                  // выходим из switch

            case 1:                                                                     // если ток втекает в узел
                leftPart[i][j] = ONE_PTR;                                               // присваиваем указатель на единицу (знак по умолчанию положительный)
                break;                                                                  // выходим из switch

            default:                                                                    // если j-ый ток не связан с i-ым узлом (j-ая ветвь не инцидентна i-ому узлу)
                leftPart[i][j] = ZERO_PTR;                                              // присваиваем указатель на ноль
            }
        }

        for (size_t j = 0; j < currentSourceCount; j++)                                 // перебираем строки, соответствующие ветвям с источниками тока
        {
            if (matrix[j + unknownCurrentCount][i])                                     // если j-ая ветвь с источником тока связана с i-ым узлом 
            {
                size_t index = getCurrentSourceElemIndex(j, elements);                  // вычисляем индекс j-го источника тока в списке элементов схемы
                const double* pointer = elements[index].getValuePointer();              // по индексу в списке элементов получаем адрес значения тока источника тока
                int sign = -matrix[j + unknownCurrentCount][i];                         // определяем знак тока от j-го источника тока в i-ом узле (инверсия, так как 
                                                                                        // knownSourceMatrix - правая часть системы уравнений)
                rightPart[i][j] = pointer;                                              // присваиваем полученный адрес на значение
                rightPart[i][j].setSign(sign);                                          // присваиваем знак
            }
            else                                                                        // если j-ая ветвь с источником тока не связана с i-ым узлом 
            {
                rightPart[i][j] = ZERO_PTR;                                             // присваиваем указатель на ноль
            }
        }

        for (size_t j = 0; j < voltageSourceCount; j++)                                 // присваиваем указатели на нулевые значения в оставшиеся позиции, соответствующие
        {                                                                               // источникам напряжения, т.к. они не участвуют в формировании уравнений по первому
            rightPart[i][j + currentSourceCount] = ZERO_PTR;                            // закону Кирхгофа
        }
    }
}

// метод обновления уравнений, составленных по II закону Кирхгофа
inline void Equations::update2ndLawEquations(const Loops& loops,
                                             const Branches& branches,
                                             const Elements& elements)
{
    for (size_t i = 0; i < loops.size(); i++)                                           // перебор по контурам схемы
    {
        for (size_t j = 0; j < loops[i].size(); j++)                                    // перебор по ветвям контура
        {
            size_t branch = abs(loops[i][j]) - (int)1;                                  // приводим номер ветви к исходному виду (без знака и инкремента)
            int branchSign = loops[i][j] > 0 ? 1 : -1;                                  // определяем знак тока в ветви branch 

            for (size_t k = 0; k < branches[branch].size(); k += 2)                     // перебираем ветвь по элементам схемы
            {
                size_t elemIndex = branches[branch][k] / 2;                             // вычисляем индекс элемента по номеру пина k в ветви branch
                const double* pointer = elements[elemIndex].getValuePointer();          // получаем адрес значения элемента

                if (elements[elemIndex].getType() == Element::Type::R)                  // если элемент является сопротивлением
                {
                    leftPart[firstLawCount + i][branch] = pointer;                      // инициализируем коэффициент, соответствующий i-му уравнению 
                                                                                        // по второму закону Кирхгофа и току branch, адресом pointer 
                    leftPart[firstLawCount + i][branch].setSign(branchSign);            // задаем соответствующий знак тока
                }
                else                                                                    // иначе - источник напряжения
                {
                    size_t voltageSourceIndex =                                         // вычисляем порядковый номер источника напряжения в системе уравнений
                        getVoltageSourceIndex(elemIndex, elements);                     // по порядковому номеру элемента в векторе элементов

                    bool pinIsOdd = branches[branch][k] % 2;                            // определяем, является ли пин k ветви branch нечетным (плюсом источника)
                    int valueSign = 1;                                                  // переменная для знака вхождения напряжения в уравнение

                    if (pinIsOdd)                                                       // если рассматриваемый пин нечетный (по допущению, четному (меньшему) id пина 
                    {                                                                   // соответсвует отрицательный полюс источника, нечетному - положительный)
                        valueSign = -1;                                                 // пины ветви branch рассматриваются через один, то есть если рассматриваемый 
                    }                                                                   // пин нечетный - идем против направления ветви - знак напряжения отрицательный

                    size_t index = currentSourceCount + voltageSourceIndex;             // вычисляем порядковый номер коэффициента источника напряжения в уравнении
                    rightPart[firstLawCount + i][index] = pointer;                      // инициализируем коэффициент, соответствующий i-му уравнению
                                                                                        // по второму закону Кирхгофа и напряжению index, адресом pointer
                    rightPart[firstLawCount + i][index].setSign(branchSign*valueSign);  // задаем соответствующий знак c учетом направления ветви в контуре и 
                }                                                                       // ориентации источника напряжения
            }
        }
    }
}

// метод вычисления индекса источника тока в списке элементов
// по последовательному номеру этого источника
inline size_t Equations::getVoltageSourceIndex(const size_t elemIndex,
                                               const Elements& elements) const
{
    size_t counter = 0;                                                                 // переменная для подсчета источников напряжения

    for (size_t i = 0; i < elemIndex; i++)                                              // перебор элементов схемы до элемента с номером elemIndex 
    {
        if (elements[i].getType() == Element::Type::E)                                  // если нашли источник напряжения
        {
            counter++;                                                                  // инкрементируем счетчик
        }
    }
    return counter;                                                                     // возвращаем значение счетчика
}

inline size_t Equations::getCurrentSourceElemIndex(const size_t currentSourceIndex,
                                                   const Elements& elements) const
{
    size_t counter = 0;                                                                 // переменная для подсчета источников тока

    for (size_t i = 0; i < elements.size(); i++)                                        // перебираем элементы схемы
    {
        if (elements[i].getType() == Element::Type::J)                                  // если элемент является источником тока
        {
            if (++counter == currentSourceIndex + 1)                                    // инкрементируем счетчик источников и сравниваем с искомым значением
            {
                return i;                                                               // если равны, возвращаем номер элемента
            }
        }
    }
    return elements.size();                                                             // если совпадений не нашлось, возвращаем размер вектора элементов
}

} // namespace CS
