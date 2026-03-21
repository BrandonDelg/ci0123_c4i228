/**
  *  C++ class to encapsulate Unix message passing intrinsic structures and system calls
  *
 **/

#include <unistd.h>	// pid_t definition
#include <sys/types.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <cstring>
#define MaxParticipantes 100

// #define KEY 0xC4I228	
enum states {
   CLIENTE = 1,
   SERVIDOR_PIEZAS,
   SERVIDOR_TENEDOR,
   REQUEST,
   RESPONSE,
   CLOSE
};
struct myMessage {
   long type;
   states st;
   char message[256];
};

class Buzon {
   public:
      Buzon();
      ~Buzon();
      // ssize_t Enviar(const char * message, long tipo );
      ssize_t Enviar(const myMessage& msg);
      ssize_t Recibir(myMessage& msg, long type);
   private:
      int id;
      pid_t owner;
};