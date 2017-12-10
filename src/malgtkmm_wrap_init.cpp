#include <glibmm.h>
#include "malgtkmm_wrap_init.hpp"
#include "malgtkmm_malitem.hpp"
#include <glibmm/init.h>

extern "C"
{
/*    GType malgtk_malitem_get_type(void); */
}

namespace MALnew
{
    void wrap_init()
    {
        Glib::wrap_register(malgtk_malitem_get_type(), &Malitem_Class::wrap_new);

        g_type_ensure(Malitem::get_type());
    }

    void init()
    {
        static bool s_init = false;
        if (!s_init) {
            Glib::init();
            MALnew::wrap_init();
            s_init = true;
        }
    }
}

