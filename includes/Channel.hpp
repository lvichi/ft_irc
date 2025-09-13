#pragma once

#include <string>
#include <set>
#include <map>

class Client;

class Channel {
private:
    std::string                 _name;
    std::string                 _topic;
    std::string                 _key;        // Senha para o modo +k
    std::set<Client*>           _members;
    std::set<Client*>           _operators;
    std::set<Client*>           _invited;    // Para o modo +i

    // Modos de canal
    bool                        _inviteOnly;   // +i
    bool                        _topicProtected; // +t (apenas operadores podem mudar o tópico)
    bool                        _hasKey;       // +k (canal tem senha)
    bool                        _hasUserLimit; // +l (canal tem limite de usuários)
    unsigned int                _userLimit;

public:
    Channel();
    Channel(const std::string& name);
    Channel(const Channel& other);
    Channel& operator=(const Channel& other);
    ~Channel();

    // Getters
    const std::string&          getName() const;
    const std::string&          getTopic() const;
    const std::string&          getKey() const;
    const std::set<Client*>&    getMembers() const;
    const std::set<Client*>&    getOperators() const;
    const std::set<Client*>&    getInvited() const;
    unsigned int                getUserLimit() const;
    
    bool                        isInviteOnly() const;
    bool                        isTopicProtected() const;
    bool                        hasKey() const;
    bool                        hasUserLimit() const;
    
    // Setters
    void                        setTopic(const std::string& topic);
    void                        setKey(const std::string& key);
    void                        setUserLimit(unsigned int limit);
    
    void                        setInviteOnly(bool mode);
    void                        setTopicProtected(bool mode);
    void                        setHasKey(bool mode);
    void                        setHasUserLimit(bool mode);
    
    // Gestão de membros
    bool                        isMember(Client* client) const;
    bool                        isOperator(Client* client) const;
    bool                        isInvited(Client* client) const;
    bool                        canJoin(Client* client, const std::string& key = "") const;
    
    void                        addMember(Client* client);
    void                        removeMember(Client* client);
    void                        addOperator(Client* client);
    void                        removeOperator(Client* client);
    void                        addInvited(Client* client);
    void                        removeInvited(Client* client);
    
    // Utils
    bool                        isEmpty() const;
    unsigned int                getMemberCount() const;
    std::string                 getMembersList() const;
    void                        broadcast(const std::string& message, class IrcServ& server) const;
    void                        sendJoinMessages(Client* client, class IrcServ& server) const;
    void                        sendPartMessages(Client* client, const std::string& reason, class IrcServ& server) const;
};