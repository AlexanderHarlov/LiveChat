#ifndef LIVE_CHAT_HPP
#define LIVE_CHAT_HPP

#include <vector>
#include <unordered_map>
#include "sw/redis++/redis++.h"

#include "Room.hpp"

using namespace sw::redis;

class View;

struct User {
    std::string id;
    std::string name;
    std::string redisHashKey;
};

class LiveChat{
    private:
        void initialize();
        Redis m_redisClient;
        Subscriber m_redisSubsriber;
        u_int64_t m_totalUsers;
        std::unordered_map<std::string, View*> m_views;
        User m_loggedUser;
        std::vector<std::string> m_roomNames;
        Room m_currentRoom;
        bool m_hasNewMessages;

        void refreshMessages();

    public:
        LiveChat();
        ~LiveChat();
        void run();
        bool registerUser(const std::string& userName, const std::string& rawPassword);
        bool loginUser(const std::string& userName, const std::string& rawPassword);
        void logoutCurrentUser();
        std::vector<std::string>& getRoomNamesList();
        bool createRoom(const std::string& roomName);
        bool deleteRoom(int roomIdx);
        bool joinRoom(int roomIdx);
        void leaveCurrentRoom();
        std::unordered_map<std::string, View*>& getViews() { return m_views; } 
        Room& getCurrentRoom() { return m_currentRoom; } 
        const std::string& getCurrentUserName() { return m_loggedUser.name; }
        bool hasNewMessages() { return m_hasNewMessages; }
        bool sendMsg(const std::string& msg);
        void tryRecvMsg();
        void onMsgRecv(std::string channel, std::string msg);
};

extern void setLiveChatInstance(LiveChat* instance);

#endif