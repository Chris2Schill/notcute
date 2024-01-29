#pragma once

#include <notcurses/notcurses.h>
#include <thread>
#include <ncpp/NotCurses.hh>
#include "event_loop.hpp"
#include "logger.hpp"

namespace notcute {

class InputListener {

public:
    // from notcurses.h
    // #define NCMICE_NO_EVENTS     0
    // #define NCMICE_MOVE_EVENT    0x1
    // #define NCMICE_BUTTON_EVENT  0x2
    // #define NCMICE_DRAG_EVENT    0x4
    // #define NCMICE_ALL_EVENTS    0x7
    InputListener() {
        ncpp::NotCurses& nc = ncpp::NotCurses::get_instance();
        bool success = notcurses_mice_enable(nc,NCMICE_ALL_EVENTS) == 0;
        log_debug(fmt::format("INPUT Mouse Support {}", success ? "Enabled" : "Disabled" ));
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

            if (nckey_mouse_p(ni.id)) {
                EventLoop::get_instance()->post(new MouseEvent(ni));
            }
            else {
                EventLoop::get_instance()->post(new KeyboardEvent(ni));
            }
        }
    }

private:
    std::jthread t;
};

}

