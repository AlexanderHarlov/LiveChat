#ifndef LOGIN_VIEW_HPP
#define LOGIN_VIEW_HPP

#include "View.hpp"

class LoginView : public View {
    public:
        LoginView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
};

#endif