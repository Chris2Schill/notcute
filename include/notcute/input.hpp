#pragma once

#include <thread>
#include <ncpp/NotCurses.hh>
#include "event_loop.hpp"

namespace notcute {

class InputListener {

public:
    InputListener() {
        t = std::jthread([this](std::stop_token st){ listen_thread(st); });
    }

    void listen_thread(std::stop_token st) {
        while(!st.stop_requested()) {
            listen_nonblocking();
        }
    }

    void listen_nonblocking() {
        ncpp::NotCurses& nc = ncpp::NotCurses::get_instance();

        timespec ts {.tv_sec = 0, .tv_nsec = 100};

        ncinput ni;
        if (nc.get(&ts, &ni) > 0) {
            EventLoop::get_instance()->post(new KeyboardEvent(nullptr, ni));
        }
    }

private:
    std::jthread t;
};

}

