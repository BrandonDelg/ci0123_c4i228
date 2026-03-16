#include "Servidor.hpp"
#include "Cliente.hpp"
#include <cstring>

struct ThreadData {
    Cliente* cliente;
    Message* msg;
    int id;
};

struct ServerData {
    Servidor* server;
};

void* enviar_mensaje(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->cliente->send_to_server(data->msg);
    
    delete data;
    return nullptr;
}

void* listen_server(void* arg) {
    ServerData* data = (ServerData*)arg;
    data->server->listen();
    delete data;
    return nullptr;
}

int main() {
    Cliente* cliente = new Cliente();
    Servidor* server = new Servidor();
    cliente->Connect(server);

    std::vector<pthread_t> hilos;
    
    ServerData* sdata = new ServerData();
    sdata->server = server;
    pthread_t serverThread;
    pthread_create(&serverThread, nullptr, listen_server, sdata);
    
    int option = 0;
    int threadCount = 0;
    bool running = true;
    while (running) {
        Message* msg = new Message();
        std::cout << "\n--- Menu ---" << std::endl;
        std::cout << "1) GET student's name" << std::endl;
        std::cout << "2) GET professors's names" << std::endl;
        std::cout << "3) GET course acronyms" << std::endl;
        std::cout << "4) Exit" << std::endl;
        std::cout << "Opción: ";
        std::cin >> option;
        
        if (option >= 1 && option <= 4) {
            msg->message_id = option;
            msg->type = (option == 4) ? CLOSE : REQUEST;
            
            std::string mensaje;
            switch(option) {
                case 1: 
                    mensaje = "GET student's name";
                break;
                case 2: 
                    mensaje = "GET professors's names";
                break;
                case 3: 
                    mensaje = "GET course acronyms";
                    break;
                case 4: 
                    mensaje = "Salir";
                break;
            }
            strncpy(msg->message, mensaje.c_str(), sizeof(msg->message) - 1);
            msg->message[sizeof(msg->message) - 1] = '\0';
            
            ThreadData* data = new ThreadData();
            data->cliente = cliente;
            data->msg = msg;
            data->id = threadCount++;
            
            pthread_t hilo;
            pthread_create(&hilo, nullptr, enviar_mensaje, data);
            hilos.push_back(hilo);
            if (!cliente->receive_from_server()) {
                running = false;
            }
        }
    }
    for (pthread_t& hilo : hilos) {
        pthread_join(hilo, nullptr);
    }
    
    pthread_join(serverThread, nullptr);
    
    delete server;
    delete cliente;
    
    return 0;
}