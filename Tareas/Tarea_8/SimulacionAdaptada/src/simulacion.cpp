#include "Cliente.hpp"
#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include <pthread.h>
#include <iostream>
#include <sstream>

void* router_thread(void* arg) {
    ((ServidorIntermedio*)arg)->listen();
    return nullptr;
}

void* piezas_thread(void* arg) {
    ((ServidorPiezas*)arg)->listen();
    return nullptr;
}

int main() {
    Cliente cliente;
    ServidorIntermedio router;
    ServidorPiezas piezas;

    cliente.Connect(&router);
    router.ConnectServidor(&piezas);
    piezas.Connect(&router);

    pthread_t t1, t2;
    pthread_create(&t1, nullptr, router_thread, &router);
    pthread_create(&t2, nullptr, piezas_thread, &piezas);

    std::string input;

    while (true) {
        std::cout << "\n1) Lista\n2) Figura\n3) Salir\nOpcion: ";
        std::getline(std::cin, input);
        std::string msg;
        if (input == "1") {
            msg = "10|";
        } else if (input == "2") {
            std::string figura;
            int mitad;

            std::cout << "Figura y mitad: ";
            std::cin >> figura >> mitad;
            std::cin.ignore();

            msg = "14|figura=" + figura + ";mitad=" + std::to_string(mitad);
        } else if (input == "3") {
            msg = "02|";
        } else {
            continue;
        }
        cliente.send_to_server(msg);
        if (msg == "02|") {
            break;
        }
        cliente.receive_from_server();
    }

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    return 0;
}