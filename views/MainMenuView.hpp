#ifndef MAIN_MENU_VIEW_HPP
#define MAIN_MENU_VIEW_HPP

#include "View.hpp"

class MainMenuView : public View {
    public:
        MainMenuView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
};

#endif