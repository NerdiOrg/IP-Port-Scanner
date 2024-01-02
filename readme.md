# Port Scanner

This is an educational usage of an IP port scanner, which can be used for ensuring a set of services are available for your servers, or for scanning a range of ports. This version will only work with modern Windows operating systems.


## Development

```
g++ -static -o dist/portscanner main.cpp -lws2_32
```

Note: the -lsw2_32 must be at end of command.
