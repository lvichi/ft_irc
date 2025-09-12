#!/bin/bash

echo "=== Testando Comandos IRC ==="

# Use a block to send multiple IRC commands to the server via netcat (nc) using a here-document style.
{
    echo "PASS test123"
    sleep 0.5
    echo "NICK testnick"
    sleep 0.5
    echo "USER testuser 0 * :Test Real Name"
    sleep 0.5
    echo "JOIN #testchannel"
    sleep 0.5
    echo "TOPIC #testchannel :Este é o tópico do canal!"
    sleep 0.5
    echo "PRIVMSG #testchannel :Olá pessoal!"
    sleep 0.5
    echo "PART #testchannel :Saindo do canal"
    sleep 0.5
    echo "QUIT :Saindo do servidor"
} | nc -C localhost 6667