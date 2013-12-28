#pragma once

#include <gtkmm.h>
#include <gtkmm/cellrenderercombo.h>

typedef struct _MalgtkCellRendererScore MalgtkCellRendererScore;
typedef struct _MalgtkCellRendererScoreClass MalgtkCellRendererScoreClass;

namespace MAL
{ class CellRendererScore_Class; } // namespace MAL

namespace MAL {

    class CellRendererScore : public Gtk::CellRendererCombo {
    public:
        typedef CellRendererScore CppObjectType;
        typedef CellRendererScore_Class CppClassType;
        typedef MalgtkCellRendererScore BaseObjectType;
        typedef MalgtkCellRendererScoreClass BaseClassType;

        virtual ~CellRendererScore();

    private:
        friend class CellRendererScore_Class;
        static CppClassType cellrendererscore_class_;

        // noncopyable
        CellRendererScore(const CellRendererScore&);
        CellRendererScore& operator=(const CellRendererScore&);

    protected:
        explicit CellRendererScore(const Glib::ConstructParams& params);
        explicit CellRendererScore(MalgtkCellRendererScore* castitem);

    public:
        static GType get_type()      G_GNUC_CONST;
        static GType get_base_type() G_GNUC_CONST;
        MalgtkCellRendererScore*       gobj()       { return reinterpret_cast<MalgtkCellRendererScore*>(gobject_); }
        const MalgtkCellRendererScore* gobj() const { return reinterpret_cast<MalgtkCellRendererScore*>(gobject_); }


    public:
        CellRendererScore();

        Glib::PropertyProxy< int > property_score();

        int get_score() const;
    };
}

namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   * 
   * @relates Gtk::CellRendererScore
   */
    MAL::CellRendererScore* wrap(MalgtkCellRendererScore* object, bool take_copy = false);
} //namespace Glib
