#pragma once

#include <queue>
#include <memory>

#include "circuit-solver/circuit_configuration/inc_matrix.h"

namespace CS
{
// класс для представления узла дерева для поиска контуров схемы
//////////////////////////////////////////////////////////////////////////////////////////
class LoopTreeNode : public std::enable_shared_from_this<LoopTreeNode>
{
public:
    using Pointer = std::shared_ptr<LoopTreeNode>;

    struct Data                                                                         // структура для хранения данных в каждом узле
    {
        int branch;                                                                     // номер ветви схемы
        size_t node;                                                                    // номер узла схемы
    };

    LoopTreeNode(const Data nodeData);                                                  // конструктор по данным
    LoopTreeNode(const Pointer node);                                                   // конструктор копирования
    Pointer addChild(const Data nodeData);                                              // метод создания потомка
    Data getData();                                                                     // геттер хранимых данных
    Pointer getParent();                                                                // геттер указателя на родителя
    Pointer createTree(const IncMatrix& matrix);                                        // метод построения дерева по матрице инцидентности от текущего узла

private:
    Data data;                                                                          // хранимые в узле данные
    Pointer parent;                                                                     // указатель на родительский узел
    std::vector<Pointer> children;                                                      // вектор указателей на дочерние узлы

    bool alreadyExists(const size_t schemeNode) const;                                  // метод для проверки существования на пути к корню узла
                                                                                        // с хранимым номером узла схемы, равным хранимому в текущем (this)
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод определения наличия на пути к родителю дерева элемента с номером узла schemeNode
inline bool LoopTreeNode::alreadyExists(const size_t schemeNode) const
{
    LoopTreeNode node = *this;                                                          // создаем переменную для хранения рассматриваемого узла 

    while (node.parent)                                                                 // пока не достигнут корень дерева
    {
        if (schemeNode == node.data.node)                                               // если номера узлов схемы совпадают
        {
            return true;                                                                // уже существует
        }
        node = node.parent;                                                             // переход на уровень выше
    }
    return false;                                                                       // еще не существует
}

// конструктор по хранимым данным
inline LoopTreeNode::LoopTreeNode(const Data data) : 
    data{ data }, parent{ nullptr } {}

// конструктор копирования
inline LoopTreeNode::LoopTreeNode(const Pointer node) :
    data{ node->data }, parent{ node->parent }, children{ node->children } {}

// метод добавления потомка по хранимым данным
inline LoopTreeNode::Pointer LoopTreeNode::addChild(const Data data)
{
    Pointer child = std::make_shared<LoopTreeNode>(data);                               // создаем потомка по данным data
    child->parent = shared_from_this();                                                 // родителя потомка инициализируем текущим узлом
    this->children.push_back(child);                                                    // в вектор потомков текущего узла помещаем нового потомка
    return child;                                                                       // возвращаем указатель на потомка
}

// метод получения данных, хранимых узлом
inline LoopTreeNode::Data LoopTreeNode::getData()
{
    return data;
}

// метод получения адреса родителя узла
inline LoopTreeNode::Pointer LoopTreeNode::getParent()
{
    return parent;
}

// метод построения дерева ветвей для выделения контуров схемы
inline LoopTreeNode::Pointer LoopTreeNode::createTree(const IncMatrix& matrix)
{
    std::queue<Pointer> nodeQueue;                                                      // создаем очередь для обхода дерева в ширину
    nodeQueue.push(shared_from_this());                                                 // помещаем в конец очереди корневой узел

    while (!nodeQueue.empty())                                                          // если очередь не пуста, ищем дальше
    {
        Pointer node = nodeQueue.front();                                               // извлекаем узел из начала очереди
        nodeQueue.pop();                                                                //

        for (size_t i = 0; i < matrix.getUnknownCurrentCount(); i++)                    // перебираем строки матрицы инцидентности
        {
            if (i == abs(node->data.branch) - 1u)                                       // если номер текущей строки соответствует хранимому в узле номеру ветви
            {
                continue;                                                               // пропускаем эту строку
            }

            if (matrix[i][node->data.node])                                             // если i-ая ветвь инцидентна узлу схемы, номер которого хранится в текущем узле
            {
                for (size_t j = 0; ; j++)                                               // перебираем столбцы i-ой строки
                {
                    if (matrix[i][j] == -matrix[i][node->data.node])                    // если i-ая ветвь инцидентна j-ому узлу в противоположном направлении относительно
                    {                                                                   // узла схемы, номер которого хранится в текущем узле
                        int branch = static_cast<int>(i + 1u) * matrix[i][j];           // инкрементируем номер ветви (чтобы избежать номера +-0) и берем со знаком
                                                                                        // рассматриваемой позиции матрицы инцидентности
                        if (!node->alreadyExists(j))                                    // если по пути от текущего узла node к корню this ещё не существует
                        {                                                               // узел с хранимым номером узла схемы j 
                            Pointer child = node->addChild({ branch, j });              // то добавляем к узлу node потомка с номером ветви branch и номером узла схемы j

                            if (this->data.branch == child->data.branch)                // если номер ветви нового потомка равен номеру ветви корня
                            {
                                this->parent = child;                                   // нужный узел найден, возвращаем его
                                return this->parent;
                            }
                        }
                        break;                                                          // иначе - петля - контур не охватывает ветвь, номер которой хранится в корневом узле
                    }                                                                   // добавлять потомка с таким номером узла схемы не нужно, выходим из цикла
                }
            }
        }

        for (Pointer child : node->children)                                            // добавляем в очередь всех потомков последнего рассмотренного узла
        {
            nodeQueue.push(child);
        }
    }

    this->parent = shared_from_this();                                                  // возвращаем указатель на текущий узел (на случай ветвей, не образующих контуры) 
    return this->parent;                                                                // для удобства дальнешей работы этим указателем инициализируется родитель корня
}

} // namespace CS
