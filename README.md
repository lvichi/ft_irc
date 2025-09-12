# ft_irc - Servidor IRC Funcional

## Como Testar com Clientes IRC Reais

### 1. **Compilar e Executar**
```bash
make
./ircserv 6667 senha
```

### 2. **Testar com HexChat/IRCCloud/Weechat**

**Configuração de Conexão:**
- **Servidor**: localhost (ou IP do seu servidor)
- **Porta**: 6667
- **Senha**: senha
- **Nickname**: seu_nick
- **Username**: seu_user

**Sequência de Comandos Testados (weechat):**
1. `/server add ftirc localhost/6667 -notls -password=senha`: Cria o servidor dentro do weechat
2. `/connect ftirc`: Conecta com o servidor
3. `/join #teste`: Cria/entra no canal
4. `Olá pessoal!`: Enviar mensagem no canal
5. `/nick newnick`: Mudar nickname

### 3. **Teste com netcat (debug)**
```bash
nc localhost 6667
PASS minhasenha
NICK testnick
USER testuser 0 * :Test User Real Name
JOIN #geral
PRIVMSG #geral :Primeira mensagem!
```

## Funcionalidades Prontas Para Uso

✅ **Autenticação completa** (senha + nick + user)  
✅ **Criação automática de canais**  
✅ **Mensagens privadas e de canal**  
✅ **Sistema de boas-vindas IRC**  
✅ **Códigos de erro padronizados**  
✅ **Gerenciamento de múltiplos clientes**  
✅ **Compatibilidade com clientes IRC oficiais**

O servidor agora está **funcional para uso real** e compatível com a maioria dos clientes IRC!