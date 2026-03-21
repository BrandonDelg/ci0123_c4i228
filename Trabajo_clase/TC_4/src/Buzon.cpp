#include "Buzon.hpp"

Buzon::Buzon() { 
    key_t key = 0xC41228;
    this->id = msgget(key, IPC_CREAT | 0600);
    if (this->id < 0) { 
        throw std::runtime_error("ERROR");
    }
}   

Buzon::~Buzon() { 
    msgctl(this->id, IPC_RMID, NULL); 
}


ssize_t Buzon::Enviar(const myMessage& msg) {
    ssize_t result = msgsnd(this->id, &msg, sizeof(myMessage) - sizeof(long), 0);

    if (result < 0) {
        throw std::runtime_error("Error enviando mensaje");
    }
    return result;
}

ssize_t Buzon::Recibir(myMessage& msg, long tipo = 0) {
    ssize_t r = msgrcv(this->id, &msg, sizeof(myMessage) - sizeof(long), tipo, 0);
    if (r < 0) {
        throw std::runtime_error("Error recibiendo mensaje");
    }
    return r;
}