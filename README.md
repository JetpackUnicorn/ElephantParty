# ElephantParty

### Compilation Note

g++ -std=c++11 -g3 -ggdb -O0 -Wextra -Wno-unused atm2.cpp crypto.cpp -o atm -lcryptopp

g++ -pthread -g3 -ggdb -O0 -Wall -Wextra -Wno-unused proxy2.cpp -o proxy -lcryptopp

g++ -std=c++11 -pthread -g3 -ggdb -O0 -Wextra -Wno-unused bank2.cpp -o bank -lcryptopp

