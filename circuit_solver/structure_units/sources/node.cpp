#include "../node.h"

// метод определения наличия на пути к родителю дерева элемента с номером узла schemeNode
bool Node::alreadyExists(const size_t schemeNode) const
{
	Node node = *this;																		// создаем переменную для хранения рассматриваемого узла 

	while (node.mParent)																	// пока не достигнут корень дерева
	{
		if (schemeNode == node.mData.node)													// если номера узлов схемы совпадают
		{
			return true;																	// уже существует
		}
		node = node.mParent;																// переход на уровень выше
	}
	return false;																			// еще не существует
}

// конструктор по хранимым данным
Node::Node(const NodeData data) : 
	mData { data }, 
	mParent { nullptr }
{
}

// конструктор копирования
Node::Node(const Node* node) :
	mData{ node->mData },
	mParent{ node->mParent },
	mChildren{ node->mChildren }
{
}

// метод добавления потомка по хранимым данным
Node* Node::addChild(const NodeData data)
{
	Node* child = new Node(data);														// создать потомка по даннмы data
	child->mParent = this;																// родителя потомка инициализировать текущим узлом
	this->mChildren.push_back(child);													// в вектор потомков текущего узла поместить нового потомка
	return child;																		// вернуть указатель на потомка
}

// метод получения данных, хранимых узлом
NodeData Node::getData()
{
	return mData;
}

// метод получения адреса родителя узла
Node* Node::getParent()
{
	return mParent;
}

// метод построения дерева ветвей для выделения контуров схемы
Node* Node::createTree(const IncMatrix& matrix)									
{
	std::queue<Node*> nodeQueue;														// создаем очередь для обхода дерева в ширину
	nodeQueue.push(this);																// помещаем в конец очереди корневой узел

	while (!nodeQueue.empty())															// если очередь не пуста, ищем дальше
	{
		Node* node = nodeQueue.front();													// извлекаем узел из начала очереди
		nodeQueue.pop();																//

		for (size_t i = 0; i < matrix.getUnknownCurrentCount(); i++)					// перебираем строки матрицы инцидентности
		{
			if (i == abs(node->mData.branch) - 1u)										// если номер текущей строки соответствует хранимому в узле номеру ветви
			{
				continue;																// пропускаем эту строку
			}

			if (matrix[i][node->mData.node])											// если i-ая ветвь инцидентна узлу схемы, номер которого хранится в текущем узле
			{
				for (size_t j = 0; ; j++)												// перебираем столбцы i-ой строки
				{
					if (matrix[i][j] == -matrix[i][node->mData.node])					// если i-ая ветвь инцидентна j-ому узлу в противоположном направлении относительно
					{																	// узла схемы, номер которого хранится в текущем узле
						int branch = static_cast<int>(i + 1u) * matrix[i][j];			// инкрементируем номер ветви (чтобы избежать номера +-0) и берем со знаком
																						// рассматриваемой позиции матрицы инцидентности
						if (!node->alreadyExists(j))									// если по пути от текущего узла node к корню this ещё не существует
						{																// узел с хранимым номером узла схемы j 
							Node* child = node->addChild({ branch, j });				// то добавляем к узлу node потомка с номером ветви branch и номером узла схемы j

							if (this->mData.branch == child->mData.branch)				// если номер ветви нового потомка равен номеру ветви корня
							{
								this->mParent = child;									// нужный узел найден, возвращаем его
								return this->mParent;
							}
						}
						break;															// иначе - петля - контур не охватывает ветвь, номер которой хранится в корневом узле
					}																	// добавлять потомка с таким номером узла схемы не нужно, выходим из цикла
				}
			}
		}

		for (Node* child : node->mChildren)												// добавляем в очередь всех потомков последнего рассмотренного узла
		{
			nodeQueue.push(child);
		}
	}

	this->mParent = this;																// возвращаем указатель на текущий узел (на случай ветвей, не образующих контуры) 
	return this->mParent;																// для удобства дальнешей работы этим указателем инициализируется родитель корня
}
