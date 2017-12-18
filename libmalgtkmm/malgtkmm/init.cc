#include "init.h"
#include "wrap_init.h"
#include <gtkmm.h>

namespace MALnew
{
    void init()
    {
        Gtk::Main::init_gtkmm_internals();
        wrap_init();
    }
}
