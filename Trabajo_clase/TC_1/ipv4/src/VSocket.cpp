

/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2026-i
  *  Grupos: 2 y 3
  *
  *******   VSocket base class implementation
  *
  * (Fedora version)
  *
 **/

#include <sys/socket.h>
#include <arpa/inet.h>		// ntohs, htons
#include <stdexcept>            // runtime_error
#include <cstring>		// memset
#include <netdb.h>		// getaddrinfo, freeaddrinfo
#include <unistd.h>		// close
/*
#include <cstddef>
#include <cstdio>

//#include <sys/types.h>
*/
#include "VSocket.hpp"


/**
  *  Class creator (constructor)
  *     use Unix socket system call
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
void VSocket::Init( char t, bool IPv6 ){
    this->IPv6 = IPv6;
    int domain = IPv6 ? AF_INET6 : AF_INET;
    int type;

    if (t == 's' || t == ' ') {
        type = SOCK_STREAM;
    } else if (t == 'd') {
        type = SOCK_DGRAM;
    } else {
        throw std::invalid_argument("Invalid socket type");
    }

    sockId = socket(domain, type, 0);
    if ( -1 == sockId ) {
        throw std::runtime_error( "VSocket::Init, (reason)" );
    }
}


/**
  * Class destructor
  *
 **/
VSocket::~VSocket() {

   this->Close();

}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void VSocket::Close(){
      if (sockId >= 0) {
        close(sockId);
        sockId = -1;
    }
}

/**
  * TryToConnect method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dot notation, example "10.84.166.62"
  * @param      int port: process address, example 80
  *
 **/
int VSocket::TryToConnect( const char * hostip, int port ) {
    int st;
    if (!IPv6) {
        struct sockaddr_in  host4;
        memset( (char *) &host4, 0, sizeof( host4 ) );
        host4.sin_family = AF_INET;
        st = inet_pton( AF_INET, hostip, &host4.sin_addr );
        if (st <= 0 ) {
            throw( std::runtime_error( "VSocket::TryToConnect, inet_pton" ));
        }
        host4.sin_port = htons( port );
        st = connect( sockId, (sockaddr *) &host4, sizeof( host4 ) );
        if ( -1 == st ) {
            throw( std::runtime_error( "VSocket::TryToConnect, connect" ));
        }
    } else {
        struct sockaddr_in6  host6;
        memset( (char *) &host6, 0, sizeof( host6 ) );
        host6.sin6_family = AF_INET6;
        st = inet_pton( AF_INET6, hostip, &host6.sin6_addr );
        if (st <= 0 ) {
            throw( std::runtime_error( "VSocket::TryToConnect, inet_pton" ));
        }
        host6.sin6_port = htons( port );
        st = connect( sockId, (sockaddr *) &host6, sizeof( host6 ) );
        if ( -1 == st ) {
            throw( std::runtime_error( "VSocket::TryToConnect, connect" ));
        }
    }
    if ( -1 == st ) {
        throw std::runtime_error( "VSocket::TryToConnect" );
    }

   return st;

}


/**
  * TryToConnect method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
/* int VSocket::TryToConnect( const char *host, const char *service ) {
   int st = -1;

   throw std::runtime_error( "VSocket::TryToConnect");

   return st;

}*/


int VSocket::TryToConnect( const char *host, const char *service ) {
    int st;
    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    st = getaddrinfo( host, service, &hints, &result );

    for ( rp = result; rp; rp = rp->ai_next ) {
    st = connect( sockId, rp->ai_addr, rp->ai_addrlen );
    if ( 0 == st )
        break;
    }

    freeaddrinfo( result );
    if (rp == NULL) {
         throw std::runtime_error("VSocket::TryToConnect connect");
    }

    return st;

}


