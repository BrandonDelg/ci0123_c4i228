#ifndef INTERMEDIARIO_HPP
#define INTERMEDIARIO_HPP
#include <iostream>
#include <thread>
#include <string>
#include "Socket.hpp"
#include <map>
#include <list>
#include <mutex>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstdint>


//#define BROADCAST_ISLA "172.16.123.95"
#define BROADCAST_ISLA "192.168.0.255"
#define BUFSIZE 512

#define DISCOVERY_PORT_INTERMEDIARIO 9092
#define DISCOVERY_PORT_SERVIDOR      9093

#define TP_JOIN_PORT      3030   // UDP
#define TP_TCP_PORT       3031   // TCP

#define PORT 8085

#define INTERMEDIARY_JOIN       0
#define INTERMEDIARY_HANDSHAKE  1
#define INTERMEDIARY_REQUEST    2
#define INTERMEDIARY_RESPONSE   3
#define FIGURE_NOT_FOUND        4

struct Ruta {
    std::string host;
    std::string port;
    bool activo;
    bool local;
};
struct PaqueteTP {
    uint8_t tipo;
    int mitad;
    uint8_t contentLength;
    std::string content;
};
//#define SERVER_PORT "1234"
class Intermediario {
    public:
        Intermediario(VSocket* fork, char* SERVER_HOST, const char* SERVER_PORT);
        ~Intermediario();
        void task(VSocket* client, bool ipv6);
        std::string consultarServidorLocal(const std::string& ruta, bool ipv6);
        std::string consultarIntermediariosTP(const std::string& figura, int mitad, bool ipv6);
        std::string extraerPiezasTP(const std::string& respuestaTP);
        std::string empaquetarTP(const PaqueteTP& paquete);
        PaqueteTP desempaquetarTP(const std::string& data);
        VSocket* getFork();
        void iniciarDescubrimiento();
        void procesarJoinIntermediario(const std::string& mensaje,
                               const std::string& ipOrigen);
        void iniciarDescubrimientoIntermediarios();
        void escucharSolicitudesTP();
        void actualizarFigurasDesdeServidorLocal(bool ipv6);

    private:
        char* SERVER_HOST;
        const char* SERVER_PORT;
        VSocket* intermediario;
        std::mutex mutexTabla;
        std::map<std::string,std::list<Ruta>> tablaRutas;

        void escucharDescubrimientoLocal();
        void enviarBroadcastIntermediario();
        void procesarAnuncioFiguras(const std::string& mensaje, const std::string& ipOrigen);
        void agregarRutaLocal(const std::string& figura,const std::string& host,const std::string& port);

        void escucharIntermediariosTP();
        void descubrirOtrosIntermediarios();
        void procesarMensajeIntermediario(const std::string& mensaje,const std::string& ipOrigen);
        std::string obtenerFigurasLocalesComoCSV();
        void agregarRutaRemota(const std::string& figura, const std::string& host, const std::string& port);
};


#endif //INTERMEDIARIO_HPP