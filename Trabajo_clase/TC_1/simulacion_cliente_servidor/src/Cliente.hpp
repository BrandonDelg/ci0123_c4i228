#include <iostream>
#include <queue>
#include <pthread.h>
enum Type {
    CONNECT,
    REQUEST,
    RESPONSE,
    CLOSE
};
struct Message {
    Type type;
    int message_id;
    char message[256];
};

class Servidor;

class Cliente {
    public:
        Cliente();
        ~Cliente();
        void Connect(Servidor* serv);
        void send_to_server(void* package);
        bool receive_from_server();
        std::queue<Message*>& getQueue();
        pthread_cond_t* getVC();
        pthread_mutex_t* getMutex();
    private:
        Servidor* server;
        std::queue<Message*> messageQueue;
        pthread_mutex_t queueMutex;
        pthread_cond_t queueCond;
};