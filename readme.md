# Port Scanner

This is an educational usage of an IP port scanner, which can be used for ensuring a set of services are available for your servers, or for scanning a range of ports. 


## Development
Edit environment main file: linux.cpp or windows.cpp

### Linux
```
g++ -static -o dist/portscanner-linux linux.cpp
```

### Windows
```
g++ -static -o dist/portscanner-windows windows.cpp -lws2_32
```
- the -lsw2_32 must be at end of command.


