#include <iostream>
#include <thread>
#include <future>

#include "ChatView.hpp"
#include "../LiveChat.hpp"

static std::mutex msgMutex;

// Helper function to measure elapsed time
template <
    typename result_t   = std::chrono::milliseconds,
    typename clock_t    = std::chrono::steady_clock,
    typename duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

void checkForNewMessages(LiveChat* chat, ChatView* cv){
    for(;;){
        chat->tryRecvMsg();
        if(chat->hasNewMessages()){       
            msgMutex.lock();         
            cv->clear();
            cv->updateMessages(chat);       
            msgMutex.unlock();       
        }       
    }
}

ChatView::ChatView(std::string name) : View(name) {}

View* ChatView::render(View* prevView, LiveChat* chat){
    std::string loggedUser = chat->getCurrentUserName();
    updateMessages(chat);
    std::string userInput;    
    std::thread msgCheckerThread(checkForNewMessages, chat, this);
    for(;;){                     
        std::getline(std::cin, userInput);
        if( userInput == "!q" ) 
            break;
        msgMutex.lock();
        chat->sendMsg(loggedUser + ": " + userInput);
        msgMutex.unlock();      
    }

    msgCheckerThread.detach();
    chat->leaveCurrentRoom();
    return chat->getViews()["MainMenuView"];
}

void ChatView::updateMessages(LiveChat* chat){
    std::cout << "***" << chat->getCurrentRoom().getName()<< "***" << std::endl;
    std::cout << "*** Input !q to exit" << std::endl;
    
    auto& messages = chat->getCurrentRoom().getMessages();

    for(auto& message : messages ){
        std::cout << "--------------------------------------------------" << std::endl;
        int64_t msg_time_ms = message.second;
        using time_point = std::chrono::system_clock::time_point;
        time_point msg_timepoint{std::chrono::duration_cast<time_point::duration>(std::chrono::milliseconds(msg_time_ms))};
        std::time_t msg_time_t = std::chrono::system_clock::to_time_t(msg_timepoint);

        std::cout << "|" << ctime(&msg_time_t) << "|" << message.first << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        
    }
    std::cout << "**************************************************" << std::endl;
    std::cout << chat->getCurrentUserName() << " >: ";

    fflush(stdout);
}