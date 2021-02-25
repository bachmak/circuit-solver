#pragma once

#include "circuit-solver/common.h"
#include "circuit-solver/circuit_configuration/elements.h"

namespace CS
{
// класс для представления матрицы соединений пинов
//////////////////////////////////////////////////////////////////////////////////////////
class PinMatrix
{
public:
    void update(const Elements& elements);                                              // метод обновления матрицы
    const size_t size() const;                                                          // геттер размера матрицы
    const BoolVect& operator[] (size_t index) const;                                    // геттер строки матрицы (одномерного вектора)
    std::string toString() const;                                                       // метод получения строкового представления матрицы

private:
    BoolMatr matrix;                                                                    // логическая двумерная матрица для хранения информации о соединениях пинов
};
//////////////////////////////////////////////////////////////////////////////////////////


// метод обновления матрицы пинов
// треугольная матрица пинов строится таким образом, что при matrix[i][j] == 1
// i-ый пин напрямую подключен к j-ому пину; иначе прямое соединение отсутствует 
// принимает на вход список элементов схемы 
inline void PinMatrix::update(const Elements& elements)
{
    size_t pinCount = elements.size() * 2;                                              // считаем пины по количеству элементов

    matrix = BoolMatr(pinCount);                                                        // создаем двумерный вектор размера pinCount
    for (size_t i = 0; i < pinCount; i++)                                               // создаем треугольную матрицу нижу главной диагонали, заполненную нулями
    {
        matrix[i] = BoolVect(i, false);
    }
                                                                                        // первая часть: соединяем то, что соединено напрямую
    for (size_t i = 0; i < pinCount; i++)                                               // перебираем по пинам схемы (их ровно в 2 раза больше, чем элементов)
    {    
        if (elements[i / 2].getLinkedPin(i % 2) >= 0)                                   // если i-ый пин соединен с каким-нибудь другим пином (-1, если не соединен)
        {                                                                               // целая часть от деления на два - номер элемента, остаток - номер пина элемента
            matrix[i][elements[i / 2].getLinkedPin(i % 2)] = true;                      // отмечаем соединение пинов в матрице соединений 
        }
    }
                                                                                        // вторая часть: соединяем то, что соединено через другие пины
    for (size_t i = 0; i < pinCount; i++)                                               // перебираем матрицу пинов по строкам
    {
        for (size_t j = 0; j < i; j++)                                                  // перебираем матрицу пинов по столбцам (смотрим ниже главной диагонали)
        {
            if (matrix[i][j])                                                           // если i-ый пин соединен с j-ым
            {
                for (size_t k = 0; k < j; k++)                                          // перебираем до главной диагонали строку j, пин из которой соединен с i-ым  
                {                                                                       // ищем пины, которые через j-ый соединены с i-ым
                    if (matrix[j][k])                                                   // если j-ый пин соединен с k-ым
                    {
                        matrix[i][k] = true;                                            // то i-ый пин так же соединен с k-ым
                    }
                }
                for (size_t k = i + 1; k < pinCount; k++)                               // перебираем от главноц диагонали столбец j, пин из которой соединен с i-ым
                {                                                                       // ищем пины, которые через j-ый соединены с i-ым
                    if (matrix[k][j])                                                   // если k-ый пин соединен с j-ым
                    {
                        matrix[k][i] = true;                                            // то k-ый пин так же соединен с i-ым
                    }                                                                   // т.е. дополняем отношение до транзитивного, каким оно и должно быть
                }
            }
        }
    }
}

// геттер размера матрицы
inline const size_t PinMatrix::size() const
{
    return matrix.size();
}

// геттер строки матрицы (одномерного вектора)
inline const BoolVect& PinMatrix::operator[](size_t index) const
{
    return matrix[index];
}

// метод получения строкового представления матрицы
inline std::string PinMatrix::toString() const
{
    using namespace std;

    stringstream stream;

    stream << "Матрица соединений пинов:\n\n";

    stream << setw(6) << ' ';

    for (size_t i = 0; i < matrix.size(); i++)
    {
        stream << setw(4) << i << ' ';
    }

    stream << "\n\n";

    for (size_t i = 0; i < matrix.size(); i++)
    {
        stream << left << setw(4) << i << right << ": ";

        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            stream << setw(4) << matrix[i][j] << ' ';
        }

        stream << "\n\n";
    }

    return stream.str();
}

} // namespace CS
