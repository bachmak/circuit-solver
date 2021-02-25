#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/pin_matrix.h"

namespace CS
{
// класс для представления узлов схемы
//////////////////////////////////////////////////////////////////////////////////////////
class Nodes
{
public:
    void update(const PinMatrix& pinMatrix);                                            // метод обновления всех структур
    const SizeVect& operator[] (size_t index) const;                                    // геттер строки списка узлов nodes (одномерного вектора)
    const IntVect& getNodesForPin() const;                                              // геттер вектора nodesForPin
    const BoolMatr& getNodesOnPins() const;                                             // геттер матрицы nodesOnPins
    size_t size() const;                                                                // геттер размера списка узлов nodes
    std::string toString() const;                                                       // метод получения строкового представления списка узлов nodes

private:
    SizeMatr nodes;                                                                     // вектор векторов номеров пинов, представляющий список узлов схемы
    IntVect nodesForPin;                                                                // вектор номеров узлов, в которые входят пины
    BoolMatr nodesOnPins;                                                               // матрица-отношение между множеством узлов и пинов

    void updateNodesOnPins(const size_t pinCount);                                      // метод обновления матрицы nodesOnPins
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления списка узлов
// каждый узел представляется вектором номеров пинов, образующих этот узел
// так же обновляет вектор nodesForPin хранящий информацию о том, какой узел формирует каждый пин
// принимает на вход: матрицу соединений пинов, по которой осуществляется поиск узлов
inline void Nodes::update(const PinMatrix& pinMatrix)
{
    nodes.clear();                                                                      // очищаем текущий вектор узлов
    nodesForPin = IntVect(pinMatrix.size(), -1);                                        // вектор, хранящий номера узлов, которые формируют выводы (-1 - узел отсутствует)
    BoolVect checkedPins(pinMatrix.size(), false);                                      // временный вектор для индикации уже рассмотренных выводов, заполненный false

    for (size_t i = 0; i < pinMatrix.size() - 2; i++)                                   // перебор по столбцам: -2, т.к. два вывода не могут сформировать узел
    {                                                                                   // по столбцам, т.к. работаем в левой нижней половине матрицы соединений
        if (!checkedPins[i])                                                            // если вывод i не участвовал в формировании других узлов
        {
            bool mayBeNode = false;                                                     // логическая переменная для индикации возможности формирования узла 
            bool nodeIsCreated = false;                                                 // логическая переменная для индикации созданного узла
            size_t secondPinForNode;                                                    // переменная для хранения номера вывода, с которым может быть узел 

            for (size_t j = i + 1; j < pinMatrix.size(); j++)                           // перебор элементов по столбцу i вниз от главной диагонали
            {
                if (pinMatrix[j][i] && !checkedPins[j])                                 // если в текущей позиции единица и текущий вывод не участвовал в 
                {                                                                       // формировании других узлов
                    if (mayBeNode)                                                      // если уже есть соединение двух выводов и возможно наличие узла
                    {
                        if (!nodeIsCreated)                                             // если узел еще не создан
                        {
                            nodes.push_back({ i, secondPinForNode });                   // добавить новый узел
                            nodesForPin[i] = nodesForPin[secondPinForNode] =
                                static_cast<int>(nodes.size()) - 1;                     // сохранение номера узла, сформированного выводами i и secondPinForNode
                            nodeIsCreated = true;                                       // узел создан
                        }

                        nodes.back().push_back(j);                                      // добавить в последний узел вывод с номером j
                        nodesForPin[j] = static_cast<int>(nodes.size()) - 1;            // сохранение номера узла, сформированного выводом j
                        checkedPins[j] = true;                                          // вывод j принял участие в формировании узла
                    }
                    else                                                                // если рассматриваемый вывод - первый присоединенный к i-му
                    {
                        mayBeNode = true;                                               // возможно наличие узла
                        secondPinForNode = j;                                           // второй вывод для узла - рассматриваемый
                        checkedPins[i] = checkedPins[secondPinForNode] = true;          // отметки о том, что выводы i и secondPinForNode были рассмотрены
                    }
                }
            }
        }
    }

    updateNodesOnPins(pinMatrix.size());                                                // обновляем матрицу-отношение mNodesOnPins
}

// геттер строки списка узлов nodes (одномерного вектора)
inline const SizeVect& Nodes::operator[](size_t index) const
{
    return nodes[index];
}

// геттер вектора nodesForPin
inline const IntVect& Nodes::getNodesForPin() const
{
    return nodesForPin;
}

// геттер матрицы nodesOnPins
inline const BoolMatr& Nodes::getNodesOnPins() const
{
    return nodesOnPins;
}

// геттер размера списка узлов nodes
inline size_t Nodes::size() const
{
    return nodes.size();
}

// метод получения строкового представления списка узлов nodes
inline std::string Nodes::toString() const
{
    using namespace std;

    stringstream stream;

    stream << "Узлы:\n\n";

    for (size_t i = 0; i < nodes.size(); i++)
    {
        stream << left << setw(4) << i << right << ": ";

        for (size_t j = 0; j < nodes[i].size(); j++)
        {
            stream << setw(4) << nodes[i][j] << ' ';
        }

        stream << "\n\n";
    }

    return stream.str();
}

// обновления матрицы-отношения между множествами узлов и пинов такой, что:
// если mNodesOnPins[i][j] равно единице, i-ый узел содержит j-ый пин (напрямую с ним соединен) 
// принимает на вход: счетчик пинов схемы
inline void Nodes::updateNodesOnPins(const size_t pinCount)
{
    nodesOnPins = BoolMatr(nodes.size(), BoolVect(pinCount, false));                    // создаем матрицу размером nodes.size() x pinCount и заполняем ее false

    for (size_t i = 0; i < nodes.size(); i++)                                           // перебираем все узлы
    {
        for (size_t j = 0; j < nodes[i].size(); j++)                                    // перебираем все пины i-го узла
        {
            nodesOnPins[i][nodes[i][j]] = true;                                         // в i-ой строке (соответствует i-ому узлу) инициализируем истиной элемент,
        }                                                                               // находящийся в столбце, номер которого хранит в себе j-ый элемент i-ого узла
    }                                                                                   // (то есть номер пина, с которым напрямую соединен i-ый узел)
}

} // namespace CS
