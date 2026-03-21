#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include <iostream>
#include <cstring>
#include "Buzon.hpp"

ServidorIntermedio::ServidorIntermedio(Buzon* b) {
    this->buzon = b;
}
void ServidorIntermedio::connect(ServidorPiezas* servp) {
    this->serv = servp;
}
ServidorIntermedio::~ServidorIntermedio() {
    
}


void ServidorIntermedio::listen() {
    myMessage msg;
    while(true) {
        buzon->Recibir(msg, SERVIDOR_TENEDOR);
        if (msg.st == CLIENTE) {
            std::cout << std::endl;
            std::cout << "[ROUTER] Solicitud del cliente recibida! Enviando request al servidor\n";
            msg.type = SERVIDOR_PIEZAS;
            msg.st = REQUEST;
            buzon->Enviar(msg);
        } else if(msg.st == RESPONSE) {
            std::cout << "[ROUTER] Respuesta del servidor enviada al cliente\n" << std::endl;
            std::cout << "[Cliente] Respuesta del servidor:\n" << msg.message << std::endl;
            msg.type = CLIENTE;
            buzon->Enviar(msg);
        } else if(msg.st == CLOSE) {
            std::cout << "[ROUTER] Cerrando sistema\n";
            msg.type = SERVIDOR_PIEZAS;
            buzon->Enviar(msg);
            break;
        }
    }
}
