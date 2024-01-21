#pragma once

namespace notcute {
    class BoxLayout;

    // For now all the only Layout type is BoxLayout
    // but in the future we could refactor to make a
    // generic Layout and have Box subclass Layout
    // to be able to provide other types of Layouts..
    // in theory. Might be tricky with how Notcurses is based
    // on 2D planes but I think its possible.
    using Layout = BoxLayout;
}

#include "box.hpp"
