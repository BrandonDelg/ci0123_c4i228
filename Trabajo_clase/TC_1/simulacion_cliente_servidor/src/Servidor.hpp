#include <iostream>
#include <queue>
class Cliente;
struct Message;
class Servidor {
    public:
        Servidor();
        ~Servidor();
        void Connect(Cliente* client);
        void answer_cliente(void* package);
        void listen();
        void Stop();
        pthread_mutex_t* getMutex();
        pthread_cond_t* getVC();
        std::queue<Message*>& getQueue();
    private:
        Cliente* cliente;
        std::queue<Message*> respuestas;
        bool running;
        pthread_mutex_t answersMutex;
        pthread_cond_t answersCond;


};