#ifndef LIST_ROOMS_VIEW_HPP
#define LIST_ROOMS_VIEW_HPP

#include "View.hpp"

class ListRoomsView : public View {
    public:
        ListRoomsView(std::string name);
        View* render(View* prevView, LiveChat* chat) override;
};

#endif