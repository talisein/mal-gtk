#include <glibmm.h>
#include "cellrendererscore.hpp"
#include "private/cellrendererscore_p.hpp"
#include "malgtk_cellrenderer_score.h"
#include <gtk/gtk.h>

namespace Glib
{

MAL::CellRendererScore* wrap(MalgtkCellRendererScore* object, bool take_copy)
{
  return dynamic_cast<MAL::CellRendererScore *> (Glib::wrap_auto ((GObject*)(object), take_copy));
}

} /* namespace Glib */


namespace MAL {

    const Glib::Class& CellRendererScore_Class::init()
    {
        if(!gtype_) // create the GType if necessary
        {
            // Glib::Class has to know the class init function to clone custom types.
            class_init_func_ = &CellRendererScore_Class::class_init_function;

            // This is actually just optimized away, apparently with no harm.
            // Make sure that the parent type has been created.
            //CppClassParent::CppObjectType::get_type();

            // Create the wrapper type, with the same class/instance size as the base type.
            register_derived_type(malgtk_cell_renderer_score_get_type());

            // Add derived versions of interfaces, if the C type implements any interfaces:

        }

        return *this;
    }

    void CellRendererScore_Class::class_init_function(void* g_class, void* class_data)
    {
        BaseClassType *const klass = static_cast<BaseClassType*>(g_class);
        CppClassParent::class_init_function(klass, class_data);
    }


    Glib::ObjectBase* CellRendererScore_Class::wrap_new(GObject* o)
    {
        return manage(new CellRendererScore((MalgtkCellRendererScore*)(o)));

    }


/* The implementation: */

    CellRendererScore::CellRendererScore(const Glib::ConstructParams& construct_params)
        :
        Gtk::CellRendererCombo(construct_params)
    {
    }

    CellRendererScore::CellRendererScore(MalgtkCellRendererScore* castitem)
        :
        Gtk::CellRendererCombo((GtkCellRendererCombo*)(castitem))
    {
    }

    CellRendererScore::~CellRendererScore()
    {
        destroy_();
    }

    CellRendererScore::CppClassType CellRendererScore::cellrendererscore_class_; // initialize static member
  
    GType CellRendererScore::get_type()
    {
        return cellrendererscore_class_.init().get_type();
    }


    GType CellRendererScore::get_base_type()
    {
        return malgtk_cell_renderer_score_get_type();
    }


    CellRendererScore::CellRendererScore()
        :
        // Mark this class as non-derived to allow C++ vfuncs to be skipped.
        Glib::ObjectBase(0),
        Gtk::CellRendererCombo(Glib::ConstructParams(cellrendererscore_class_.init()))
    {
    }


    Glib::PropertyProxy< int > CellRendererScore::property_score()
    {
        return Glib::PropertyProxy< int >(this, "score");
    }

    int CellRendererScore::get_score() const
    {
        return malgtk_cell_renderer_score_get_score(gobj());
    }
}
