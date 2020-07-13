#pragma once

#include <queue>

#include "../type_definitions.h"
#include "node_data.h"
#include "../scheme_configuration/inc_matrix.h"

// класс для представления узла древовидного графа
class Node
{
public:
	Node(const NodeData nodeData);														// конструктор по данным
	Node(const Node* node);																// конструктор копирования
	Node* addChild(const NodeData nodeData);											// метод создания потомка
	NodeData getData();																	// геттер хранимых данных
	Node* getParent();																	// геттер указателя на родителя
	Node* createTree(const IncMatrix& matrix);											// метод построения дерева по матрице инцидентности от текущего узла

private:
	NodeData mData;																		// хранимые в узле данные
	Node* mParent;																		// указатель на родительский узел
	std::vector<Node*> mChildren;														// вектор указателей на дочерние узлы

	bool alreadyExists(const size_t schemeNode) const;									// метод для проверки существования на пути к корню узла
																						// с хранимым номером узла схемы, равным хранимому в текущем (this)
};
