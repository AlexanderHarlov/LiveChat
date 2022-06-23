#include <iostream>
#include <string>
#include <chrono>

#include "sw/redis++/command_options.h"

#include "views/StartView.hpp"
#include "views/LoginView.hpp"
#include "views/RegisterView.hpp"
#include "views/MainMenuView.hpp"
#include "views/ListRoomsView.hpp"
#include "views/ChatView.hpp"
#include "LiveChat.hpp"
// Connection protocol, ip and port
static const std::string& ConnectionStr{"tcp://127.0.0.1:6379"};

static const std::string& TotalUsersKey{"live_chat:total_users"};
static const std::string& UserLookupIDKey{"live_chat:username:"};
static const std::string& UserHashKey{"live_chat:user:"};
static const std::string& RoomNamesListKey{"live_chat:roomNames"};
static const std::string& RoomSortedSetKey{"live_chat:room:"};

static LiveChat* liveChatInstance = nullptr;        // file scope only

void setLiveChatInstance(LiveChat* instance) {
    liveChatInstance = instance;      
}

// Nasty hack to use member function as callback
void onChatMsgRecvCb(std::string channel, std::string msg){
    if(liveChatInstance != nullptr)
        liveChatInstance->onMsgRecv(channel, msg);
}

LiveChat::LiveChat() : m_redisClient{Redis(ConnectionStr)},
                       m_totalUsers{0}, m_hasNewMessages{false},
                       m_redisSubsriber{m_redisClient.subscriber()}, m_currentRoom{std::string{""}}
{
    initialize();
}

LiveChat::~LiveChat(){
    for(auto& view : m_views){
        delete view.second;
    }
    logoutCurrentUser();
}

void LiveChat::initialize(){
    // get total number of users to use for next user id
    auto val = m_redisClient.get(TotalUsersKey);
    if( val ) {
        m_totalUsers = std::stoull(*val);
    } else {
        m_redisClient.set(TotalUsersKey, std::to_string(m_totalUsers));
    }

    //std::cout << "Total users: " << m_totalUsers << std::endl;
    // Initialize views
    auto startView = new StartView{std::string{"StartView"}};
    m_views["StartView"] = startView;

    auto loginView = new LoginView{std::string{"LoginView"}};
    m_views["LoginView"] = loginView;

    auto registerView = new RegisterView{std::string{"RegisterView"}};
    m_views["RegisterView"] = registerView;

    auto mainMenuView = new MainMenuView{std::string{"MainMenuView"}};
    m_views["MainMenuView"] = mainMenuView;

    auto listRoomsView = new ListRoomsView{std::string{"ListRoomsView"}};
    m_views["ListRoomsView"] = listRoomsView;

    auto chatView = new ChatView{std::string{"ChatView"}};
    m_views["ChatView"] = chatView;

    // register callback function that handles msg received from pub/sub
    m_redisSubsriber.on_message(onChatMsgRecvCb);
}

void LiveChat::run(){
    View* prevView = nullptr, *currView = m_views["StartView"];
    do{
        auto nextView = currView->render(prevView, this);
        if( nextView == nullptr ) break;    //exit
        if (nextView != currView) {           
            currView->clear();
            prevView = currView;
            currView = nextView;
        }
    }while(true);
}

bool LiveChat::registerUser(const std::string& userName, const std::string& rawPassword){
    std::string userLookupKey = UserLookupIDKey + userName;
    // check if username exists 
    auto lookUpId = m_redisClient.get(userLookupKey);
    if(lookUpId)
        return false; 
    // increment total user (used for id)
    m_totalUsers = m_redisClient.incr(TotalUsersKey);
    // create new lookup key based on username 
    auto created = m_redisClient.set(userLookupKey, std::to_string(m_totalUsers));
    if( !created ) 
        return false;
    // save info locally
    m_loggedUser.name = userName;
    m_loggedUser.id = std::to_string(m_totalUsers);
    m_loggedUser.redisHashKey = UserHashKey + m_loggedUser.id;

    // insert user info in hash
    m_redisClient.hset(m_loggedUser.redisHashKey, "user_name", userName);
    m_redisClient.hset(m_loggedUser.redisHashKey, "encr_password", rawPassword);
    //m_redisClient.hset(m_loggedUser.redisHashKey, "online", "true");

    return true;
}

bool LiveChat::loginUser(const std::string& userName, const std::string& rawPassword){
    std::string userLookupKey = UserLookupIDKey+ userName;
    // check if username exists 
    auto lookUpId = m_redisClient.get(userLookupKey);
    if(!lookUpId)
        return false; 
    // get user info from hash
    std::string userHashKey = UserHashKey + (*lookUpId);
    // no real encryption
    auto storedPass = m_redisClient.hget(userHashKey, "encr_password");
    if( rawPassword != storedPass )
        return false;
    // check if user is online 
    //if( m_redisClient.hget(userHashKey, "online") == "true" )
        //return false;

    m_loggedUser.name = userName;
    m_loggedUser.id = *lookUpId;     
    m_loggedUser.redisHashKey = userHashKey;
    // set online
    //m_redisClient.hset(m_loggedUser.redisHashKey, "online", "true");
    return true;
}

void LiveChat::logoutCurrentUser(){
    //m_redisClient.hset(m_loggedUser.redisHashKey, "online", "false");

    m_loggedUser.name.clear();
    m_loggedUser.id.clear();     
    m_loggedUser.redisHashKey.clear();
}

std::vector<std::string>& LiveChat::getRoomNamesList(){
    m_roomNames.clear();
    m_redisClient.lrange(RoomNamesListKey, 0, -1, std::back_inserter(m_roomNames));
    return m_roomNames;
}

bool LiveChat::createRoom(const std::string& roomName){
    if ( std::find(m_roomNames.begin(), m_roomNames.end(), roomName) != m_roomNames.end() )
        return false;   //already exists

    long long inserted = m_redisClient.rpush(RoomNamesListKey, roomName);
    if (inserted == 0 )
        return false;

    m_roomNames.push_back(roomName);
    
    if(joinRoom(m_roomNames.size()-1)){
        if(sendMsg("SYSTEM: Room " + roomName + " created")){
            refreshMessages();
            return true;
        }
    }
      
    return false;
}

bool LiveChat::deleteRoom(int roomIdx){
    if( roomIdx < 0 || roomIdx >= m_roomNames.size() )
        return false;
    // remove all messages from sorted set 
    long long removedMsgs = m_redisClient.zremrangebyscore(RoomSortedSetKey + m_roomNames[roomIdx], UnboundedInterval<double>{});
    volatile long long test = removedMsgs;
    // remove room name from list
    long long removed = m_redisClient.lrem(RoomNamesListKey, 1, m_roomNames[roomIdx]);
    if(removed == 0)
        return false;
    // remove local copy of room name
    m_roomNames.erase(m_roomNames.begin() + roomIdx);

    return true;
}

bool LiveChat::joinRoom(int roomIdx){
    if( roomIdx < 0 || roomIdx >= m_roomNames.size() )
        return false;
    
    const std::string& roomName = m_roomNames[roomIdx];
    std::string roomSSKey = RoomSortedSetKey + roomName;
    m_currentRoom.setName(roomName);  
    m_currentRoom.setRedisKey(roomSSKey);
    refreshMessages();
    m_redisSubsriber.subscribe(roomName);         
    return true;
}

void LiveChat::leaveCurrentRoom(){
    m_redisSubsriber.unsubscribe(m_currentRoom.getName());
    m_currentRoom.setName(std::string{});  
    m_currentRoom.setRedisKey(std::string{});
    refreshMessages();    
}

void LiveChat::refreshMessages(){
    // repopulate room messages
    auto& messages = m_currentRoom.getMessages();
    messages.clear();
    if(!m_currentRoom.getName().empty() && !m_currentRoom.getRedisKey().empty())
        m_redisClient.zrangebyscore(m_currentRoom.getRedisKey(), UnboundedInterval<double>{}, std::back_inserter(messages));
}

bool LiveChat::sendMsg(const std::string& msg){
    if(m_currentRoom.getName().empty() || m_currentRoom.getRedisKey().empty())
        return false;
    // Get current time to use as score
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    // redis plus plus uses double as score type
    double score = static_cast<double>(now_ms); 
    m_redisClient.zadd(m_currentRoom.getRedisKey(), msg, score);

    m_redisClient.publish(m_currentRoom.getName(), msg);
    return true;
}

void LiveChat::tryRecvMsg(){
    m_hasNewMessages = false;
    try
    {
        m_redisSubsriber.consume();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Sub consume error: " << e.what() << '\n';
    } 
}

void LiveChat::onMsgRecv(std::string channel, std::string msg){
    if(channel != m_currentRoom.getName())
        return;

    refreshMessages();
    m_hasNewMessages = true;
}