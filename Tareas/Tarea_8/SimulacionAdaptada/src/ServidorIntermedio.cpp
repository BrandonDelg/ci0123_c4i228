#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include "Cliente.hpp"
#include <iostream>

ServidorIntermedio::ServidorIntermedio() {
    cliente = nullptr;
    servidor = nullptr;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
}

ServidorIntermedio::~ServidorIntermedio() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void ServidorIntermedio::Connect(Cliente* c) {
    cliente = c;
}

void ServidorIntermedio::ConnectServidor(ServidorPiezas* s) {
    servidor = s;
}

void ServidorIntermedio::listen() {
    pthread_mutex_t* mutex_client = cliente->getMutex();
    pthread_cond_t* cond_client = cliente->getVC();

    while (true) {
        pthread_mutex_lock(mutex_client);
        while (cliente->getQueue().empty()) {
            pthread_cond_wait(cond_client, mutex_client);
        }

        std::string msg = cliente->getQueue().front();
        cliente->getQueue().pop();
        pthread_mutex_unlock(mutex_client);

        if (msg.find("|") == std::string::npos) {
            std::string resp = "101|mensaje=Formato invalido";

            pthread_mutex_lock(cliente->getMutex());
            cliente->getResponseQueue().push(resp);
            pthread_cond_signal(cliente->getVC());
            pthread_mutex_unlock(cliente->getMutex());
            continue;
        }
        if (parser.getTipo(msg) == 2) {
            std::cout << "[ROUTER] Cerrando sistema" << std::endl;

            pthread_mutex_lock(servidor->getMutex());
            servidor->getQueue().push("02|");
            pthread_cond_signal(servidor->getVC());
            pthread_mutex_unlock(servidor->getMutex());
            break;
        }

        std::cout << "[ROUTER] Enviando solicitud al servidor de piezas" << std::endl;

        pthread_mutex_lock(servidor->getMutex());
        servidor->getQueue().push(msg);
        pthread_cond_signal(servidor->getVC());
        pthread_mutex_unlock(servidor->getMutex());

        pthread_mutex_lock(&mutex);
        while (respuestas.empty()) {
            pthread_cond_wait(&cond, &mutex);
        }

        std::string resp = respuestas.front();
        respuestas.pop();
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(cliente->getMutex());
        cliente->getResponseQueue().push(resp);
        pthread_cond_signal(cliente->getVC());
        pthread_mutex_unlock(cliente->getMutex());

        std::cout << "[ROUTER] Respuesta enviada al cliente" << std::endl;
    }
}

pthread_mutex_t* ServidorIntermedio::getMutex() {
    return &mutex;
}

pthread_cond_t* ServidorIntermedio::getVC() {
    return &cond;
}

std::queue<std::string>& ServidorIntermedio::getQueue() {
    return respuestas;
}