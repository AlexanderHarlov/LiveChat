#include <iostream>
#include <limits>

#include "StartView.hpp"
#include "../LiveChat.hpp"

StartView::StartView(std::string name) : View(name) {}

View* StartView::render(View* prevView, LiveChat* chat){
    std::cout << "WELCOME TO LIVE CHAT" << std::endl << "[0] Exit" << std::endl << "[1] Register" 
              << std::endl << "[2] Login" << std::endl;

    std::string rawInput;
    std::cin >> rawInput;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    int userInput = -1;

    try
    {
        userInput = stoi(rawInput);
    }
    catch(const std::exception& e)
    {
        showErrorMessage("Invalid input. Please enter correct option");
        return this;
    }

    switch(userInput){
        case 0: return nullptr;
        case 1: return chat->getViews()["RegisterView"];
        case 2: return chat->getViews()["LoginView"];
        default: {
            showErrorMessage("Invalid input. Please enter correct option");
            return this;
        }
    }
}