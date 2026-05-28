/**
 * @file ServidorIntermedio.cpp
 * @brief Implementación del servidor intermedio
 */
#include "ServidorIntermedio.hpp"
#include "ServidorPiezas.hpp"
#include "Cliente.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <ctime>

ServidorIntermedio::ServidorIntermedio() {
    id = "SI-default";
    cliente = nullptr;
    servidor = nullptr;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    pthread_mutex_init(&peerMutex, nullptr);
    pthread_cond_init(&peerCond, nullptr);
    pthread_mutex_init(&logMutex, nullptr);
    logFile.open("bitacora.log", std::ios::app);
}

ServidorIntermedio::ServidorIntermedio(const std::string& islaId) {
    id = islaId;
    cliente = nullptr;
    servidor = nullptr;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    pthread_mutex_init(&peerMutex, nullptr);
    pthread_cond_init(&peerCond, nullptr);
    pthread_mutex_init(&logMutex, nullptr);
    logFile.open("bitacora.log", std::ios::app);
}

ServidorIntermedio::~ServidorIntermedio() {
    if (logFile.is_open()) logFile.close();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&peerMutex);
    pthread_cond_destroy(&peerCond);
    pthread_mutex_destroy(&logMutex);
}

std::string ServidorIntermedio::timestamp() {
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

void ServidorIntermedio::logEvento(const std::string& tipo, const std::string& evento, const std::string& detalle) {
    pthread_mutex_lock(&logMutex);
    std::string linea = "[" + timestamp() + "] [" + tipo + "] [" + evento + "]: " + detalle;
    if (logFile.is_open()) {
        logFile << linea << std::endl;
    }
    // std::cout << "[" << id << "] " << linea << std::endl;
    pthread_mutex_unlock(&logMutex);
}

void ServidorIntermedio::Connect(Cliente* c) {
    cliente = c;
}

void ServidorIntermedio::ConnectServidor(ServidorPiezas* s) {
    servidor = s;
    sendToServer("01|");
}

std::string ServidorIntermedio::getId() const {
    return id;
}

void ServidorIntermedio::sendHeartbeat() {
    if (!servidor) {
        logEvento("ERROR", "HEARTBEAT_ERROR", "Puntero SP nulo en sendHeartbeat");
        return;
    }

    logEvento("INFO", "HEARTBEAT_SENT", "Enviando HEARTBEAT a SP");
    sendToServer("03|");
}

void ServidorIntermedio::handleHeartbeat(bool alive){
    if(alive){
        logEvento("INFO", "ALIVE_RECEIVED", "ALIVE recibido");
    } else {
        logEvento("WARN", "HEARTBEAT_NO_RESPONSE", "Sin respuesta ALIVE");
    }
}
void ServidorIntermedio::sendToClient(const std::string& msg) {
    pthread_mutex_lock(&mutex);
    respuestas.push(msg);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

bool ServidorIntermedio::sendToServer(const std::string& msg) {
    if (!servidor) {
        return false;
    }
    pthread_mutex_lock(servidor->getMutex());
    servidor->getQueue().push(msg);
    pthread_cond_signal(servidor->getVC());
    pthread_mutex_unlock(servidor->getMutex());
    return true;
}
void ServidorIntermedio::listen() {
    if (cliente == nullptr) {
        std::cout << "[" << id << "] No hay cliente conectado" << std::endl;
        return;
    }

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

        int tipo = parser.getTipo(msg);

        if (tipo == 90) {
            continue;
        }

        std::cout << "[" << id << "] Mensaje recibido: "
                  << msg << std::endl;

        if (tipo == 1) {
            logEvento("INFO", "REGISTER",
                      "Servidor de piezas registrado correctamente");
            continue;
        }

        if (tipo == 2) {
            std::cout << "[" << id << "] Cerrando intermediario" << std::endl;

            if (servidor) {
                sendToServer("02|");
            }

            break;
        }

        if (tipo == 10) {
            std::cout << "[" << id
                      << "] Cliente solicitó lista de figuras"
                      << std::endl;

            std::string lista;

            for (const std::string& figura : figurasLocales) {
                if (!lista.empty()) {
                    lista += ",";
                }

                lista += figura;
            }

            for (ServidorIntermedio* peer : peers) {
                for (const std::string& figura : peer->figurasLocales) {
                    if (!lista.empty()) {
                        lista += ",";
                    }

                    lista += figura;
                }
            }

            sendToClient("11|figura=" + lista + ";");
            continue;
        }

        if (tipo == 14) {
            std::string figura = parser.getFigura(msg);
            int mitad = parser.getMitad(msg);

            if (mitad < 1 || mitad > 2) {
                mitad = 1;
            }

            std::cout << "[" << id
                      << "] Cliente pidió figura "
                      << figura
                      << " mitad "
                      << mitad
                      << std::endl;

            if (figurasLocales.find(figura) != figurasLocales.end()) {
                std::cout << "[" << id
                          << "] Figura local. Consultando servidor local"
                          << std::endl;

                std::string solicitud =
                    "14|figura=" + figura +
                    ";mitad=" + std::to_string(mitad) + ";";

                if (!sendToServer(solicitud)) {
                    sendToClient(
                        "102|mensaje=Servidor local no disponible;");
                }

                continue;
            }

            std::cout << "[" << id
                      << "] Figura no local. Consultando peers"
                      << std::endl;

            std::string respuesta =
                consultarPeer(figura, mitad);

            sendToClient(respuesta);

            continue;
        }

        sendToClient("107|mensaje=Tipo no soportado;");
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

Cliente* ServidorIntermedio::getCliente(){
    return cliente;
};

std::string ServidorIntermedio::atenderRequestIntermediario(const std::string& msg) {
    std::string figura = parser.getFigura(msg);
    int mitad = parser.getMitad(msg);

    std::cout << "[" << id << "] INTERMEDIARY_REQUEST recibido: "
              << figura << " mitad " << mitad << std::endl;

    if (figurasLocales.find(figura) == figurasLocales.end()) {
        std::cout << "[" << id << "] No tengo la figura "
                  << figura << std::endl;

        return "104|figura=" + figura + ";mensaje=Figura no encontrada;";
    }

    std::cout << "[" << id << "] Sí tengo "
              << figura << ", consultando mi servidor local" << std::endl;

    std::string solicitud =
        "14|figura=" + figura +
        ";mitad=" + std::to_string(mitad) + ";";

    if (!sendToServer(solicitud)) {
        return "102|mensaje=Servidor remoto no disponible;";
    }

    pthread_mutex_lock(&mutex);

    while (respuestas.empty()) {
        pthread_cond_wait(&cond, &mutex);
    }

    std::string respuesta = respuestas.front();
    respuestas.pop();

    pthread_mutex_unlock(&mutex);

    int tipo = parser.getTipo(respuesta);

    if (tipo == 15) {
        std::cout << "[" << id << "] INTERMEDIARY_RESPONSE enviado con figura "
                  << figura << std::endl;

        return "200|" + respuesta.substr(respuesta.find('|') + 1);
    }

    return "104|figura=" + figura + ";mensaje=Figura no encontrada;";
}

std::string ServidorIntermedio::consultarPeer(const std::string& figura, int mitad) {
    std::cout << "[" << id << "] Buscando "
              << figura << " en otros intermediarios" << std::endl;

    for (ServidorIntermedio* peer : peers) {
        std::cout << "[" << id << "] Preguntando a "
                  << peer->getId() << std::endl;

        std::string request =
            "300|figura=" + figura +
            ";mitad=" + std::to_string(mitad) +
            ";source=" + id + ";";

        std::string respuesta = peer->atenderRequestIntermediario(request);

        if (parser.getTipo(respuesta) == 200) {
            std::cout << "[" << id << "] Respuesta recibida desde "
                      << peer->getId() << std::endl;

            return "15|" + respuesta.substr(respuesta.find('|') + 1);
        }
    }

    std::cout << "[" << id << "] Ningún intermediario tiene "
              << figura << std::endl;

    return "104|figura=" + figura + ";mensaje=Figura no encontrada;";
}


void ServidorIntermedio::registrarFiguraLocal(const std::string& figura) {
    figurasLocales.insert(figura);

    std::cout << "[" << id << "] Figura local registrada: "
              << figura << std::endl;

    logEvento("INFO", "FIGURA_LOCAL", figura);
}

void ServidorIntermedio::conectarIntermediario(ServidorIntermedio* peer) {
    if (!peer || peer == this) {
        return;
    }

    peers.push_back(peer);

    std::cout << "[" << id << "] HANDSHAKE enviado a "
              << peer->getId() << std::endl;

    peer->recibirHandshake(this, figurasLocales);
    recibirHandshake(peer, peer->figurasLocales);
}

void ServidorIntermedio::recibirHandshake(ServidorIntermedio* peer,const std::set<std::string>& figurasPeer) {
    std::cout << "[" << id << "] HANDSHAKE recibido desde "
              << peer->getId() << std::endl;

    for (const std::string& figura : figurasPeer) {
        std::cout << "[" << id << "] Figura remota conocida: "
                  << figura << " -> " << peer->getId() << std::endl;
    }

    logEvento("INFO", "HANDSHAKE_RECEIVED",
              "Handshake con " + peer->getId());
}