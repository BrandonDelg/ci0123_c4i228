#include "Servidor.hpp"
#include "Cliente.hpp"
#include <cstring>

Servidor::Servidor(): cliente(nullptr), running(true) {
    pthread_mutex_init(&answersMutex, nullptr);
    pthread_cond_init(&answersCond, nullptr);
}

Servidor::~Servidor() {
    running = false;
    pthread_mutex_destroy(&answersMutex);
    pthread_cond_destroy(&answersCond);
}

void Servidor::Connect(Cliente* client) {
    this->cliente = client;
}


void Servidor::listen() {
    pthread_mutex_t* mutex_client = cliente->getMutex();
    pthread_cond_t* cond_client = cliente->getVC();
    while (running) {
        Message* msg = nullptr;
        
        pthread_mutex_lock(mutex_client);

        while (cliente->getQueue().empty()  && running) {
            pthread_cond_wait(cond_client, mutex_client);
        }
        
        if (!running) {
            pthread_mutex_unlock(mutex_client);
            break;
        }
        if (!cliente->getQueue().empty()) {
            msg = cliente->getQueue().front();
            cliente->getQueue().pop();
        }
        pthread_mutex_unlock(mutex_client);
        if (msg != nullptr) {
            if (msg->type == REQUEST) {
                answer_cliente(msg);
                delete msg;
            } else if (msg->type == CLOSE) {                
                Message* closeMsg = new Message();
                closeMsg->type = CLOSE;
                closeMsg->message_id = msg->message_id;
                strncpy(closeMsg->message, "Servidor cerrando conexión", sizeof(closeMsg->message) - 1);
                closeMsg->message[sizeof(closeMsg->message) - 1] = '\0';
                
                pthread_mutex_lock(&answersMutex);
                respuestas.push(closeMsg);
                pthread_cond_signal(&answersCond);
                pthread_mutex_unlock(&answersMutex);
                
                delete msg;
                Stop();
                break;
            }
        }
    }
    
    std::cout << "Servidor dejó de escuchar" << std::endl;
}

void Servidor::answer_cliente(void* p) {
    Message* msgRequest = (Message*)p;
    Message* msgResponce = new Message();
    std::string respuesta;
    switch (msgRequest->message_id) {
    case 1:
        respuesta = "Brandon Palacios Delgado";
        break;
    case 2:
        respuesta = "Maeva Murcia and Francisco Arroyo";
        break;
    case 3: 
        respuesta = "CI0-123";
        break;
    default:
        break;
    }
    pthread_mutex_lock(&answersMutex);
    strncpy(msgResponce->message, respuesta.c_str(), respuesta.length());  
    msgResponce->type = RESPONSE;
    msgResponce->message_id = msgRequest->message_id;
    respuestas.push(msgResponce);
    pthread_cond_signal(&answersCond);
    pthread_mutex_unlock(&answersMutex);
}

void Servidor::Stop() {
    std::cout << "Deteniendo servidor..." << std::endl;
    running = false;
    if (cliente) {
        pthread_cond_signal(cliente->getVC());
    }
    pthread_cond_signal(&answersCond);
}


pthread_mutex_t* Servidor::getMutex() {
    return &this->answersMutex;
}

pthread_cond_t* Servidor::getVC() {
    return &this->answersCond;
}

std::queue<Message*>& Servidor::getQueue() {
    return this->respuestas;
}
