#pragma once

#include <map>

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/pin_matrix.h"
#include "circuit-solver/circuit_configuration/nodes.h"

namespace CS
{
// класс для представления ветвей схемы
//////////////////////////////////////////////////////////////////////////////////////////
class Branches
{
public:
    void update(const PinMatrix& matrix, const Nodes& nodes,
                const Elements& elements);                                              // метод обновления всех структур
    const SizeMatr& get() const;                                                        // геттер для списка ветвей branches
    const SizeVect& operator[] (size_t index) const;                                    // геттер строки списка ветвей branches (одномерного вектора)
    const BoolMatr& getBranchesOnPins() const;                                          // геттер матрицы branchesOnPins
    size_t size() const;                                                                // геттер размера списка ветвей branches
    size_t getCurrentSourceCount() const;                                               // геттер счетчика источников тока
    std::string toString() const;                                                       // метод получения строкового представления списка ветвей branches

private:
    SizeMatr branches;                                                                  // вектор векторов номеров пинов, представляющий список ветвей схемы
    BoolMatr branchesOnPins;                                                            // матрица-отношение между множеством ветвей и пинов
    size_t currentSourceCount = 0;                                                      // счетчик источников тока

    void sortBranches(BoolVect branchesWithCurrentSource);                              // метод сортировки ветвей в списке mBranches по наличию источника тока
    void updateBranchesOnPins(const size_t pinCount);                                   // метод обновления матрицы mBranchesOnPins
    BoolVect findBranchesWithCurrentSource(const Elements& elements) const;             // метод построения вектора индикации наличия в i-ой ветви источника тока 
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления вектора ветвей
// каждая ветвь представляется вектором номеров пинов, входящих в нее
// принимает на вход: 
// 1) матрицу соединений пинов, по которой осуществляется поиск последовательностей пинов
// 2) объект nodes для информации об узлах схемы
// 4) вектор элементов схемы для определения принадлежности пина источнику тока 
inline void Branches::update(const PinMatrix& matrix, const Nodes& nodes,
                             const Elements& elements)
{
    branches.clear();                                                                   // очищаем текущий вектор ветвей
    BoolVect pinsInBranches(matrix.size(), false);                                      // нулевой вектор для индикации выводов, участвующих в формировании ветвей

    for (size_t i = 0; i < nodes.size() - 1; i++)                                       // перебор по узлам схемы (- 1, т.к. каждая ветвь начинается и заканчивается в узле)
    {                                                                                   // т.е. если от n-1 узлов все ветви построены, то от n-ого узла не построить новую
        for (size_t j = 0; j < nodes[i].size(); j++)                                    // перебор по выводам i-го узла (строим ветви от узла во все стороны)
        {
            if (!pinsInBranches[nodes[i][j]])                                            // если рассматриваемый пин nodes[i][j] не входит ни в одну из ветвей
            {
                size_t pinForBranch = nodes[i][j];                                      // сохраняем номер этого пина в переменную - он будет первым в новой ветви
                branches.push_back({});                                                 // добавляем новую ветвь

                do                                                                      // один элемент точно добавим (потом может быть узел или другой элемент)
                {
                    branches.back().push_back(pinForBranch);                            // добавляем в ветвь пин pinForBranch
                    pinsInBranches[pinForBranch] = true;                                // делаем отметку о том, что пин pinForBranch вошел в ветвь

                    pinForBranch += ((pinForBranch + 1) % 2) - (pinForBranch % 2);      // переинициализируем пин pinForBranch соседним по элементу пином: четный номер
                                                                                        // инкрементируем, нечетный - декрементриуем. т.к. элементы расположены по порядку
                    branches.back().push_back(pinForBranch);                            // добавляем в ветвь пин pinForBranch
                    pinsInBranches[pinForBranch] = true;                                // делаем отметку о том, что пин pinForBranch вошел в ветвь    

                    if (nodes.getNodesForPin()[pinForBranch] >= 0)                      // если пин pinForBranch напрямую соединен с каким-нибудь узлом (-1 - не соединен)
                    {
                        break;                                                          // выходим из цикла формирования ветви
                    }

                    for (size_t k = pinForBranch + 1; ; k++)                            // перебор по строкам матрицы соединений
                    {
                         if (matrix[k][pinForBranch])                                   // ищем пин, с которым соединен пин pinForBranch (не через элемент, а "проводом")
                        {
                            pinForBranch = k;                                           // переинициализируем pinForBranch выводом, соединенным с ним
                            break;                                                      // выходим из цикла поиска
                        }
                    }
                } while (true);                                                         // бесконечный цикл
            }
        }
    }

    sortBranches(findBranchesWithCurrentSource(elements));                              // сортируем ветви по признаку наличия в ветви источника тока 
    updateBranchesOnPins(matrix.size());                                                // обновляем матрицу-отношение mNodesOnPins
}

// геттер для списка ветвей branches
inline const SizeMatr& Branches::get() const
{
    return branches;
}

// геттер строки списка ветвей branches (одномерного вектора)
inline const SizeVect& Branches::operator[](size_t index) const
{
    return branches[index];
}

// геттер матрицы branchesOnPins
inline const BoolMatr& Branches::getBranchesOnPins() const
{
    return branchesOnPins;
}

// геттер размера списка ветвей branches
inline size_t Branches::size() const
{
    return branches.size();
}

// геттер счетчика источников тока
inline size_t Branches::getCurrentSourceCount() const
{
    return currentSourceCount;
}

// метод получения строкового представления списка ветвей branches
inline std::string Branches::toString() const
{
    using namespace std;

    stringstream stream;

    stream << "Ветви:\n\n";

    for (size_t i = 0; i < branches.size(); i++)
    {
        stream << left << setw(4) << i << right << ": ";

        for (size_t j = 0; j < branches[i].size(); j++)
        {
            stream << setw(4) << branches[i][j] << ' ';
        }

        stream << "\n\n";
    }

    return stream.str();
}

// метод для сортировки ветвей в векторе branches по наличию источника тока
// ветви с источником тока - в конец вектора (так с ними потом удобнее работать)
// принимает на вход: булевый вектор наличия источника тока в i-ой ветви
inline void Branches::sortBranches(BoolVect branchesWithCurrentSource)
{
    std::multimap<bool, SizeVect> sortedBranches;                                       // создаем словарь (multimap, т.к. ключи не являются уникальными)
                                                                                            
    for (size_t i = 0; i < branches.size(); i++)                                    
    {
        sortedBranches.emplace(branchesWithCurrentSource[i], branches[i]);              // наполняем словарь парами (ключ: содержит ли ветвь источник тока,
    }                                                                                   // значение: непосредственно, ветвь)

    size_t i = 0;                                                                       // счетчик для индексации по списку ветвей
    for (auto& item : sortedBranches)                                                   // range-based цикл для перебора по словарю
    {
        branches[i] = item.second;                                                      // переписываем ветвь
        i++;                                                                            // инкрементируем счетчик
     }

    currentSourceCount = sortedBranches.count(true);                                    // считаем источники тока и сохраняем
}

// обновления матрицы-отношения между множествами ветвей и пинов такой, что:
// если branchesOnPins[i][j] равно единице, i-ая ветвь содержит j-ый пин 
// принимает на вход: счетчик пинов схемы
inline void Branches::updateBranchesOnPins(const size_t pinCount)
{
    branchesOnPins = BoolMatr(branches.size(), BoolVect(pinCount, false));              // создаем матрицу размера branches.size() x pinCount, заполненную false

    for (size_t i = 0; i < branches.size(); i++)                                        // перебираем ветви
    {
        for (size_t j = 0; j < branches[i].size(); j++)                                 // перебираем пины i-ой ветви
        {
            branchesOnPins[i][branches[i][j]] = true;                                   // в i-ой строке (соответствует i-ой ветви) инициализируем истиной элемент,
        }                                                                               // находящийся в столбце, номер которого хранит в себе j-ый элемент i-ой ветви
    }                                                                                   // (то есть номер пина, который содержится в i-ой ветви)
}

// метод создания вектора отметок о ветвях, содержащих источники тока, такого, что:
// если branchesWithCurrentSource[i] равно единице, i-ая ветвь содержит источник тока
// принимает на вход: вектор элементов схемы для определения типа элемента;
inline BoolVect Branches::findBranchesWithCurrentSource(const Elements& elements) const
{
    BoolVect branchesWithCurrentSource(branches.size(), false);                         // создаем вектор размером branches.size() и заполняем его false
    size_t foundCurrentSourceCount = 0;                                                 // переменная, хранящая в себе количество найденных источников тока 

    for (size_t i = 0; i < branches.size(); i++)                                        // перебираем ветви
    {
        for (size_t j = 0; j < branches[i].size(); j += 2)                              // перебираем элементы ветви
        {
            if (elements[branches[i][j] / 2].getType() == Element::Type::J)             // если данный вывод принадлежит источнику тока
            {
                branchesWithCurrentSource[i] = true;                                    // инициализируем элемент массива - ветвь содержит источник тока
                foundCurrentSourceCount++;                                              // инкрементируем счетчик найденных источников тока
                break;                                                                  // выходим из цикла перебора ветви по элементам (т.к. в одной ветви
            }                                                                           // не может быть двух источников тока)
        }
    }

    return branchesWithCurrentSource;                                                   // возвращаем искомый вектор
}

} // namespace CS
