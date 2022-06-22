#include <cstdlib>
#include <iostream>
#include "View.hpp"

void View::clear(){
    system("clear"); //Dangerous, rewrite if possible !!
}

void View::showErrorMessage(const std::string& errMsg){
    std::string exit;
    this->clear();
    std::cout << errMsg << "\nPress ENTER to clear message\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    this->clear();
}