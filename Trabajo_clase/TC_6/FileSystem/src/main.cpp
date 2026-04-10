#include "FileSystem.hpp"
#include <iostream>

int main() {
    FileSystem fs("filesystem.dat");

    fs.crearFigura("Perro",
        "1\nlego 1x2:4\nlego 2x2:2\nlego 3x2:4\nlego 3x3:2\nlego 3x2:4\nlego 3x3:2\n"
        "2\nlego 3x2:4\nlego 3x3:2\n");

    fs.crearFigura("Gato",
        "1\nlego 5x2:4\n"
        "2\nlego 6x2:4\n");

    auto figs = fs.getFiguras();

    for (auto& f : figs)
        std::cout << "- " << f << std::endl;

    std::cout << "\nPiezas Perro mitad 1:\n";
    std::cout << fs.getPiezas("Perro", 1);
}