# ClientServerExample
A simple example of a client and server using TCP/IP protocol.

The server generates a base and a number in that base, which it then sends to the client along with a possible prime factor written in decimal.

The client checks if this possible prime factor is a valid prime factor before returning its results to the server. 

The server distributes prime factors in this way using a round robin system, and it ends when no clients are connected, or when n prime factors are found in a row.

How to compile programs:

gcc Server.c -lm -lpthread -o server

gcc Client.c -0 client

Depending on linux distro, these commands might also require -std=c99 flag added to each.


How to run program:

./client "clientini.txt"

./server "serverini.txt"

or

./client

./server

ini files can be changed to adjust necessary parameters.
