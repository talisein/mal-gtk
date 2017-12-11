#pragma once

#include <glibmm.h>
#include "malgtk_date.h"

extern "C" { typedef struct _MalgtkDate MalgtkDate; }

namespace MAL
{ class Date; } // namespace MAL

namespace MAL {
    class Date
    {
    public:
        typedef Date CppObjectType;
        typedef MalgtkDate BaseObjectType;

        Date();
        explicit Date(MalgtkDate* castitem, bool make_a_copy = false);
        Date(const Date& src);
        Date& operator=(const Date& src);
        ~Date();

        MalgtkDate* gobj() { return gobject_; };
        const MalgtkDate* gobj() const { return gobject_; }

        MalgtkDate* gobj_copy() const;

    protected:
        MalgtkDate *gobject_;

    public:
        Glib::Date::Day    get_day() const;
        Glib::Date::Month  get_month() const;
        Glib::Date::Year   get_year() const;
        Glib::Date         get_g_date() const;
        Glib::ustring      get_season() const;
        void               set(Glib::Date::Day day, Glib::Date::Month month, Glib::Date::Year year);
        void               set(const std::string& str);
        void               set(const Glib::ustring& str);
        Glib::ustring      get_string() const;
        bool               is_complete() const;
        bool               is_clear() const;

        friend bool operator==(const Date& lhs, const Date& rhs);
        friend bool operator!=(const Date& lhs, const Date& rhs);
    };

    bool operator==(const Date& lhs, const Date& rhs);
    bool operator!=(const Date& lhs, const Date& rhs);
}

namespace Glib
{

  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates MAL::Date
   */
    MAL::Date wrap(MalgtkDate* object, bool take_copy = false);

} // namespace Glib
