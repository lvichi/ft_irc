#!/bin/bash

echo "=== Testando Comandos IRC ==="

# Use a block to send multiple IRC commands to the server via netcat (nc) using a here-document style.
{
    echo "PASS abc"
    sleep 0.5
    echo "NICK testnick"
    sleep 0.5
    echo "USER testuser 0 * :Test Real Name"
    sleep 0.5
    echo "JOIN #testchannel"
    sleep 0.5
    echo "MODE #testchannel"
    sleep 0.5
    echo "MODE #testchannel +i"
    sleep 0.5
    echo "MODE #testchannel +o"
    sleep 0.5
    echo "MODE #testchannel +o cat"
    sleep 0.5
    echo "MODE #testchannel o cat"
    sleep 0.5
    echo "MODE #testchannel +io cat"
    sleep 0.5
    echo "MODE #testchannel +io cat this"
    sleep 0.5
    echo "MODE #testchannel +o cat -l+k this"
    sleep 0.5
    echo "QUIT :Saindo do servidor"
} | nc -C localhost 6667
