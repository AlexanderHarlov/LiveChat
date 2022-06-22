#ifndef CHAT_VIEW_HPP
#define CHAT_VIEW_HPP

#include "View.hpp"

class ChatView : public View {
    public:
        ChatView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
        void updateMessages(LiveChat* chat);   
};

#endif