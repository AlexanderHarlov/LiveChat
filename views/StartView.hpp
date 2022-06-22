#ifndef START_VIEW_HPP
#define START_VIEW_HPP

#include "View.hpp"

class StartView : public View {
    public:
        StartView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
};

#endif