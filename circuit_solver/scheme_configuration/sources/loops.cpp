#include "../loops.h"

void Loops::update(const IncMatrix& matrix)
{
	mLoops = int_matr_t();																// вектор для хранения контуров (номера ветвей)
	bool_vect_t branchesInLoops(matrix.getUnknownCurrentCount());						// массив для индикации вхождения ветви в контур (изначально нулевой)

	for (size_t i = 0; i < matrix.getUnknownCurrentCount(); i++)						// перебор по ветвям (контуры строятся так, чтобы каждая ветвь 
	{																					// вошла хотя бы в один контур)
		if (!branchesInLoops[i])														// фильтрация ветвей, уже вошедших в контур
		{
			for (size_t j = 0; j < matrix[i].size(); j++)								// перебор по узлам матрицы инцидентности (номер узла нужен, чтобы находить ветви, 
			{																			// инцидентные заданному узлу, то есть смежные с заданной ветвью)
				if (matrix[i][j])														// если ветвь i инцидентна узлу j, отсюда можно строить дерево
				{
					NodeData data = { (int)(i + 1) * matrix[i][j], j };					// (*) создание структуры для хранения в узле: ветвь с номером i + 1, узел с номером j
																						// инкремент нужен для того, чтобы отличать направление 0-й ветви по знаку +-
					Node* node = new Node(data);										// создание узла, от которого будет строиться дерево
					node->createTree(matrix);											// построение дерева от узла node
					mLoops.push_back({});												// помещение в конец вектора контуров пустого вектора для нового контура

					for (Node* itNode = node->getParent()->getParent();					// ХИТРО: указатель родителя корня node - конечный элемент контура: номера их ветвей
						itNode->getData().node != node->getData().node;					// совпадают. Поэтому, чтобы восстановить последовательность ветвей контура, нужно
						itNode = itNode->getParent())									// сместиться ДВАЖДЫ к родителю от корня, а затем восстановить путь от предпоследнего  
					{																	// элемента контура до родителя включительно
						mLoops.back().push_back(itNode->getData().branch * -1);			// добавляем номер ветви, умноженный на -1, в контур
						branchesInLoops[abs(itNode->getData().branch) - 1] = true;		// делаем отметку о том, что ветвь branch вошла в контур - от неё не нужно строить 
					}																	// новый контур. Берем по модулю и отнимаем единицу, см. (*)
					
					mLoops.back().push_back(node->getData().branch * -1);				// добавляем умноженный на -1 номер ветви корня в контур

					if (mLoops.back().size() == 1)										// если контур состоит из одной ветви (в случае узла с тремя ветвями,
					{																	// две из которых содержат источники тока)
						mLoops.pop_back();												// извлекаем контур из вектора контуров
					}
					else
					{
						branchesInLoops[int(abs(node->getData().branch)) - 1] = true;	// делаем отметку о том, что ветвь branch вошла в контур
					}

					break;																// выходим из цикла, т.к. одна ветвь может быть инцидентна только двум узлам,
				}																		// при этом, так как ищем только в одном направлении, достаточно одного узла
			}
		}
	}
}

size_t Loops::size() const
{
	return mLoops.size();
}

const int_vect_t& Loops::operator[](size_t index) const
{
	return mLoops[index];
}

std::string Loops::toString() const
{
	using namespace std;

	stringstream stream;

	stream << "Контуры:\n\n";

	for (size_t i = 0; i < mLoops.size(); i++)
	{
		stream << left << setw(4) << i << right << ": ";

		for (size_t j = 0; j < mLoops[i].size(); j++)
		{
			stream << setw(4) << abs(mLoops[i][j]) - 1 << ' ';
		}

		stream << "\n\n";
	}

	return stream.str();
}
