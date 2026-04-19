#include "Cliente.hpp"
#include "ServidorIntermedio.hpp"

Cliente::Cliente() {
    this->server = nullptr;
    pthread_mutex_init(&queueMutex, nullptr);
    pthread_cond_init(&queueCond, nullptr);
}

Cliente::~Cliente() {
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueCond);
}

void Cliente::Connect(ServidorIntermedio* serv) {
    this->server = serv;
    this->server->Connect(this);
    std::cout << "[CLIENTE] Conectado al servidor intermedio" << std::endl;
}

void Cliente::send_to_server(std::string msg) {
    pthread_mutex_lock(&queueMutex);
    messageQueue.push(msg);
    pthread_cond_signal(&queueCond);
    pthread_mutex_unlock(&queueMutex);

    if (parser.getTipo(msg) != 2) {
        std::cout << "[CLIENTE] Solicitud enviada" << std::endl;
    } else {
        std::cout << "[CLIENTE] Solicitud enviada -> Exit" << std::endl;
    }
}

bool Cliente::receive_from_server() {
    pthread_mutex_lock(&queueMutex);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;
    while (responseQueue.empty()){
            if (pthread_cond_timedwait(&queueCond, &queueMutex, &ts) == ETIMEDOUT) {
            std::cout << "[CLIENTE] ERROR: Timeout" << std::endl;
            pthread_mutex_unlock(&queueMutex);
            return false;
        }
    }
    std::string msg = responseQueue.front();
    responseQueue.pop();
    pthread_mutex_unlock(&queueMutex);
    int tipo = parser.getTipo(msg);

    if (tipo == 2) {
        return false;
    }
    if (tipo == 15) {
        std::cout << "\n[CLIENTE] Piezas necesarias:\n";
    } else if (tipo == 16 || tipo >= 100) {
        std::cout << "\n[CLIENTE] ERROR:\n";
    } else if (tipo == 11) {
        std::cout << "\n[CLIENTE] Lista de figuras:\n";
    }

    std::string contenido = parser.getMensaje(msg);
    if (contenido == "") {
        contenido = msg.substr(msg.find("|") + 1);
    }

    std::cout << contenido << std::endl;

    return true;
}

std::queue<std::string>& Cliente::getQueue() {
    return messageQueue;
}

pthread_mutex_t* Cliente::getMutex() {
    return &queueMutex;
}

pthread_cond_t* Cliente::getVC() {
    return &queueCond;
}
std::queue<std::string>& Cliente::getResponseQueue() {
    return responseQueue;
}