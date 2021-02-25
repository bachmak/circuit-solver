#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/elements.h"
#include "circuit-solver/circuit_configuration/nodes.h"
#include "circuit-solver/circuit_configuration/branches.h"
#include "circuit-solver/circuit_configuration/inc_matrix.h"
#include "circuit-solver/circuit_configuration/loops.h"

namespace CS
{
class Equations
{
public:
	void update(const IncMatrix& matrix, const Loops& loops,
                const Branches& branches, const Elements& elements);
	const CoeffMatr& left() const;
	const CoeffMatr& right() const;
	std::string toString(size_t precision) const;

private:
	CoeffMatr leftPart;
	CoeffMatr rightPart;
	size_t unknownCurrentCount = 0;
	size_t currentSourceCount = 0;
	size_t voltageSourceCount = 0;
	size_t knownSourceCount = 0;
	size_t firstLawCount = 0;
	size_t secondLawCount = 0;

	void update1stLawEquations(const IncMatrix& matrix, const Elements& elements);
	void update2ndLawEquations(const Loops& loops, const Branches& branches,
                               const Elements& elements);

	size_t getCurrentSourceElemIndex(const size_t currentSourceIndex,
                                     const Elements& elements) const;
	size_t getVoltageSourceIndex(const size_t elemIndex,
                                 const Elements& elements) const;
};

inline void Equations::update(const IncMatrix& matrix, const Loops& loops,
                              const Branches& branches, const Elements& elements)
{
	unknownCurrentCount = matrix.getUnknownCurrentCount();

	leftPart = CoeffMatr(unknownCurrentCount,											// создаем двумерную матрицу нулевых коэффициентов
		CoeffVect(unknownCurrentCount, Coefficient(ZERO_PTR)));	    					// размером unknownCurrentNumber х unknownCurrentNumber

	currentSourceCount = elements.getCurrentSourceCount();     							// получаем количество источников тока 

	voltageSourceCount = elements.getVoltageSourceCount();     							// получаем количество источников напряжения 

	knownSourceCount = currentSourceCount + voltageSourceCount;							// считаем все источники

	rightPart = CoeffMatr(matrix.getUnknownCurrentCount(),								// создаем матрицу нулевых коэффициентов 
		CoeffVect(knownSourceCount, Coefficient(ZERO_PTR)));							// размера unknownCurrentCount x knownSourceCount

	firstLawCount = matrix.size() ? matrix[0].size() - 1 : 0;
	secondLawCount = loops.size();

	update1stLawEquations(matrix, elements);
	update2ndLawEquations(loops, branches, elements);

    for (size_t i = 0; i < leftPart.size(); i++)
    {
        leftPart[i][i] += 1;
    }
}

inline const CoeffMatr& Equations::left() const
{
	return leftPart;
}

inline const CoeffMatr& Equations::right() const
{
	return rightPart;
}

inline std::string Equations::toString(size_t precision) const
{
	using namespace std;

	stringstream stream;

	stream << "Система уравнений:\n\n" << setw(9) << ' ';

	stream << std::left << fixed << setprecision(precision);

	for (size_t i = 0; i < unknownCurrentCount; i++)
	{
		stream << "I" << setw(precision + 9) << i << ' ';
	}
	stream << setw(precision + 10) << ' ';

	for (size_t i = 0; i < currentSourceCount; i++)
	{
		stream << "J" << setw(precision + 9) << i << ' ';
	}

	for (size_t i = 0; i < voltageSourceCount; i++)
	{
		stream << "E" << setw(precision + 9) << i << ' ';
	}

	stream << "\n\n";

	for (size_t i = 0; i < unknownCurrentCount; i++)
	{
		stream << setw(4) << i << ":    ";

		for (size_t j = 0; j < unknownCurrentCount; j++)
		{
			stream << setw(precision + 10) << leftPart[i][j]() << ' ';
		}

		stream << setw(precision + 10) << ' ';

		for (size_t j = 0; j < knownSourceCount; j++)
		{
			stream << setw(precision + 10) << leftPart[i][j]() << ' ';
		}

		stream << "\n\n";
	}

	return stream.str();
}

inline void Equations::update1stLawEquations(const IncMatrix& matrix,
                                             const Elements& elements)
{
	for (size_t i = 0; i < matrix[0].size() - 1; i++)									// перебираем столбцы матрицы инцидентности. сколько столбцов - столько узлов -
	{																					// столько уравнений (за исключением одного линейно зависимого столбца)
		for (size_t j = 0; j < unknownCurrentCount; j++)								// перебираем строки, соответствующие неизвестным токам (здесь нет значений,
		{																				// связанных с параметрами элементов - только +-1 или 0 - зависит от геометрии схемы)
			switch (matrix[j][i])														// смотрим, как соотносится j-ый ток и i-ый узел 
			{
			case -1:																	// если ток вытекает из узла
				leftPart[i][j] = ONE_PTR;												// присваиваем указатель на единицу
				leftPart[i][j].setSign(-1);											    // знак отрицательный
				break;																	// выходим из switch

			case 1:																		// если ток втекает в узел
				leftPart[i][j] = ONE_PTR;												// присваиваем указатель на единицу (знак по умолчанию положительный)
				break;																	// выходим из switch

			default:																	// если j-ый ток не связан с i-ым узлом (j-ая ветвь не инцидентна i-ому узлу)
				leftPart[i][j] = ZERO_PTR;												// присваиваем указатель на ноль
			}
		}

		for (size_t j = 0; j < currentSourceCount; j++)								    // перебираем строки, соответствующие ветвям с источниками тока
		{
			if (matrix[j + unknownCurrentCount][i])									    // если j-ая ветвь с источником тока связана с i-ым узлом 
			{
				size_t index = getCurrentSourceElemIndex(j, elements);					// вычисляем индекс j-го источника тока в списке элементов схемы
				const double* pointer = elements[index].getValuePointer();				// по индексу в списке элементов получаем адрес значения тока источника тока
				int sign = -matrix[j + unknownCurrentCount][i];						    // определяем знак тока от j-го источника тока в i-ом узле (инверсия, так как 
																						// knownSourceMatrix - правая часть системы уравнений)
				rightPart[i][j] = pointer;												// присваиваем полученный адрес на значение
				rightPart[i][j].setSign(sign);											// присваиваем знак
			}
			else																		// если j-ая ветвь с источником тока не связана с i-ым узлом 
			{
				rightPart[i][j] = ZERO_PTR;											    // присваиваем указатель на ноль
			}
		}

		for (size_t j = 0; j < voltageSourceCount; j++)								    // присваиваем указатели на нулевые значения в оставшиеся позиции, соответствующие
		{																				// источникам напряжения, т.к. они не участвуют в формировании уравнений по первому
			rightPart[i][j + currentSourceCount] = ZERO_PTR;							// закону Кирхгофа
		}
	}
}

inline void Equations::update2ndLawEquations(const Loops& loops,
                                             const Branches& branches,
                                             const Elements& elements)
{
	for (size_t i = 0; i < loops.size(); i++)											// перебор по контурам схемы
	{
		for (size_t j = 0; j < loops[i].size(); j++)									// перебор по ветвям контура
		{
			size_t branch = abs(loops[i][j]) - (int)1;									// приводим номер ветви к исходному виду (без знака и инкремента)
			int branchSign = loops[i][j] > 0 ? 1 : -1;									// определяем знак тока в ветви branch 

			for (size_t k = 0; k < branches[branch].size(); k += 2)						// перебираем ветвь по элементам схемы
			{
				size_t elemIndex = branches[branch][k] / 2;								// вычисляем индекс элемента по номеру пина k в ветви branch
				const double* pointer = elements[elemIndex].getValuePointer();			// получаем адрес значения элемента

				if (elements[elemIndex].getType() == Element::Type::R)					// если элемент является сопротивлением
				{
					leftPart[firstLawCount + i][branch] = pointer;	        			// инициализируем коэффициент, соответствующий i-му уравнению 
																						// по второму закону Кирхгофа и току branch, адресом pointer 
					leftPart[firstLawCount + i][branch].setSign(branchSign);			// задаем соответствующий знак тока
				}
				else																	// иначе - источник напряжения
				{
					size_t voltageSourceIndex =											// вычисляем порядковый номер источника напряжения в системе уравнений
						getVoltageSourceIndex(elemIndex, elements);						// по порядковому номеру элемента в векторе элементов

					bool pinIsOdd = branches[branch][k] % 2;							// определяем, является ли пин k ветви branch нечетным (плюсом источника)
					int valueSign = 1;													// переменная для знака вхождения напряжения в уравнение

					if (pinIsOdd)														// если рассматриваемый пин нечетный (по допущению, четному (меньшему) id пина 
					{																	// соответсвует отрицательный полюс источника, нечетному - положительный)
						valueSign = -1;													// пины ветви branch рассматриваются через один, то есть если рассматриваемый 
					}																	// пин нечетный - идем против направления ветви - знак напряжения отрицательный

					size_t index = currentSourceCount + voltageSourceIndex;			// вычисляем порядковый номер коэффициента источника напряжения в уравнении
					rightPart[firstLawCount + i][index] = pointer;					// инициализируем коэффициент, соответствующий i-му уравнению
																						// по второму закону Кирхгофа и напряжению index, адресом pointer
					rightPart[firstLawCount + i][index].setSign(branchSign*valueSign);// задаем соответствующий знак c учетом направления ветви в контуре и 
				}																		// ориентации источника напряжения
			}
		}
	}
}

inline size_t Equations::getVoltageSourceIndex(const size_t elemIndex,
                                               const Elements& elements) const
{
	size_t counter = 0;																	// переменная для подсчета источников напряжения

	for (size_t i = 0; i < elemIndex; i++)												// перебор элементов схемы до элемента с номером elemIndex 
	{
		if (elements[i].getType() == Element::Type::E)									// если нашли источник напряжения
		{
			counter++;																	// инкрементируем счетчик
		}
	}
	return counter;																		// возвращаем значение счетчика
}

inline size_t Equations::getCurrentSourceElemIndex(const size_t currentSourceIndex,
                                                   const Elements& elements) const
{
	size_t counter = 0;																	// переменная для подсчета источников тока

	for (size_t i = 0; i < elements.size(); i++)										// перебираем элементы схемы
	{
		if (elements[i].getType() == Element::Type::J)									// если элемент является источником тока
		{
			if (++counter == currentSourceIndex + 1)									// инкрементируем счетчик источников и сравниваем с искомым значением
			{
				return i;																// если равны, возвращаем номер элемента
			}
		}
	}
	return elements.size();																// если совпадений не нашлось, возвращаем размер вектора элементов
}

} // namespace CS
