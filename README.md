# ElephantParty

### Compilation Note
I added the tcp_client.h and tcp_client.cpp files as a common interface since they need to be used by both the ATM and the proxy. As such, when compiling either the ATM or proxy, you need to include the tcp_client.cpp file as one of your sources, i.e.

<code>g++ ATM.cpp tcp_client.cpp -o atm</code>
OR
<code>g++ proxy.cpp tcp_client.cpp -o proxy</code>.

We can eventually create a MAKE file that will do all of this for us, but for now, do this.

To compile Card.h, please use c++11 by
<code>g++ -std=c++11 ATM.cpp tcp_client.cpp -o atm</code>
