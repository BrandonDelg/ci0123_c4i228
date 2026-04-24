# Como compilar?

```.
para ipv6 use:
make clean; make; make run-serverCC PORT=1234 IPV6=true
make run-clientCC IP=::1 PORT=1234 IPV6=true

para ipv4
make clean; make; make run-serverCC PORT=1234
make run-clientCC IP=127.0.0.1 PORT=1234
```
