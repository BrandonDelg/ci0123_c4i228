class ServidorIntermedio;
struct myMessage;
class Buzon;


class ServidorPiezas {
    public:
        ServidorPiezas(Buzon* b);
        ~ServidorPiezas();
        void listen();
        void procesarSolicitud(myMessage msg);
    private:
        bool running;
        Buzon* buzon;
    };