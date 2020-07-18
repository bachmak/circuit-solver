#include "../equations.h"

void Equations::update(const IncMatrix& matrix, const Loops& loops, const Branches& branches, const Elements& elements)
{
	mUnknownCurrentCount = matrix.getUnknownCurrentCount();

	mLeftPart = coeff_matr_t(mUnknownCurrentCount,											// создаем двумерную матрицу нулевых коэффициентов
		coeff_vect_t(mUnknownCurrentCount, Coefficient(ZERO_PTR)));							// размером unknownCurrentNumber х unknownCurrentNumber

	mCurrentSourceCount = elements.getCurrentSourceCount();     							// получаем количество источников тока 

	mVoltageSourceCount = elements.getVoltageSourceCount();     							// получаем количество источников напряжения 

	mKnownSourceCount = mCurrentSourceCount + mVoltageSourceCount;							// считаем все источники

	mRightPart = coeff_matr_t(matrix.getUnknownCurrentCount(),								// создаем матрицу нулевых коэффициентов 
		coeff_vect_t(mKnownSourceCount, Coefficient(ZERO_PTR)));							// размера unknownCurrentCount x knownSourceCount

	mFirstLawCount = matrix.size() ? matrix[0].size() - 1 : 0;
	mSecondLawCount = loops.size();

	update1stLawEquations(matrix, elements);
	update2ndLawEquations(loops, branches, elements);

    for (size_t i = 0; i < mLeftPart.size(); i++)
    {
        mLeftPart[i][i] += 1;
    }
    
}

const coeff_matr_t& Equations::left() const
{
	return mLeftPart;
}

const coeff_matr_t& Equations::right() const
{
	return mRightPart;
}

std::string Equations::toString(size_t precision) const
{
	using namespace std;

	stringstream stream;

	stream << "Система уравнений:\n\n" << setw(9) << ' ';

	stream << std::left << fixed << setprecision(precision);

	for (size_t i = 0; i < mUnknownCurrentCount; i++)
	{
		stream << "I" << setw(precision + 9) << i << ' ';
	}
	stream << setw(precision + 10) << ' ';

	for (size_t i = 0; i < mCurrentSourceCount; i++)
	{
		stream << "J" << setw(precision + 9) << i << ' ';
	}

	for (size_t i = 0; i < mVoltageSourceCount; i++)
	{
		stream << "E" << setw(precision + 9) << i << ' ';
	}

	stream << "\n\n";

	for (size_t i = 0; i < mUnknownCurrentCount; i++)
	{
		stream << setw(4) << i << ":    ";

		for (size_t j = 0; j < mUnknownCurrentCount; j++)
		{
			stream << setw(precision + 10) << mLeftPart[i][j]() << ' ';
		}

		stream << setw(precision + 10) << ' ';

		for (size_t j = 0; j < mKnownSourceCount; j++)
		{
			stream << setw(precision + 10) << mLeftPart[i][j]() << ' ';
		}

		stream << "\n\n";
	}

	return stream.str();
}

void Equations::update1stLawEquations(const IncMatrix& matrix, const Elements& elements)
{
	for (size_t i = 0; i < matrix[0].size() - 1; i++)									// перебираем столбцы матрицы инцидентности. сколько столбцов - столько узлов -
	{																					// столько уравнений (за исключением одного линейно зависимого столбца)
		for (size_t j = 0; j < mUnknownCurrentCount; j++)								// перебираем строки, соответствующие неизвестным токам (здесь нет значений,
		{																				// связанных с параметрами элементов - только +-1 или 0 - зависит от геометрии схемы)
			switch (matrix[j][i])														// смотрим, как соотносится j-ый ток и i-ый узел 
			{
			case -1:																	// если ток вытекает из узла
				mLeftPart[i][j] = ONE_PTR;												// присваиваем указатель на единицу
				mLeftPart[i][j].setSign(-1);											// знак отрицательный
				break;																	// выходим из switch

			case 1:																		// если ток втекает в узел
				mLeftPart[i][j] = ONE_PTR;												// присваиваем указатель на единицу (знак по умолчанию положительный)
				break;																	// выходим из switch

			default:																	// если j-ый ток не связан с i-ым узлом (j-ая ветвь не инцидентна i-ому узлу)
				mLeftPart[i][j] = ZERO_PTR;												// присваиваем указатель на ноль
			}
		}

		for (size_t j = 0; j < mCurrentSourceCount; j++)								// перебираем строки, соответствующие ветвям с источниками тока
		{
			if (matrix[j + mUnknownCurrentCount][i])									// если j-ая ветвь с источником тока связана с i-ым узлом 
			{
				size_t index = getCurrentSourceElemIndex(j, elements);					// вычисляем индекс j-го источника тока в списке элементов схемы
				const double* pointer = elements[index].getValuePointer();				// по индексу в списке элементов получаем адрес значения тока источника тока
				int sign = -matrix[j + mUnknownCurrentCount][i];						// определяем знак тока от j-го источника тока в i-ом узле (инверсия, так как 
																						// knownSourceMatrix - правая часть системы уравнений)
				mRightPart[i][j] = pointer;												// присваиваем полученный адрес на значение
				mRightPart[i][j].setSign(sign);											// присваиваем знак
			}
			else																		// если j-ая ветвь с источником тока не связана с i-ым узлом 
			{
				mRightPart[i][j] = ZERO_PTR;											// присваиваем указатель на ноль
			}
		}

		for (size_t j = 0; j < mVoltageSourceCount; j++)								// присваиваем указатели на нулевые значения в оставшиеся позиции, соответствующие
		{																				// источникам напряжения, т.к. они не участвуют в формировании уравнений по первому
			mRightPart[i][j + mCurrentSourceCount] = ZERO_PTR;							// закону Кирхгофа
		}
	}
}

void Equations::update2ndLawEquations(const Loops& loops, const Branches& branches, const Elements& elements)
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

				if (elements[elemIndex].getType() == Type::R)							// если элемент является сопротивлением
				{
					mLeftPart[mFirstLawCount + i][branch] = pointer;					// инициализируем коэффициент, соответствующий i-му уравнению 
																						// по второму закону Кирхгофа и току branch, адресом pointer 
					mLeftPart[mFirstLawCount + i][branch].setSign(branchSign);			// задаем соответствующий знак тока
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

					size_t index = mCurrentSourceCount + voltageSourceIndex;			// вычисляем порядковый номер коэффициента источника напряжения в уравнении
					mRightPart[mFirstLawCount + i][index] = pointer;					// инициализируем коэффициент, соответствующий i-му уравнению
																						// по второму закону Кирхгофа и напряжению index, адресом pointer
					mRightPart[mFirstLawCount + i][index].setSign(branchSign*valueSign);// задаем соответствующий знак c учетом направления ветви в контуре и 
				}																		// ориентации источника напряжения
			}
		}
	}
}

size_t Equations::getVoltageSourceIndex(const size_t elemIndex, const Elements& elements) const
{
	size_t counter = 0;																	// переменная для подсчета источников напряжения

	for (size_t i = 0; i < elemIndex; i++)												// перебор элементов схемы до элемента с номером elemIndex 
	{
		if (elements[i].getType() == Type::E)											// если нашли источник напряжения
		{
			counter++;																	// инкрементируем счетчик
		}
	}
	return counter;																		// возвращаем значение счетчика
}

size_t Equations::getCurrentSourceElemIndex(const size_t currentSourceIndex, const Elements& elements) const
{
	size_t counter = 0;																	// переменная для подсчета источников тока

	for (size_t i = 0; i < elements.size(); i++)										// перебираем элементы схемы
	{
		if (elements[i].getType() == Type::J)											// если элемент является источником тока
		{
			if (++counter == currentSourceIndex + 1)									// инкрементируем счетчик источников и сравниваем с искомым значением
			{
				return i;																// если равны, возвращаем номер элемента
			}
		}
	}
	return elements.size();																// если совпадений не нашлось, возвращаем размер вектора элементов
}
