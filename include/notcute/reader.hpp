#pragma once

#include <ncpp/Reader.hh>
#include <ncpp/Widget.hh>


namespace notcute {

class Reader : public ncpp::Widget
{
public:
    explicit Reader (ncpp::Plane *plane, const ncreader_options *opts)
        : Widget (ncpp::Utilities::get_notcurses_cpp (plane))
    {
        ensure_valid_plane (plane);
        common_init (ncpp::Utilities::to_ncplane (plane), opts);
        take_plane_ownership (plane);
    }

    explicit Reader (ncpp::Plane &plane, const ncreader_options *opts)
        : Widget (ncpp::Utilities::get_notcurses_cpp (plane))
    {
        ensure_valid_plane (plane);
        common_init (ncpp::Utilities::to_ncplane (plane), opts);
        take_plane_ownership (plane);
    }

    ~Reader ()
    {
        if (!is_notcurses_stopped ())
            ncreader_destroy (reader, nullptr);
    }

    bool clear () const NOEXCEPT_MAYBE
    {
        bool ret = ncreader_clear (reader) != 0;
        return error_guard_cond<bool, bool> (ret, ret);
    }

    //
    // None of the move* methods should throw since their return value (0 - moved, -1 - not moved) appear to be
    // purely informational, not errors per se. If we had `can_move*` functions then `move*` could throw exceptions,
    // potentially.
    //
    int move_left () const noexcept
    {
        return ncreader_move_left (reader);
    }

    int move_right () const noexcept
    {
        return ncreader_move_right (reader);
    }

    int move_up () const noexcept
    {
        return ncreader_move_up (reader);
    }

    int move_down () const noexcept
    {
        return ncreader_move_down (reader);
    }

    bool write_egc (const char *egc) const NOEXCEPT_MAYBE
    {
        return error_guard (ncreader_write_egc (reader, egc), -1);
    }

    char* get_contents () const noexcept
    {
        return ncreader_contents(reader);
    }

    ncpp::Plane* get_plane () const noexcept
    {
        return ncpp::Plane::map_plane (ncreader_plane (reader));
    }

    ncreader* to_ncreader() const noexcept
    {
        return reader;
    }

    bool offer_input(ncinput* ni) {
        return ncreader_offer_input(reader, ni);
    }

private:
    void common_init (ncplane *n, const ncreader_options *opts)
    {
        reader = ncreader_create (n, opts);
        if (reader == nullptr)
            throw ncpp::init_error ("Notcurses failed to create a new reader");
    }

private:
    ncreader *reader;
};

}
