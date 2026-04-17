#include <iostream>
#include <thread>
#include <string>
#include "Socket.hpp"

#define PORT 8080
#define BUFSIZE 512

#define SERVER_HOST "::1"
#define SERVER_PORT "1234"

void task(VSocket* client) {
    char buffer[BUFSIZE] = {0};
    int st;
    std::string request;
    while ((st = client->Read(buffer, BUFSIZE - 1)) > 0) {
        buffer[st] = 0;
        request += buffer;

        if (request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    std::cout << "Request recibido del cliente\n";
    std::cout << request << std::endl;

    VSocket* server = new Socket('s', true);
    server->Connect(SERVER_HOST, SERVER_PORT);

    server->Write(request.c_str(), request.length());

    std::string response;
    while ((st = server->Read(buffer, BUFSIZE - 1)) > 0) {
        buffer[st] = 0;
        response += buffer;
    }

    std::cout << "Respuesta del servidor\n";
    std::cout << response << std::endl;

    client->Write(response.c_str(), response.length());

    server->Close();
    delete server;

    client->Close();
    delete client;
}


int main() {
    VSocket* intermediario;
    std::thread* worker;

    intermediario = new Socket('s', true);

    intermediario->Bind(PORT);
    intermediario->MarkPassive(5);

    std::cout << "Intermediario escuchando en puerto " << PORT << "...\n";

    while (true) {
        VSocket* client = intermediario->AcceptConnection();

        worker = new std::thread(task, client);
        worker->detach();
    }

    return 0;
}