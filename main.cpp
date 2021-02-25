#include <iostream>
#include <chrono>
#include <fstream>
#include <ctime>

#include "circuit-solver/circuit.h"

using namespace std;

int main()
{
    CS::Circuit simple_circuit, complex_circuit, middle_circuit;

    simple_circuit.add({CS::Element::Type::R, 2, -1, -1});
    simple_circuit.add({CS::Element::Type::R, 6, 1, -1});
    simple_circuit.add({CS::Element::Type::E, 2, 0, -1});
    simple_circuit.add({CS::Element::Type::J, 2, 5, 3});
    simple_circuit.add({CS::Element::Type::R, 8, 1, 5});

    middle_circuit.add({CS::Element::Type::R, 1, -1, -1});
    middle_circuit.add({CS::Element::Type::E, 2, 1, -1});
    middle_circuit.add({CS::Element::Type::E, 3, -1, 3});
    middle_circuit.add({CS::Element::Type::R, 4, 0, 4});
    middle_circuit.add({CS::Element::Type::R, 5, 3, -1});
    middle_circuit.add({CS::Element::Type::R, 6, 4, -1});
    middle_circuit.add({CS::Element::Type::J, 7, 11, 9});
    middle_circuit.add({CS::Element::Type::E, 8, -1, 0});
    middle_circuit.add({CS::Element::Type::E, 9, -1, 4});
    middle_circuit.add({CS::Element::Type::J, 10, 11, -1});
    middle_circuit.add({CS::Element::Type::R, 11, 14, 16});
    middle_circuit.add({CS::Element::Type::R, 12, 16, 19});

    complex_circuit.add({CS::Element::Type::E, 1, -1, -1});
    complex_circuit.add({CS::Element::Type::R, 1, 1, -1});
    complex_circuit.add({CS::Element::Type::R, 2, 3, -1});
    complex_circuit.add({CS::Element::Type::R, 3, 5, -1});

    complex_circuit.add({CS::Element::Type::R, 4, -1, 0});
    complex_circuit.add({CS::Element::Type::E, 2, -1, 1});
    complex_circuit.add({CS::Element::Type::J, 1, -1, 3});
    complex_circuit.add({CS::Element::Type::J, 2, -1, 5});
    complex_circuit.add({CS::Element::Type::E, 3, -1, 7});

    complex_circuit.add({CS::Element::Type::R, 5, 8, 10});
    complex_circuit.add({CS::Element::Type::R, 6, 10, 12});
    complex_circuit.add({CS::Element::Type::R, 7, 12, 14});
    complex_circuit.add({CS::Element::Type::R, 8, 14, 16});

    complex_circuit.add({CS::Element::Type::E, 4, -1, 8});
    complex_circuit.add({CS::Element::Type::R, 9, -1, 10});
    complex_circuit.add({CS::Element::Type::J, 3, 12, -1});
    complex_circuit.add({CS::Element::Type::J, 4, 14, -1});
    complex_circuit.add({CS::Element::Type::E, 5, 16, -1});

    complex_circuit.add({CS::Element::Type::R, 10, 26, 28});
    complex_circuit.add({CS::Element::Type::R, 11, 28, 31});
    complex_circuit.add({CS::Element::Type::R, 12, 31, 33});
    complex_circuit.add({CS::Element::Type::R, 13, 33, 35});

    complex_circuit.add({CS::Element::Type::E, 6, 36, -1});
    complex_circuit.add({CS::Element::Type::E, 7, 38, -1});
    complex_circuit.add({CS::Element::Type::E, 8, 40, -1});
    complex_circuit.add({CS::Element::Type::E, 9, 42, -1});
    complex_circuit.add({CS::Element::Type::E, 10, 43, -1});

    complex_circuit.add({CS::Element::Type::R, 14, 45, 47});
    complex_circuit.add({CS::Element::Type::R, 15, 47, 49});
    complex_circuit.add({CS::Element::Type::R, 16, 49, 51});
    complex_circuit.add({CS::Element::Type::R, 17, 51, 53});

    complex_circuit.add({CS::Element::Type::R, 18, 45, -1});
    complex_circuit.add({CS::Element::Type::R, 19, 47, -1});
    complex_circuit.add({CS::Element::Type::R, 20, 49, -1});
    complex_circuit.add({CS::Element::Type::R, 21, 51, -1});
    complex_circuit.add({CS::Element::Type::R, 22, 53, -1});

    complex_circuit.add({CS::Element::Type::E, 11, 63, 65});
    complex_circuit.add({CS::Element::Type::E, 12, 67, 65});
    complex_circuit.add({CS::Element::Type::E, 13, 67, 69});
    complex_circuit.add({CS::Element::Type::E, 14, 71, 69});

    simple_circuit.update();
    middle_circuit.update();
    complex_circuit.update();

    ofstream ofs_simple("simple_circuit.txt");
    ofstream ofs_middle("middle_circuit.txt");
    ofstream ofs_complex("complex_circuit.txt");

    ofs_simple << simple_circuit.toString();
    ofs_middle << middle_circuit.toString();
    ofs_complex << complex_circuit.toString();

    return 0;
}