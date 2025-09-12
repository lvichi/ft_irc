#!/bin/bash

echo "=== ft_irc Test ==="
echo "Testar servidor na porta 6667 com a senha 'senha'..."

# mata o teste anterior (aconteceu uma vez de isso causar erro)
pkill -f ircserv 2>/dev/null

# inicia o servidor no background
cd /workspaces/ft_irc
./ircserv 6667 senha &
SERVER_PID=$!

echo "Servidor iniciado com PID: $SERVER_PID"
sleep 2

echo ""
echo "=== Teste de Conexão Básica ==="
echo "Testando conexão básica com netcat..."

# ainda bem que existe o chatgpt, aprendi mt bash com ele...
{
    echo "PASS senha"
    echo "NICK testuser"
    echo "USER testuser 0 * :Test User"
    echo "JOIN #test"
    echo "PRIVMSG #test :Hello, IRC World!"
    sleep 1
    echo "QUIT :Goodbye"
} | nc -C localhost 6667 &
NETCAT_PID=$!
sleep 3

# bah, netcat precisa morrer
kill $NETCAT_PID 2>/dev/null

echo ""
echo "=== Server Log ==="
sleep 1

# bah servidor
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "=== OKAY ==="
echo ""