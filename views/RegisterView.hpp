#ifndef REGISTER_VIEW_HPP
#define REGISTER_VIEW_HPP

#include "View.hpp"

class RegisterView : public View {
    public:
        RegisterView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
};

#endif