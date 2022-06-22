#ifndef VIEW_HPP
#define VIEW_HPP

#include "unordered_map"

class LiveChat;

class View {
    public:
        View(std::string name) : m_name(name), m_userSelection{0} {}
        virtual View* render(View* prevView, LiveChat* chat) = 0;
        void clear();
        void showErrorMessage(const std::string& errMsg);
        std::string& getName() { return m_name; }
        int getUserSelection() { return m_userSelection; }
    protected:
        std::string m_name;
        int m_userSelection;
};

#endif