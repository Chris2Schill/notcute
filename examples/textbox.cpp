#include "notcute/box_layout.hpp"
#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/scroll_area.hpp>
#include <notcute/tab_widget.hpp>
#include <notcute/text_widget.hpp>
#include <openssl/rand.h>

using namespace notcute;

class TextBox : public FrameWidget {
public:
    TextBox(const std::string& text, Widget* parent = nullptr)
        :FrameWidget(parent)
        ,text(text)
    {
        set_layout(new VBoxLayout);
    }

    void draw(ncpp::Plane* p) override {
        const char* c = text.c_str();
        int size = text.size();

        int rows = p->get_dim_y();
        int cols = p->get_dim_x();

        int y = 0, x = 0;

        int i = 0;
        while(i < size) {
            if (*c == '\n') {
                ++y;
                ++i;
                ++c;
                x = 0;
                continue;
            }

            p->putstr(y,x, c);

            ++x;

            if (x >= cols) {
                x = 0;
                y++;
            }

            ++i;
            ++c; 
        }
        
        
    }

private:
    std::string text;
};

int main() {
    srand(time(NULL));

    MainWindow mw;

    // ScrollArea* sq = new ScrollArea(nullptr, &mw);
    // sq->set_layout(new VBoxLayout);

    std::string longtext = 
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n";

    Label* textbox = new Label(longtext, &mw);
    // textbox->set_scrolling(true);
    // textbox->debug_set_plane_color(200,0,0);
    // sq->set_content(textbox);

    mw.get_layout()->add_widget(textbox);
    // sq->set_focus();

    mw.show();
}
