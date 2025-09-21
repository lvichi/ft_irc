#!/bin/bash

echo "=== Testing IRC Server Broadcasting Fix ==="

# Kill any existing server
pkill -f ircserv 2>/dev/null

# Start server
cd /workspaces/ft_irc
valgrind --leak-check=full --show-leak-kinds=all ./ircserv 6667 abc &
SERVER_PID=$!
echo "Server started with PID: $SERVER_PID"
sleep 2

echo ""
echo "=== Connecting Client 1 ==="
# Client 1 connects and joins channel
{
    echo "PASS abc"
    echo "NICK client1"
    echo "USER client1 0 * :Client One"
    echo "JOIN #test"
    sleep 2
	echo "MODE #test +o client2"
    echo "PRIVMSG #test :Hello from client1"
    sleep 2
    echo "PART #test"
	echo "JOIN #test"
	echo "MODE #test +l 30"
	echo "MODE #test"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT1_PID=$!

echo ""
echo "=== Connecting Client 2 ==="
# Client 2 connects and joins channel
{
    echo "PASS abc"
    echo "NICK client2"
    echo "USER client2 0 * :Client Two"
    echo "JOIN #test"
    sleep 3
	echo "MODE #test +i"
    sleep 3
	echo "INVITE client3 #test"
    echo "PRIVMSG #test :Hello from client2"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT2_PID=$!

echo ""
echo "=== Connecting Client 3 ==="
# Client 3 connects and joins channel
{
    echo "PASS abc"
    echo "NICK client3"
    echo "USER client3 0 * :Client Three"
    sleep 5
    echo "JOIN #test"
    sleep 1
    echo "PRIVMSG #test :Hello from client3"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT3_PID=$!

# Wait for all clients to finish
sleep 20

# Kill processes
kill -SIGINT $CLIENT1_PID 2>/dev/null
kill -SIGINT $CLIENT2_PID 2>/dev/null
kill -SIGINT $CLIENT3_PID 2>/dev/null
kill -SIGINT $SERVER_PID 2>/dev/null

echo ""
echo "=== Test Complete ==="
echo "If broadcasting is working correctly, all clients should have received messages from each other."
