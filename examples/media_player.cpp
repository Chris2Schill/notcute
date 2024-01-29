#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/visual.hpp>
#include <notcute/button.hpp>


namespace nq = notcute;

class MediaPlayer : public nq::FrameWidget {
public:
    MediaPlayer(std::string& filename, nq::Widget* parent = nullptr)
        :nq::FrameWidget(parent)
    {
        set_layout(new nq::VBoxLayout);
        set_focus_policy(nq::FocusPolicy::FOCUS);
        set_title("Media Player");

        ui.video = nq::Visual::from_file(filename, this);
        ui.controls = new nq::Button(this);
        ui.blitters = new nq::Widget(this);
        ui.blitters->set_layout(new nq::HBoxLayout(3, 0));
        ui.blitters->get_layout()->set_behave(LAY_HFILL);

        ui.video->get_layout()->set_margins_ltrb(1,1,1,0);
        ui.controls->get_layout()->set_margins_ltrb(1,0,1,0);
        ui.blitters->get_layout()->set_margins_ltrb(1,0,1,1);


        ui.blit1x1 = new nq::Button("1x1");
        ui.blit2x1 = new nq::Button("2x1");
        ui.blit2x2 = new nq::Button("2x2");
        ui.blit3x2 = new nq::Button("3x2");
        ui.blitbraille = new nq::Button("BRAILLE");
        ui.blitpixel = new nq::Button("PIXEL");
        ui.blit4x1 = new nq::Button("4x1");
        ui.blit8x1 = new nq::Button("8x1");

        ui.blit1x1->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_1x1); });
        ui.blit2x1->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_2x1); });
        ui.blit2x2->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_2x2); });
        ui.blit3x2->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_3x2); });
        ui.blitbraille->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_BRAILLE); });
        ui.blitpixel->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_PIXEL); });
        ui.blit4x1->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_4x1); });
        ui.blit8x1->clicked.connect([this](){ ui.video->set_blitter(NCBLIT_8x1); });

        ui.blitters->get_layout()->add_widget(ui.blit1x1);
        ui.blitters->get_layout()->add_widget(ui.blit2x1);
        ui.blitters->get_layout()->add_widget(ui.blit2x2);
        ui.blitters->get_layout()->add_widget(ui.blit3x2);
        ui.blitters->get_layout()->add_widget(ui.blitbraille);
        ui.blitters->get_layout()->add_widget(ui.blitpixel);
        ui.blitters->get_layout()->add_widget(ui.blit4x1);
        ui.blitters->get_layout()->add_widget(ui.blit8x1);


        get_layout()->add_widget(ui.video);
        get_layout()->add_widget(ui.controls);
        get_layout()->add_widget(ui.blitters);

        set_focus();

        set_name("media_player");
        ui.video->set_name("video");
        ui.controls->set_name("BUTTON");
        ui.controls->get_layout()->set_behave(LAY_BOTTOM);
        ui.controls->set_label("PAUSE");

        ui.controls->clicked.connect([this](){
                nq::log_debug("CLICKED");

                if (ui.video->get_is_playing()) {
                    ui.video->set_is_playing(false);
                    ui.controls->set_label("PLAY");
                }
                else {
                    ui.video->set_is_playing(true);
                    ui.controls->set_label("PAUSE");
                }

            });
    }

private:
    struct ui{
        nq::Visual* video;
        nq::Button*  controls;
        nq::Widget* blitters;
        nq::Button* blit1x1;
        nq::Button* blit2x1;
        nq::Button* blit2x2;
        nq::Button* blit3x2;
        nq::Button* blitbraille;
        nq::Button* blitpixel;
        nq::Button* blit4x1;
        nq::Button* blit8x1;
    }ui;

};

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "no media file given as cmd line arg\n";
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    nq::MainWindow mw;
    mw.get_layout()->add_widget(new MediaPlayer(filename, &mw));
    mw.show();
}
