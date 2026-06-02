#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include "FileSystem.hpp"
#include <mutex>

#include "Socket.hpp"

std::mutex fsMutex;
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
   char buffer[BUFSIZE] = {0};
   int st;

   std::string request;

   st = client->Read(buffer, BUFSIZE - 1);
   if (st <= 0) {
      client->Close();
      delete client;
      return;
   }

   buffer[st] = 0;
   request = buffer;

   std::string response;

   std::cout << "Request:\n" << request << std::endl;

   if (request.rfind("P/", 0) == 0) {
      while (!request.empty() &&
            (request.back() == '\n' || request.back() == '\r' || request.back() == '\0')) {
         request.pop_back();
      }

      if (request == "P/R/dir") {
         std::vector<std::string> figs;
         {
            std::lock_guard<std::mutex> lock(fsMutex);
            figs = fs.getFiguras();
         }

         response = "P/D/";

         for (size_t i = 0; i < figs.size(); ++i) {
            response += figs[i];
            if (i + 1 < figs.size()) {
               response += ",";
            }
         }

      } else if (request.rfind("P/G/", 0) == 0) {
         std::string mensaje = request.substr(4);

         if (mensaje.empty()) {
            response = "P/D/";
         } else {
            std::string figura = mensaje;
            int parte = 0;
\
            size_t slash = mensaje.find("/");
            if (slash != std::string::npos) {
               figura = mensaje.substr(0, slash);

               try {
                  parte = std::stoi(mensaje.substr(slash + 1));
               } catch (...) {
                  parte = 0;
               }
            }

            std::lock_guard<std::mutex> lock(fsMutex);

            if (parte == 1 || parte == 2) {
               std::string piezas = fs.getPiezas(figura, parte);

               if (piezas.empty()) {
                  response = "P/D/";
               } else {
                  response = "P/D/" + piezas;
               }

            } else {
               std::string piezas1 = fs.getPiezas(figura, 1);
               std::string piezas2 = fs.getPiezas(figura, 2);
               // o cosunmir
               // std::string piezas1 = fs.consumirPiezas(figura,1);
               // std::string piezas2 = fs.consumirPiezas(figura,2);

               if (piezas1.empty() && piezas2.empty()) {
                  response = "P/D/";
               } else {
                  response = "P/D/";

                  if (!piezas1.empty()) {
                     response += piezas1;
                  }

                  if (!piezas1.empty() && !piezas2.empty()) {
                     response += "\n";
                  }

                  if (!piezas2.empty()) {
                     response += piezas2;
                  }
               }
            }
         }

      } else if (request == "P/Q/" || request == "P/Q") {
         response = "P/A/";

      } else if (request == "P/K/" || request == "P/K") {
         std::cout << "[SERVIDOR] Kill recibido por protocolo\n";

         response = "P/A/";

         client->Write(response.c_str(), response.length());
         client->Close();
         delete client;

         exit(0);

      } else {
         response = "P/D/";
      }

      client->Write(response.c_str(), response.length());
      client->Close();
      delete client;
      return;
   }

   while (request.find("\r\n\r\n") == std::string::npos) {
      st = client->Read(buffer, BUFSIZE - 1);
      if (st <= 0) break;

      buffer[st] = 0;
      request += buffer;
   }

   if (request.find("GET /shutdown") != std::string::npos) {
      std::cout << "[SERVIDOR] Shutdown recibido\n";

      response =
         "HTTP/1.1 200 OK\r\n"
         "Access-Control-Allow-Origin: *\r\n"
         "Content-Type: text/plain\r\n"
         "\r\n"
         "OK";

      client->Write(response.c_str(), response.length());
      client->Close();
      delete client;

      exit(0);

   } else if (request.find("GET /figuras") != std::string::npos) {
      std::vector<std::string> figs;

      {
         std::lock_guard<std::mutex> lock(fsMutex);
         figs = fs.getFiguras();
      }

      response =
         "HTTP/1.1 200 OK\r\n"
         "Access-Control-Allow-Origin: *\r\n"
         "Content-Type: text/plain\r\n"
         "\r\n";

      for (auto& f : figs) {
         response += f + "\n";
      }

   } else if (request.find("GET /figura/") != std::string::npos) {
      size_t start = request.find("/figura/") + 8;
      size_t end = request.find(" ", start);

      if (end == std::string::npos) {
         response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n";

      } else {
         std::string ruta = request.substr(start, end - start);
         size_t slash = ruta.find("/");

         if (slash == std::string::npos) {
            response =
               "HTTP/1.1 400 Bad Request\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n";

         } else {
            std::string nombre = ruta.substr(0, slash);
            int parte = std::stoi(ruta.substr(slash + 1));

            std::lock_guard<std::mutex> lock(fsMutex);
            std::string piezas = fs.getPiezas(nombre, parte);

            if (piezas.empty()) {
               response =
                  "HTTP/1.1 404 Not Found\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Content-Type: text/plain\r\n"
                  "\r\n";

            } else {
               fs.getPiezas(nombre, parte);
               //fs.consumirPiezas(nombre, parte);

               response =
                  "HTTP/1.1 200 OK\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Content-Type: text/plain\r\n"
                  "\r\n" + piezas;
            }
         }
      }

   } else {
      response =
         "HTTP/1.1 404 Not Found\r\n"
         "Access-Control-Allow-Origin: *\r\n"
         "Content-Type: text/plain\r\n"
         "\r\n";
   }

   client->Write(response.c_str(), response.length());
   client->Close();
   delete client;
}
/**
 *   Create server code
 *      Infinite for
 *         Wait for client conection
 *         Spawn a new thread to handle client request
 *
 **/
int main(int argc, char* argv[]) {
   std::thread * worker;
   VSocket * s1, * client;
   if (argc < 2) {
      std::cerr << "Uso: " << argv[0] << "<ipv6 0|1>\n";
      return 1;
   }
   bool ipv6 = std::stoi(argv[1]);

   s1 = new Socket( 's', ipv6);
   std::cout << "Servidor en " << (ipv6 ? " (IPv6)" : " (IPv4)") << std::endl;
   s1->Bind( PORT );		// Port to access this mirror server
   s1->MarkPassive( 5 );	// Set socket passive and backlog queue to 5 connections

   for( ; ; ) {
      client = s1->AcceptConnection();	 	// Wait for a client connection
      worker = new std::thread( task, client );
   }

}