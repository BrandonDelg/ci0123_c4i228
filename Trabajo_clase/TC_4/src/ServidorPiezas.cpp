#include "ServidorPiezas.hpp"
#include "ServidorIntermedio.hpp"
#include <cstring>
#include <iostream>
#include "Buzon.hpp"

ServidorPiezas::ServidorPiezas(Buzon* b) {
    running = true;
    buzon = b;
}

ServidorPiezas::~ServidorPiezas() {
   //delete buzon;
}


void ServidorPiezas::listen() {

    myMessage msg;

    while (running) {
        buzon->Recibir(msg, SERVIDOR_PIEZAS);
        if (msg.st == REQUEST) {
            std::cout << "[SERVIDOR PIEZAS] solicitud recibida: " << msg.message << std::endl;
            procesarSolicitud(msg);
        } else if (msg.st == CLOSE) {
            std::cout << "[SERVIDOR PIEZAS] cerrando servidor" << std::endl;
            running = false;
        }
    }
}
void ServidorPiezas::procesarSolicitud(myMessage msg) {
    std::string piezas;
    std::string cadena = msg.message;
    if (strcmp(msg.message, "see_figures") == 0) {
        std::string figures =
        "Listado de Figuras en el servidor :)\n"
        "- Perro \n"
        "- Gato \n"
        "- Ballena \n"
        "- Oveja \n";
        piezas = figures;
    } else if (cadena.find("GET ") == 0) {
        std::string figura = cadena.substr(4);
        std::cout << "[SERVIDOR PIEZAS] Buscando piezas de " << figura << std::endl;
        if (figura ==  "figure_Perro") {
            piezas = 
            "lego 1x2 : 4\n"
            "lego 2x2 : 2\n"
            "lego 1x4 : 3\n";
        } else if (figura == "figure_Gato") {
           piezas = 
            "lego 5x2 : 4\n"
            "lego 4x2 : 2\n";
        } else if (figura ==  "figure_Ballena") {
            piezas = 
            "lego 3x2 : 4\n"
            "lego 2x2 : 2\n"
            "lego 1x5 : 3\n"
            "lego 1x2 : 4\n"
            "lego 4x2 : 2\n"
            "lego 3x4 : 3\n";
        } else if (figura ==  "figure_Oveja") {
            piezas = 
            "lego 2x2 : 2\n"
            "lego 1x5 : 3\n"
            "lego 1x2 : 4\n";
        } else {
            piezas = "La figura ingresada no es valida!\n";
        }
    } else {
        piezas = "Comando Invalido!\n";
    }
    myMessage resp;
    resp.type = SERVIDOR_TENEDOR;
    resp.st = RESPONSE;
    strncpy(resp.message, piezas.c_str(), sizeof(resp.message) - 1);
    resp.message[sizeof(resp.message)-1] = '\0';
    buzon->Enviar(resp);
    std::cout << "[SERVIDOR PIEZAS] Enviando informacion al router" << std::endl;
}
