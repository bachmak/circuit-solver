#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/inc_matrix.h"
#include "circuit-solver/structure_units/loop_tree_node.h"

namespace CS
{
// класс для представления информации о замкнутых контурах схемы
//////////////////////////////////////////////////////////////////////////////////////////
class Loops
{
public:
    void update(const IncMatrix& matrix);                                               // метод обновления списка контуров
    size_t size() const;                                                                // геттер размера списка контуров (количества контуров)
    const IntVect& operator[] (size_t index) const;                                     // константный геттер контура (последовательности номеров ветвей)
    friend std::ostream& operator<<(std::ostream& os, const Loops& loops);              // перегрузка оператора вывода в поток

private:
    IntMatr loops;                                                                      // список контуров (каждый контур - последовательность номеров ветвей) 
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления списка контуров:
// выполняет обход матрицы инцидентности и с помощью построения 
// дерева ветвей находит замкнутые контуры схемы
inline void Loops::update(const IncMatrix& matrix)
{
    loops.clear();                                                                      // вектор для хранения контуров (номера ветвей)
    BoolVect branchesInLoops(matrix.getUnknownCurrentCount());                          // массив для индикации вхождения ветви в контур (изначально нулевой)

    for (size_t i = 0; i < branchesInLoops.size(); i++)                                 // перебор по ветвям (контуры строятся так, чтобы каждая ветвь 
    {                                                                                   // вошла хотя бы в один контур)
        if (!branchesInLoops[i])                                                        // фильтрация ветвей, еще не вошедших в контур
        {
            for (size_t j = 0; j < matrix[i].size(); j++)                               // перебор по узлам матрицы инцидентности (номер узла нужен, чтобы находить ветви, 
            {                                                                           // инцидентные заданному узлу, то есть смежные с заданной ветвью)
                if (matrix[i][j])                                                       // если ветвь i инцидентна узлу j, отсюда можно строить дерево
                {
                    LoopTreeNode::Data data = { (int)(i + 1) * matrix[i][j], j };       // (*) создание структуры для хранения в узле: ветвь с номером i + 1, узел с номером j
                                                                                        // инкремент нужен для того, чтобы отличать направление 0-й ветви по знаку +-
                    auto node = std::make_shared<LoopTreeNode>(data);                   // создание узла, от которого будет строиться дерево
                    node->createTree(matrix);                                           // построение дерева от узла node
                    loops.push_back({});                                                // помещение в конец вектора контуров пустого вектора для нового контура

                    for (auto itNode = node->getParent()->getParent();                  // ХИТРО: указатель родителя корня node - конечный элемент контура: номера их ветвей
                        itNode->getData().node != node->getData().node;                 // совпадают. Поэтому, чтобы восстановить последовательность ветвей контура, нужно
                        itNode = itNode->getParent())                                   // сместиться ДВАЖДЫ к родителю от корня, а затем восстановить путь от предпоследнего  
                    {                                                                   // элемента контура до родителя включительно
                        loops.back().push_back(itNode->getData().branch * -1);          // добавляем номер ветви, умноженный на -1, в контур
                        branchesInLoops[abs(itNode->getData().branch) - 1] = true;      // делаем отметку о том, что ветвь branch вошла в контур - от неё не нужно строить 
                    }                                                                   // новый контур. Берем по модулю и отнимаем единицу, см. (*)
                    
                    loops.back().push_back(node->getData().branch * -1);                // добавляем умноженный на -1 номер ветви корня в контур

                    if (loops.back().size() == 1)                                       // если контур состоит из одной ветви (в случае узла с тремя ветвями,
                    {                                                                   // две из которых содержат источники тока)
                        loops.pop_back();                                               // извлекаем контур из вектора контуров
                    }
                    else
                    {
                        branchesInLoops[int(abs(node->getData().branch)) - 1] = true;   // делаем отметку о том, что ветвь branch вошла в контур
                    }

                    break;                                                              // выходим из цикла, т.к. одна ветвь может быть инцидентна только двум узлам,
                }                                                                       // при этом, так как ищем только в одном направлении, достаточно одного узла
            }
        }
    }
}

// геттер размера списка контуров
inline size_t Loops::size() const
{
    return loops.size();
}

// константный геттер контура (последовательности номеров ветвей)
inline const IntVect& Loops::operator[](size_t index) const
{
    return loops[index];
}

// перегрузка оператора вывода в поток
inline std::ostream& operator<<(std::ostream& os, const Loops& loops)
{
    os << "Контуры:\n\n";

    for (size_t i = 0; i < loops.size(); i++)
    {
        os << std::left << std::setw(4) << i << std::right << ": ";

        for (size_t j = 0; j < loops[i].size(); j++)
        {
            os << std::setw(4) << abs(loops[i][j]) - 1 << ' ';
        }
        
        os << "\n\n";
    }

    return os;
}

} // namespace CS
