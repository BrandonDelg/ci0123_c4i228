/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *   Socket client/server example with threads
  *
  * (Fedora version)
  *
 **/
 
#include <iostream>
#include <thread>
#include "FileSystem.hpp"

#include "Socket.hpp"

#define PORT 1234
#define BUFSIZE 512


/**
 *   Task each new thread will run
 *      Read string from socket
 *      Write it back to client
 *
 **/

FileSystem fs("filesystem.dat");

void task(VSocket* client) {
   char buffer[512] = {0};
   client->Read(buffer, 512);
   std::string request(buffer);
   std::string response;
   std::cout << "Request:\n" << request << std::endl;
   if (request.find("GET /figuras") != std::string::npos) {
      auto figs = fs.getFiguras();
      response = "HTTP/1.1 200 OK\r\n\r\n";
      for (auto& f : figs) {
         response += f + "\n";
      }
      
   } else if (request.find("GET /figura/") != std::string::npos) {
      size_t start = request.find("/figura/") + 8;
      size_t end = request.find(" ", start);
      std::string ruta = request.substr(start, end - start);
      size_t slash = ruta.find("/");
      if (slash == std::string::npos) {
         response = "HTTP/1.1 400 Bad Request\r\n\r\nFormato invalido";
      } else {
         std::string nombre = ruta.substr(0, slash);
         int parte = std::stoi(ruta.substr(slash + 1));
         std::string piezas = fs.getPiezas(nombre, parte);
         response = "HTTP/1.1 200 OK\r\n\r\n";
         response += piezas;
      }
   } else if (request.find("GET /shutdown") != std::string::npos) {
      response = "HTTP/1.1 200 OK\r\n\r\nServidor apagandose...";
      client->Write(response.c_str());
      client->Close();
      exit(0);
   } else {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
   }
   client->Write(response.c_str());
   client->Close();
}
/**
 *   Create server code
 *      Infinite for
 *         Wait for client conection
 *         Spawn a new thread to handle client request
 *
 **/
int main() {
   std::thread * worker;
   VSocket * s1, * client;

   s1 = new Socket( 's');

   s1->Bind( PORT );		// Port to access this mirror server
   s1->MarkPassive( 5 );	// Set socket passive and backlog queue to 5 connections

   for( ; ; ) {
      client = s1->AcceptConnection();	 	// Wait for a client connection
      worker = new std::thread( task, client );
   }

}


