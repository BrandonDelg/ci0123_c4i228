
class Cliente;
class ServidorPiezas;
struct Message;
class Buzon;

class ServidorIntermedio {
    public:
        ServidorIntermedio(Buzon* b);
        void connect(ServidorPiezas* servp);
        ~ServidorIntermedio();
        void listen();
    private:
        Buzon* buzon;
        ServidorPiezas* serv;
};