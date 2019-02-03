# Bank Client-Server Application

Simulation of a client-server banking program using socket programming that allows users to deposit, withdraw, and check balances concurrently with other users.

## Concepts utilized:
- Signal handling in different threads that avoided deadlock (mutexing)
- Socket/port server connection (bind, listen, accept, setsockopt, connect, etc)
- Timers that would send signals to running thread on an given interval
- Account information updates on different machines running concurrently through server updates
- Semaphore as an asynchronous threadsafe synchronization mechanism

## Abstract
For this project you will write a server program to emulate a bank and a client program to communicate
with it. The server program will need to support multiple simultaneous client TCP/IP connections,
which will require multithreading. Multiple clients accessing the same account database at the same
time will require the use of mutexes to protect all shared data. The server will also globally lock the
account data at regular intervals to print diagnostic information to its STDOUT, ostensibly for the
bank's network operator to monitor server status. The server will not take any commands once it is
running and should be able to shut down gracefully by cataching the exit signal sent by Ctrl+C.
Your client will be fairly simple. It will connect to the server, read in user commands from STDIN,
send them to your bank server and write the server's responses to STDOUT. The client side should
additionally check the commands the user is entering to make sure they are correctly formed and
formatted.

The command syntax allows the user to; create accounts, start sessions to serve specific accounts,
and to exit the client process altogether:
create <accountname (char) >
serve <accountname (char) >
deposit <amount (double) >
withdraw <amount (double) >
query
end
quit
The client process will send commands to the bank, and the bank will send responses back to the
client. Every messages the client sends to the server should result in a response from the server; either
data, an error or a confirmation message.

The client and server programs can be invoked in any order. Client processes that cannot find the
server should repeatedly try to connect every 3 seconds until they find a server. The client must specify
the name of the machine and port where the client expects to find the server process as a command-line
argument. The server takes the port to listen on as the only argument:
./bankingServer 9999
./bankingClient cp.cs.rutgers.edu 9999
