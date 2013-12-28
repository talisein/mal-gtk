#pragma once

#include <gtkmm/private/cellrenderercombo_p.h>

#include <glibmm/class.h>

namespace MAL
{

class CellRendererScore_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef CellRendererScore CppObjectType;
  typedef MalgtkCellRendererScore BaseObjectType;
  typedef MalgtkCellRendererScoreClass BaseClassType;
  typedef Gtk::CellRendererCombo_Class CppClassParent;
  typedef GtkCellRendererComboClass BaseClassParent;

  friend class CellRendererScore;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.

  //Callbacks (virtual functions):
};


} // namespace Gtk
