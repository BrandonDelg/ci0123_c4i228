/**
 * @file main.cpp
 * @brief Creación del cliente y interfaz para interactuar
 */
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "VSocket.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include "Logger.hpp"

#define MAXBUF 1024 /** Tamaño maximo del buffer */
/**
 * @brief Creación del cliente y interfaz para interactuar
 * 
 * @param argc Contador de argumentos
 * @param argv Argumentos, si es más de 1 es https de otra manera http
 *
 * @return Ejecución exitosa en 0
 */
int main() {

   Client cliente(1);
   Logger log("./logs.log");
   const char* service = "1234";

   char buffer[MAXBUF];
   int st;

   bool running = true;

   std::vector<std::string> figuras;

   std::cout << "1) Listar figuras\n";
   std::cout << "2) Pedir figura\n";
   std::cout << "3) Salir\n";
   while (running) {

      std::string comando;
      std::cout << "\nComando: ";
      std::getline(std::cin, comando);

      if (comando == "1") {
         VSocket* client = new Socket('s', true);
         cliente.ClientRequestList(client, service, log);
         std::string response;
         while ((st = client->Read(buffer, MAXBUF-1)) > 0) {
            buffer[st] = 0;
            response += buffer;
         }
         delete client;
         size_t pos = response.find("\r\n\r\n");
         if (pos != std::string::npos) {
            response = response.substr(pos + 4);
         }

         std::cout << "Figuras disponibles:\n";

         std::stringstream ss(response);
         std::string linea;

         figuras.clear();

         while (std::getline(ss, linea)) {
            if (!linea.empty()) {
               figuras.push_back(linea);
               std::cout << "- " << linea << std::endl;
            }
         }
      } else if (comando == "2") {
         std::string figura;
         std::cout << "Nombre figura: ";
         std::getline(std::cin, figura);
         std::cout << "Parte (1,2): ";
         std::string parte;
         std::getline(std::cin, parte);
         int p = std::stoi(parte);
         if (p > 2 || p <= 0) {
            std::cout << "parte incorrecta, ingrese nuevamente" << std::endl;
            continue;
         } else {
            VSocket* client = new Socket('s', true);
            cliente.ClientRequestFigure(client, figura, p, service, log);
            std::string response;
            while ((st = client->Read(buffer, MAXBUF-1)) > 0) {
               buffer[st] = 0;
               response += buffer;
            }
   
            delete client;
            size_t pos = response.find("\r\n\r\n");
            if (pos != std::string::npos) {
               response = response.substr(pos + 4);
            }
            if (response != "") {
               std::cout << "Piezas:\n";
               std::cout << response << std::endl;
            }
         }

      } else if (comando == "3") {
         VSocket* client = new Socket('s', true);
         std::cout << "Saliendo...\n";
         cliente.CloseConnection(client, service);
         running = false;
         delete client;
      } else {
         std::cout << "Comando invalido\n";
      }
   }

   return 0;
}