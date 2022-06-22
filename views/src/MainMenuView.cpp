#include <iostream>
#include <exception>

#include "MainMenuView.hpp"
#include "ListRoomsView.hpp"
#include "../LiveChat.hpp"

MainMenuView::MainMenuView(std::string name) : View(name) {}

View* MainMenuView::render(View* prevView, LiveChat* chat){
    std::string userChoice;
    std::cout << "[0] Logout" << std::endl << "[1] Create Room" << std::endl << "[2] Join Room"
              << std::endl << "[3] Delete Room" << std::endl;
    std::cin >> userChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    try {
        m_userSelection = std::stoi(userChoice);
    } catch( const std::exception &e ){
        m_userSelection = -1;
        showErrorMessage("Invalid input");
        return this;
    }

    switch(m_userSelection){
        case 0: {
            chat->logoutCurrentUser();
            return chat->getViews()["StartView"];
        }
        
        case 1: // fallthrough
        case 2: // fallthrough
        case 3: return chat->getViews()["ListRoomsView"];
        default: {
            showErrorMessage("Invalid input. Please enter correct option");
            return this;
        }
    }
}