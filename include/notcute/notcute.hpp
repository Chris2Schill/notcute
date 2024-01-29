#pragma once

#include "layout.hpp"
#include "widget.hpp"
#include "logger.hpp"
#include "renderer.hpp"
#include "text_widget.hpp"


namespace notcute {

inline void report_closing_diagnostic() {
    log_debug("Shutting Down");
    log_debug(fmt::format("Alive Widgets = {}", Widget::alive_widgets));
}

}
