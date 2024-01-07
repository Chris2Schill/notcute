#pragma once

#include "logger.hpp"

struct Point {
    int x;
    int y;

    std::string to_string() const {
        return fmt::format("Position({},{})", x, y);
    }
};

struct Size {
    unsigned width;
    unsigned height;

    std::string to_string() const {
        return fmt::format("Size({},{})", width, height);
    }
};

struct Rect {
    const Point& pos() const { return m_pos; }
    const Size& size() const { return m_size; }

    int x() const { return m_pos.x; }
    int y() const { return m_pos.y; }
    int width() const { return m_size.width; }
    int height() const { return m_size.height; }

    void set_height(int h) { m_size.height = h; }
    void set_width(int w) { m_size.width = w; }

    std::string to_string() const {
        return fmt::format("{} {}", m_pos.to_string(), m_size.to_string());
    }

    void set_top(int y) { m_pos.y = y; }
    void set_bottom(int y) { m_pos.y = y-m_size.height; }
    void set_left(int x) { m_pos.x = x; }
    void set_right(int x) { m_pos.x = x-m_size.width; }

    int top() const { return y(); }
    int bottom() const { return y()+m_size.height; }
    int left() const { return x(); }
    int right() const { return x()+m_size.width; }

    Point m_pos;
    Size m_size;
};
