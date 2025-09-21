#!/bin/bash

echo "=== Testando Comandos IRC ==="

# Use a block to send multiple IRC commands to the server via netcat (nc) using a here-document style.
{
    echo "PASS abc"
    sleep 0.5
    echo "NICK cat"
    sleep 0.5
    echo "USER testuser 0 * :Test Real Name"
    sleep 0.5
    echo "JOIN #testchannel"
    sleep 0.5
    echo "MODE #testchannel"
    sleep 1.5
    echo "MODE #testchannel +itkol this cat 10"
    sleep 1.5
    echo "MODE #testchannel +kol this cat 10"
    sleep 1.5
    echo "MODE #testchannel +o"
    sleep 1.5
    echo "MODE #testchannel +it"
    sleep 1.5
    echo "MODE #testchannel"
	sleep 1.5
	echo "MODE #testchannel +l 30"
    sleep 1.5
    echo "MODE #testchannel +o cat"
    sleep 1.5
    echo "MODE #testchannel o cat"
    sleep 1.5
    echo "MODE #testchannel +io cat"
    sleep 1.5
    echo "MODE #testchannel +io cat this"
    sleep 1.5
    echo "MODE #testchannel +o cat -l+k this"
    sleep 1.5
    echo "MODE #testchannel -k that"
    sleep 1.5
    echo "MODE #testchannel"
    sleep 1.5
    echo "MODE #testchannel -k this"
    sleep 1.5
    echo "MODE #testchannel"
    sleep 1.5
    echo "part #testchannel"
    sleep 1.5
    echo "join #testchannel"
    sleep 1.5
    echo "join #testchannel this"
    sleep 1.5
    echo "QUIT :Saindo do servidor"
} | nc -C localhost 6667
