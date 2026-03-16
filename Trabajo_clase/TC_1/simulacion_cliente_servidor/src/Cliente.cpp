#include "Cliente.hpp"
#include "Servidor.hpp"

Cliente::Cliente() {
    this->server = nullptr;
    pthread_mutex_init(&queueMutex, nullptr);
    pthread_cond_init(&queueCond, nullptr);

}

Cliente::~Cliente() {
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueCond);

}

void Cliente::Connect(Servidor* serv) {
    this->server = serv;
    this->server->Connect(this);
    std::cout << "Conectado al servidor!" << std::endl;
}

void Cliente::send_to_server(void* package) {
    Message* msg = (Message*) package;
    pthread_mutex_lock(&queueMutex);
    this->messageQueue.push(msg);
    pthread_cond_signal(&this->queueCond);
    pthread_mutex_unlock(&queueMutex);
    std::cout << "Enviando mensaje al server :)" << std::endl;
}

bool Cliente::receive_from_server() {
    if (!server) return false; 
    
    pthread_mutex_t* mutex_server = server->getMutex();
    pthread_cond_t* cond_server = server->getVC();
    
    pthread_mutex_lock(mutex_server);
    
    while (server->getQueue().empty()) {
        pthread_cond_wait(cond_server, mutex_server);
    }
    
    Message* answersFromServer = server->getQueue().front();
    server->getQueue().pop();
    
    pthread_mutex_unlock(mutex_server);
    
    if (answersFromServer->type == CLOSE) {
        delete answersFromServer;
        return false;
    } else if (answersFromServer->type == RESPONSE) {
        std::cout << "Respuesta del servidor: " << answersFromServer->message << std::endl;
        delete answersFromServer;
        return true;
    } 
    return true;
    
}

std::queue<Message*>& Cliente::getQueue() {
    return this->messageQueue;
}

pthread_mutex_t* Cliente::getMutex() {
    return &this->queueMutex;
}

pthread_cond_t* Cliente::getVC() {
    return &this->queueCond;
}