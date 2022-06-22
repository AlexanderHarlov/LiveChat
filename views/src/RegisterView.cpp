#include <iostream>
#include<ios> 
#include <limits>

#include "RegisterView.hpp"
#include "../LiveChat.hpp"

RegisterView::RegisterView(std::string name) : View(name) {}

View* RegisterView::render(View* prevView, LiveChat* chat){
    std::string user_name, password;

    std::cout << "REGISTER NEW USER" << std::endl 
              << "Enter username (input 'q' to cancel): ";
    fflush(stdout);
    std::cin >> user_name;
    // discard everything but the first word if the user entered space separated name
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (user_name == "q")
        return chat->getViews()["StartView"];

    std::cout << "Enter password (input 'q' to cancel): ";
    fflush(stdout);

    std::cin >> password;
    // discard everything but the first word if the user entered space separated pass
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (password == "q")
        return chat->getViews()["StartView"];

    if(!chat->registerUser(user_name, password)) {
        showErrorMessage("Error registering user.");    
        return this;    // keep the view
    }

    return chat->getViews()["MainMenuView"];
}
