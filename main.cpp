#include <iostream>
#include <chrono>
#include <fstream>
#include <ctime>

#include "circuit-solver/circuit.h"

using namespace std;

int main()
{
    CS::Circuit simple_circuit, complex_circuit, middle_circuit;

    ifstream ifs_simple("../circuit_simple.txt");
    ifstream ifs_middle("../circuit_middle.txt");
    ifstream ifs_complex("../circuit_complex.txt");

    ifs_simple >> simple_circuit;
    ifs_middle >> middle_circuit;
    ifs_complex >> complex_circuit;

    simple_circuit.update();
    middle_circuit.update();
    complex_circuit.update();

    ofstream ofs_simple("circuit_simple_results.txt");
    ofstream ofs_middle("circuit_middle_results.txt");
    ofstream ofs_complex("circuit_complex_results.txt");

    ofs_simple << simple_circuit;
    ofs_middle << middle_circuit;
    ofs_complex << complex_circuit;

    return 0;
}