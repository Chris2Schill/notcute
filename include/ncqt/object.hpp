#pragma once

#include <ncpp/NotCurses.hh>

namespace ncqt {

class Object : public ncpp::Root {
    
public:
    virtual ~Object() = default;
    Object(Object* parent = nullptr)
        : ncpp::Root(ncpp::NotCurses::get_instance())
        , parent(parent)
    {
    }
protected:

    Object* parent = nullptr;
};

}
