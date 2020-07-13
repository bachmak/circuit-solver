#include "../nodes.h"

// метод обновления списка узлов
// каждый узел представляется вектором номеров пинов, образующих этот узел
// так же обновляет вектор mNodesForPin хранящий информацию о том, какой узел формирует каждый пин
// принимает на вход: матрицу соединений пинов, по которой осуществляется поиск узлов
void Nodes::update(const PinMatrix& pinMatrix)
{
	mNodes.clear();																		// очищаем текущий вектор узлов
	mNodesForPin = int_vect_t(pinMatrix.size(), -1);									// вектор, хранящий номера узлов, которые формируют выводы (-1 - узел отсутствует)
	bool_vect_t checkedPins(pinMatrix.size(), false);									// временный вектор для индикации уже рассмотренных выводов, заполненный false

	for (size_t i = 0; i < pinMatrix.size() - 2; i++)									// перебор по столбцам: -2, т.к. два вывода не могут сформировать узел
	{																					// по столбцам, т.к. работаем в левой нижней половине матрицы соединений
		if (!checkedPins[i])															// если вывод i не участвовал в формировании других узлов
		{
			bool mayBeNode = false;														// логическая переменная для индикации возможности формирования узла 
			bool nodeIsCreated = false;													// логическая переменная для индикации созданного узла
			size_t secondPinForNode;													// переменная для хранения номера вывода, с которым может быть узел 

			for (size_t j = i + 1; j < pinMatrix.size(); j++)							// перебор элементов по столбцу i вниз от главной диагонали
			{
				if (pinMatrix[j][i] && !checkedPins[j])									// если в текущей позиции единица и текущий вывод не участвовал в 
				{																		// формировании других узлов
					if (mayBeNode)														// если уже есть соединение двух выводов и возможно наличие узла
					{
						if (!nodeIsCreated)												// если узел еще не создан
						{
							mNodes.push_back({ i, secondPinForNode });					// добавить новый узел
							mNodesForPin[i] = mNodesForPin[secondPinForNode] =
								static_cast<int>(mNodes.size()) - 1;					// сохранение номера узла, сформированного выводами i и secondPinForNode
							nodeIsCreated = true;										// узел создан
						}

						mNodes.back().push_back(j);										// добавить в последний узел вывод с номером j
						mNodesForPin[j] = static_cast<int>(mNodes.size()) - 1;			// сохранение номера узла, сформированного выводом j
						checkedPins[j] = true;											// вывод j принял участие в формировании узла
					}
					else																// если рассматриваемый вывод - первый присоединенный к i-му
					{
						mayBeNode = true;												// возможно наличие узла
						secondPinForNode = j;											// второй вывод для узла - рассматриваемый
						checkedPins[i] = checkedPins[secondPinForNode] = true;			// отметки о том, что выводы i и secondPinForNode были рассмотрены
					}
				}
			}
		}
	}

	updateNodesOnPins(pinMatrix.size());												// обновляем матрицу-отношение mNodesOnPins
}

const size_vect_t& Nodes::operator[](size_t index) const
{
	return mNodes[index];
}

const int_vect_t& Nodes::getNodesForPin() const
{
	return mNodesForPin;
}

const bool_matr_t& Nodes::getNodesOnPins() const
{
	return mNodesOnPins;
}

size_t Nodes::size() const
{
	return mNodes.size();
}

std::string Nodes::toString() const
{
	using namespace std;

	stringstream stream;

	stream << "Узлы:\n\n";

	for (size_t i = 0; i < mNodes.size(); i++)
	{
		stream << left << setw(4) << i << right << ": ";

		for (size_t j = 0; j < mNodes[i].size(); j++)
		{
			stream << setw(4) << mNodes[i][j] << ' ';
		}

		stream << "\n\n";
	}

	return stream.str();
}

// обновления матрицы-отношения между множествами узлов и пинов такой, что:
// если mNodesOnPins[i][j] равно единице, i-ый узел содержит j-ый пин (напрямую с ним соединен) 
// принимает на вход: счетчик пинов схемы
void Nodes::updateNodesOnPins(const size_t pinCount)
{
	mNodesOnPins = bool_matr_t(mNodes.size(), bool_vect_t(pinCount, false));			// создаем матрицу размером nodes.size() x pinCount и заполняем ее false

	for (size_t i = 0; i < mNodes.size(); i++)											// перебираем все узлы
	{
		for (size_t j = 0; j < mNodes[i].size(); j++)									// перебираем все пины i-го узла
		{
			mNodesOnPins[i][mNodes[i][j]] = true;										// в i-ой строке (соответствует i-ому узлу) инициализируем истиной элемент,
		}																				// находящийся в столбце, номер которого хранит в себе j-ый элемент i-ого узла
	}																					// (то есть номер пина, с которым напрямую соединен i-ый узел)
}
