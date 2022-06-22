#include <iostream>

#include "ListRoomsView.hpp"
#include "../LiveChat.hpp"

ListRoomsView::ListRoomsView(std::string name) : View(name) {}

View* ListRoomsView::render(View* prevView, LiveChat* chat){
    m_userSelection = -1;
    int prevMenuSelection = chat->getViews()["MainMenuView"]->getUserSelection();
    auto roomNamesList = chat->getRoomNamesList();
    if( roomNamesList.empty() && prevMenuSelection > 1 ) {
        showErrorMessage("No rooms currently created");
        return prevView;
    }
    if(prevMenuSelection > 1){  // join or delete
        std::cout << "[0] Cancel" << std::endl;
    }

    for( const auto& room : roomNamesList){
        int index = &room - &roomNamesList[0] + 1;
        std::cout << "[" << index << "] " << room << std::endl;
    }

    switch (prevMenuSelection)
    {
    case 1:                 // Create room
    {
        std::string roomName;
        std::cout << "Enter room name, different from the above. Do NOT use spaces in name (!q to cancel): ";
        fflush(stdout);
        std::cin >> roomName; 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');    
        if(roomName == "!q")
            return chat->getViews()["MainMenuView"];
        // discard everything but the first word if the user entered space separated room name
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

        if(!chat->createRoom(roomName)){
            showErrorMessage("Could not create room");
            return this;
        }
        return chat->getViews()["ChatView"];
    }
    case 2:                 // Join room
    {
        std::string raw_input;
        std::cout << "Enter room number to JOIN: ";
        fflush(stdout);
        std::cin  >> raw_input;     
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        try {
            m_userSelection = std::stoi(raw_input);
        } catch(const std::exception& e){
            showErrorMessage("Invalid input. Please enter correct room number");
            return this;
        }
        if(m_userSelection < 0 || m_userSelection > roomNamesList.size() ){
            showErrorMessage("Invalid input. Please enter correct room number");
            return this;
        }
        if(m_userSelection == 0)    //cancel
            return chat->getViews()["MainMenuView"];

        chat->joinRoom(m_userSelection - 1);
        return chat->getViews()["ChatView"];
    }
    case 3:                 // Delete room
    {
        std::string raw_input;
        std::cout << "Enter room number to DELETE: ";
        fflush(stdout);
        std::cin  >> raw_input;     
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        try {
            m_userSelection = std::stoi(raw_input);
        } catch(const std::exception& e){
            showErrorMessage("Invalid input. Please enter correct room number");
            return this;
        }
        if(m_userSelection < 0 || m_userSelection > roomNamesList.size() ){
            showErrorMessage("Invalid input. Please enter correct room number");
            return this;
        }

        if(m_userSelection == 0)
            return chat->getViews()["MainMenuView"];

        chat->deleteRoom(m_userSelection - 1);
        return chat->getViews()["MainMenuView"];
    }
    default:
        return prevView;
    }

    return prevView;
}