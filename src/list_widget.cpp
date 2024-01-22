#include <notcute/list_widget.hpp>

namespace notcute {

ListWidget::ListWidget(Widget* parent)
    :ScrollArea(nullptr, parent)
{
    set_layout(new VBoxLayout);
    set_focus_policy(FocusPolicy::FOCUS);

    Widget* list_content = new Widget(this);
    list_content->set_layout(new VBoxLayout);
    set_content(list_content);
}

ListWidget::~ListWidget() {
    delete_items();
}

void ListWidget::add_item(ListItem* item) {
    bool prev = is_selected_idx_valid();
    items.push_back(item);
    bool valid = is_selected_idx_valid();

    if (valid != prev && valid) {
        item_hovered(items[selected_idx]);
    }
    resize_content();
}

void ListWidget::add_text(const std::string& text) {
    add_item(new ListTextItem(text));
}

void ListWidget::add_widget(Widget* w) {
    w->reparent(this);
    add_item(new ListWidgetItem(w));
}

void ListWidget::clear() {
    delete_items();
    redraw();
}

void ListWidget::next_item() {
    if (selected_idx < items.size()-1) {
        selected_idx++;
        notcute::log_debug("listwidget nextitem");
        if (selected_idx > get_visible_height()+content_subwindow.y-1) {
            scroll_vertically();
        }
        redraw();
    }
}

void ListWidget::prev_item() {
    if (selected_idx != 0) {
        selected_idx--;
        if (selected_idx < content_subwindow.y) {
            scroll_vertically(-1);
        }
        redraw();
    }
}

void ListWidget::draw_border(ncpp::Plane* p) {
    assert(p == get_plane());
    ScrollArea::draw_border(p);

    auto chans = p->get_channels();

    set_fg_color({255,255,255, NCALPHA_OPAQUE});
    set_bg_color(get_frame_bg());

    int selected = is_selected_idx_valid() ? selected_idx + 1 : 0;
    std::string curr_item = fmt::format("{} of {}", selected, items.size());
    plane->putstr(plane->get_dim_y()-1, plane->get_dim_x()-2-curr_item.size(), curr_item.c_str());

    p->set_channels(chans);
}

void ListWidget::draw_content(Widget* content) {
    ncpp::Plane* cp = content->get_plane();

    for (int i = 0; i < items.size(); ++i)
    {
        ListItem* item = items[i];
        if (!item) { continue; }
        std::string str = item->to_string();
        cp->putstr(i, 0, str.c_str());
        bool selected = i == selected_idx;
        highlight_row(i, selected);
    }
    redraw();
}

void ListWidget::highlight_row(int y, bool selected) {
    Color fg = selected ? fg_selected : fg_notselected;
    Color bg = selected ? bg_selected : bg_notselected;

    ncpp::Plane* cp = content->get_plane();

    for (int col = 0; col < cp->get_dim_x(); ++col) {
        ncpp::Cell c;
        cp->get_at(y,col,c);
        c.set_fg_rgb8(fg.red, fg.green, fg.blue);
        c.set_bg_rgb8(bg.red, bg.green, bg.blue);
        c.set_fg_alpha(fg.alpha);
        c.set_bg_alpha(bg.alpha);
        cp->putc(y,col,c);
    }
}

bool ListWidget::on_keyboard_event(KeyboardEvent* e) {
    switch (e->get_key()) {
        case 'j':
        case NCKEY_DOWN:
            next_item();
            if (is_selected_idx_valid()) {
                item_hovered(items[selected_idx]);
            }
            return true;
        case 'k':
        case NCKEY_UP:
            prev_item();
            if (is_selected_idx_valid()) {
                item_hovered(items[selected_idx]);
            }
            return true;
        case NCKEY_ENTER:
            if (is_selected_idx_valid()) {
                item_selected(items[selected_idx]);
            }
            redraw();
            return true;
        default:
            return ScrollArea::on_keyboard_event(e);
    }
}

const std::vector<ListItem*>& ListWidget::get_items() {
    return items;
}

bool ListWidget::is_selected_idx_valid() const {
    return (0 <= selected_idx && selected_idx < items.size());
}

int ListWidget::get_selected_idx() const {
    return selected_idx;
}

void ListWidget::resize_content() {
    adjust_size_to_contents();
}

void ListWidget::adjust_size_to_contents() {
    if (!content) { return; }
    // Start with the size of the scroll area window
    Rect rect = get_geometry();
    int width = rect.width();
    int height = rect.height();

    // grow in height if need be..
    height = std::max(height, static_cast<int>(items.size()));

    // grow in width if need be.. this could be optimized
    // but wont matter for most use cases.
    for (auto& item : items) {
        int w = item->to_string().size();
        if (w > width) { width = w; }
        width = std::max(width, static_cast<int>(item->to_string().size()));
    }

    Rect r = {};
    r.set_width(width);
    r.set_height(height);
    content->set_geometry(r);

    // Run a predraw here to ensure that the backing layout
    // and plane all get updated geometry asap. maybe
    // set_geometry should call this but it seems wrong.
    // Scroll area is a weird one compared to all other widgets
    // so leaving this here
    content->pre_draw(content->get_plane());

    log_debug(fmt::format("listwidget resized({},{})", height, width));
    redraw();
}


void ListWidget::delete_items() {
    for (ListItem* item : items) {
        delete item;
    }
    items.clear();
}

}
