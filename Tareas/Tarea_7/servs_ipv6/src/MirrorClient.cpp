#include <stdio.h>
#include <cstring>
#include <string>
#include "Socket.hpp"

#define PORT 1234
#define BUFSIZE 512

int main(int argc, char **argv) {
   VSocket *s;
   char buffer[BUFSIZE];

   bool useIPv6 = false;
   const char* message = "Hello world 2026 ...";
   if (argc > 1) {
      message = argv[1];
   }
   if (argc > 2 && std::string(argv[2]) == "ipv6") {
      useIPv6 = true;
   }
   s = new Socket('s', useIPv6);
   memset(buffer, 0, BUFSIZE);
   const char* host = useIPv6 ? "::1" : "127.0.0.1";
   s->TryToConnect(host, PORT);
   s->Write(message);
   s->Read(buffer, BUFSIZE);
   printf("%s", buffer);

   delete s;
   return 0;
}