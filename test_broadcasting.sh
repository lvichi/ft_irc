#!/bin/bash

echo "=== Testing IRC Server Broadcasting Fix ==="

# Kill any existing server
pkill -f ircserv 2>/dev/null

# Start server
cd /workspaces/ft_irc
./ircserv 6667 senha &
SERVER_PID=$!
echo "Server started with PID: $SERVER_PID"
sleep 2

echo ""
echo "=== Connecting Client 1 ==="
# Client 1 connects and joins channel
{
    echo "PASS senha"
    echo "NICK client1"
    echo "USER client1 0 * :Client One"
    echo "JOIN #test"
    sleep 2
    echo "PRIVMSG #test :Hello from client1"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT1_PID=$!

echo ""
echo "=== Connecting Client 2 ==="
# Client 2 connects and joins channel
{
    echo "PASS senha"
    echo "NICK client2"
    echo "USER client2 0 * :Client Two"
    echo "JOIN #test"
    sleep 1
    echo "PRIVMSG #test :Hello from client2"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT2_PID=$!

echo ""
echo "=== Connecting Client 3 ==="
# Client 3 connects and joins channel
{
    echo "PASS senha"
    echo "NICK client3"
    echo "USER client3 0 * :Client Three"
    echo "JOIN #test"
    sleep 1
    echo "PRIVMSG #test :Hello from client3"
    sleep 2
    echo "QUIT"
} | /usr/bin/nc -C localhost 6667 &
CLIENT3_PID=$!

# Wait for all clients to finish
sleep 5

# Kill processes
kill $CLIENT1_PID 2>/dev/null
kill $CLIENT2_PID 2>/dev/null
kill $CLIENT3_PID 2>/dev/null
kill $SERVER_PID 2>/dev/null

echo ""
echo "=== Test Complete ==="
echo "If broadcasting is working correctly, all clients should have received messages from each other."