#include "Cliente.hpp"
#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include "Parser.hpp"

#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

struct HeartbeatArgs {
    ServidorIntermedio* router;
    ServidorPiezas* piezas;
    bool* running;
};

void* router_thread(void* arg) {
    ((ServidorIntermedio*)arg)->listen();
    return nullptr;
}

void* piezas_thread(void* arg) {
    ((ServidorPiezas*)arg)->listen();
    return nullptr;
}

void* heartbeat_thread(void* arg) {
    HeartbeatArgs* heartbeatArgs = (HeartbeatArgs*)arg;

    while (*(heartbeatArgs->running)) {
        sleep(5);

        if (*(heartbeatArgs->running)) {
            heartbeatArgs->router->sendHeartbeat();
        }
    }

    return nullptr;
}

int main() {
    Cliente cliente;

    ServidorIntermedio router1("SI-01");
    ServidorIntermedio router2("SI-02");

    ServidorPiezas piezas1("SP-01");
    ServidorPiezas piezas2("SP-02");

    Parser parser;

    cliente.Connect(&router1);

    router1.ConnectServidor(&piezas1);
    piezas1.Connect(&router1);

    router2.ConnectServidor(&piezas2);
    piezas2.Connect(&router2);

    router1.registrarFiguraLocal("Perro");
    router1.registrarFiguraLocal("Gato");
    router1.registrarFiguraLocal("Ballena");

    router2.registrarFiguraLocal("Oveja");
    router2.registrarFiguraLocal("Carro");

    router1.conectarIntermediario(&router2);

    bool running = true;

    HeartbeatArgs heartbeatArgs1 = { &router1, &piezas1, &running };
    HeartbeatArgs heartbeatArgs2 = { &router2, &piezas2, &running };

    pthread_t routerT1;
    pthread_t piezasT1;
    pthread_t piezasT2;
    pthread_t heartbeatT1;
    pthread_t heartbeatT2;

    pthread_create(&routerT1, nullptr, router_thread, &router1);

    pthread_create(&piezasT1, nullptr, piezas_thread, &piezas1);
    pthread_create(&piezasT2, nullptr, piezas_thread, &piezas2);

    pthread_create(&heartbeatT1, nullptr, heartbeat_thread, &heartbeatArgs1);
    pthread_create(&heartbeatT2, nullptr, heartbeat_thread, &heartbeatArgs2);

    sleep(1);

    std::string input;

    while (running) {
        std::cout << "1) Solicitar lista de figuras\n";
        std::cout << "2) Solicitar figura\n";
        std::cout << "3) Salir\n";
        std::cout << "\nOpcion: ";

        std::getline(std::cin, input);

        std::string msg = "";

        if (input == "1") {
            msg = "10|";
            std::cout << "[CLIENTE] Solicita lista de figuras\n";

        } else if (input == "2") {
            std::string linea;

            std::cout << "Ingrese figura y mitad (ej: Perro 1): ";
            std::getline(std::cin, linea);

            std::istringstream iss(linea);

            std::string figura;
            int mitad;

            if (!(iss >> figura >> mitad)) {
                std::cout << "Formato inválido. Use: Perro 1\n";
                continue;
            }

            msg =
                "14|figura=" + figura +
                ";mitad=" + std::to_string(mitad) + ";";

            std::cout << "[CLIENTE] Solicita figura: "
                      << figura << " mitad: " << mitad << "\n";

        } else if (input == "3") {
            msg = "02|";
            running = false;
            cliente.running = false;

            std::cout << "[CLIENTE] Cerrando conexión\n";

        } else {
            std::cout << "Opcion inválida\n";
            continue;
        }

        cliente.send_to_server(msg);

        int tipoMsg = parser.getTipo(msg);

        if (tipoMsg == 2) {
            break;
        }

        cliente.receive_from_server();
    }

    running = false;

    piezas1.Stop();
    piezas2.Stop();

    pthread_join(heartbeatT1, nullptr);
    pthread_join(heartbeatT2, nullptr);

    pthread_join(routerT1, nullptr);

    pthread_join(piezasT1, nullptr);
    pthread_join(piezasT2, nullptr);

    return 0;
}