# Como compilar

```.
para el archivo ipv4-ssl-cli.cpp abra una terminal en la carpeta src y ejecute:
make
./ipv4-ssl-cli.out


para el archivo ssl-cli.cpp:
g++ SSLSocket.cpp VSocket.cpp Socket.cpp ssl-cli.cpp -o ssl -lssl -lcrypto
./ssl

```
