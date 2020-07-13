#include "../inc_matrix.h"

// метод обновления матрицы-отношения между множествами ветвей и узлов такой, что:
// если mMatrix[i][j] равно -1, ток в i-ой ветви вытекает из j-го узла;
// если mMatrix[i][j] равно 1, ток в i-ой ветви втекает в j-ый узел;
// принимает на вход:
// 1) объект nodes для информации об узлах схемы
// 2) объект branches для информации о ветвях схемы
// 2) матрицу-отношение между множествами узлов и пинов
void IncMatrix::update(const Nodes& nodes, const Branches& branches, const elem_vect_t& elements)
{
	mMatrix = int_matr_t(branches.size(), int_vect_t(nodes.size(), 0));					// создаем нулевую матрицу размера branchesSize x nodesSize, заполненную нулями

	bool_matr_t branchesOnPins = branches.getBranchesOnPins();
	bool_matr_t nodesOnPins = nodes.getNodesOnPins();

	for (size_t i = 0; i < branchesOnPins.size(); i++)									// перебираем строки первого отношения (ветви)
	{
		for (size_t j = 0; j < nodesOnPins.size(); j++)									// перебираем строки второго отношения (узлы)
		{
			for (size_t k = 0; k < nodesOnPins[j].size(); k++)							// перебираем столбцы отношений (пины)
			{
				mMatrix[i][j] |= 
					nodes.getNodesOnPins()[j][k] && branches.getBranchesOnPins()[i][k];	// дизъюнкция всех конъюнкций элемента из первого отношения с элементом из второго
			}																			// (если у узла j найдется хоть один общий пин с ветвью i, то соединяем их)
		}
	}

	orient(nodes, branches, elements);
}

size_t IncMatrix::size() const
{
	return mMatrix.size();
}

const int_vect_t& IncMatrix::operator[](size_t index) const
{
	return mMatrix[index];
}

std::string IncMatrix::toString() const
{
	using namespace std;

	stringstream stream;

	stream << "Матрица инцидентности:\n\n";

	if (!mMatrix.empty())
	{
		stream << setw(6) << ' ';

		for (size_t i = 0; i < mMatrix[0].size(); i++)
		{
			stream << setw(4) << i << ' ';
		}

		stream << "\n\n";

		for (size_t i = 0; i < mMatrix.size(); i++)
		{
			stream << left << setw(4) << i << right << ": ";

			for (size_t j = 0; j < mMatrix[i].size(); j++)
			{
				stream << setw(4) << mMatrix[i][j] << ' ';
			}

			stream << "\n\n";
		}
	}

	return stream.str();
}

size_t IncMatrix::getUnknownCurrentCount() const
{
	return mUnknownCurrentCount;
}

void IncMatrix::orient(const Nodes& nodes, const Branches& branches, const elem_vect_t& elements)
{
	mUnknownCurrentCount = branches.size() - branches.getCurrentSourceCount();		// сохраняем количество неизвестных токов

	for (size_t i = 0; i < mUnknownCurrentCount; i++)								// перебор по ветвям с неизвестными токами (строки в матрице сортированы)
	{
		size_t nodeNumber = nodes.getNodesForPin().at(branches[i][0]);				// промежуточная переменная - номер узла, с которым соединен первый пин i-ой ветви
		mMatrix[i][nodeNumber] = -1;												// инициализируем -1 позицию, соответствующую соединению i-ой ветви с узлом nodeNumber 
	}																				// т.е. направляем ток от узла с меньшим номером к узлу с большим

	for (size_t i = mUnknownCurrentCount; i < branches.size(); i++)					// перебор по ветвям с источниками тока
	{
		for (size_t j = 0; ; j += 2)												// перебираем пины ветви - ищем источник тока
		{
			if (elements[branches[i][j] / 2].getType() == Type::J)					// если элемент является источником тока
			{
				if (branches[i][j] % 2)												// если рассматривается нечетный (положительный) вывод источника 
				{
					mMatrix[i][nodes.getNodesForPin().at(branches[i].back())] = -1;	// инициализируем последний элемент ветви значением -1 (ток от конца ветви) 
				}
				else																// если рассматривается четный (отрицательный) вывод источника
				{
					mMatrix[i][nodes.getNodesForPin().at(branches[i][0])] = -1;		// инициализируем первый элемент ветви значением -1 (ток от начала ветви)
				}
				break;																// выходим из цикла перебора по элементам
			}
		}
	}
}
