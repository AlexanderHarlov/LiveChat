#include <sw/redis++/redis++.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "LiveChat.hpp"

using namespace sw::redis;

int main()
{
    try
    {
        LiveChat chat;
        setLiveChatInstance(&chat);
        chat.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
  
    return 0;
}