#ifndef ROOM_HPP
#define ROOM_HPP

#include <string>
#include <vector>

class Room {
    private:
        std::string m_name;
        std::string m_redisKey;
        std::vector<std::pair<std::string, double>> m_messages;
    public:
        Room(std::string name) : m_name{name} {};
        std::string& getName() { return m_name; }
        void setName(const std::string& name) { m_name = name; }
        std::string& getRedisKey() { return m_redisKey; }
        void setRedisKey(const std::string& key) { m_redisKey = key; }
        std::vector<std::pair<std::string, double>>& getMessages() { return m_messages; }        
};

#endif