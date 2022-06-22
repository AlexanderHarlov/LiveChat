#include <iostream>

#include "LoginView.hpp"
#include "../LiveChat.hpp"

LoginView::LoginView(std::string name) : View(name) {}

View* LoginView::render(View* prevView, LiveChat* chat){
    std::string user_name, password;

    std::cout << "LOGIN" << std::endl 
              << "Enter username (input 'q' to cancel): ";
    fflush(stdout);
    std::cin >> user_name;
    // discard everything but the first word if the user entered space separated name
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (user_name == "q")
        return prevView;

    std::cout << "Enter password (input 'q' to cancel): ";
    fflush(stdout);
    std::cin >> password;
    // discard everything but the first word if the user entered space separated name
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (password == "q")
        return prevView;

    if(!chat->loginUser(user_name, password)) {
        showErrorMessage("Error loging in.");   
        return this;    // keep the view
    }

    return chat->getViews()["MainMenuView"];
}