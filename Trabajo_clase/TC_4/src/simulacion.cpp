#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include "Buzon.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

int main() {

    Buzon buzon;

    ServidorIntermedio router(&buzon);
    ServidorPiezas piezas(&buzon);

    pid_t pid1 = fork();

    if (pid1 == 0) {
        piezas.listen();
        return 0;
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
        router.listen();
        return 0;
    }

    myMessage msg;
    bool running = true;
    std::string op;
    
    std::cout << "Use: see_figures\n";
    std::cout << "Use: GET figure_name\n";
    std::cout << "Use: Exit\n";
    while (running) {
        std::getline(std::cin, op);
        if (op == "Exit") {
            msg.st = CLOSE;
            running = false;
        } else {
            msg.st = CLIENTE;
        }

        msg.type = SERVIDOR_TENEDOR;
        strcpy(msg.message, op.c_str());

        buzon.Enviar(msg);

        if (msg.st == CLOSE) {
            break;
        }
    }
    return 0;
}