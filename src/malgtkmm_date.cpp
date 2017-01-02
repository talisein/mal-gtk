#include <glibmm/utility.h>
#include "malgtkmm_date.hpp"

namespace MAL
{
    Date::Date() :
        gobject_(malgtk_date_new())
    {
    }

    Date::Date(const Date& src) :
        gobject_(src.gobject_ ? malgtk_date_copy(src.gobject_) : nullptr)
    {
    }

    Date::Date(MalgtkDate* castitem, bool make_a_copy)
    {
        if (!make_a_copy) {
            gobject_ = castitem;
        } else {
            if (castitem)
                gobject_ = malgtk_date_copy(castitem);
            else
                gobject_ = nullptr;
        }
    }

    Date& Date::operator=(const Date& src)
    {
        MalgtkDate *const new_malgtkdate = src.gobject_ ? malgtk_date_copy(src.gobject_) : nullptr;

        if (gobject_)
            malgtk_date_free(gobject_);

        gobject_ = new_malgtkdate;

        return *this;
    }

    Date::~Date()
    {
        if (gobject_)
            malgtk_date_free(gobject_);
    }

    MalgtkDate*
    Date::gobj_copy() const
    {
        return malgtk_date_copy(gobject_);
    }

    Glib::Date::Day
    Date::get_day() const
    {
        return malgtk_date_get_day(gobject_);
    }
    
    Glib::Date::Month
    Date::get_month() const
    {
        return static_cast<Glib::Date::Month>(malgtk_date_get_month(gobject_));
    }
    
    Glib::Date::Year
    Date::get_year() const
    {
        return malgtk_date_get_year(gobject_);
    }
    
    Glib::Date
    Date::get_g_date() const
    {
        if (is_complete()) {
            return Glib::Date(get_day(), get_month(), get_year());
        } else {
            return Glib::Date();
        }
    }
    
    Glib::ustring
    Date::get_season() const
    {
        return Glib::convert_return_gchar_ptr_to_ustring(malgtk_date_get_season(gobject_));
    }

    void
    Date::set(Glib::Date::Day day, Glib::Date::Month month, Glib::Date::Year year)
    {
        malgtk_date_set_dmy(gobject_, day, static_cast<GDateMonth>(month), year);
    }
    
    void
    Date::set(const std::string& str)
    {
        malgtk_date_set_from_string(gobject_, str.c_str());
    }

    void
    Date::set(const Glib::ustring& str)
    {
        malgtk_date_set_from_string(gobject_, str.c_str());
    }

    Glib::ustring
    Date::get_string() const
    {
        return Glib::convert_return_gchar_ptr_to_ustring(malgtk_date_get_string(gobject_));
    }

    bool
    Date::is_complete() const
    {
        return malgtk_date_is_complete(gobject_);
    }

    bool
    Date::is_clear() const
    {
        return malgtk_date_is_clear(gobject_);
    }

    bool operator==(const Date& lhs, const Date& rhs)
    {
        return malgtk_date_is_equal(lhs.gobject_, rhs.gobject_);
    }

    bool operator!=(const Date& lhs, const Date& rhs)
    {
        return !malgtk_date_is_equal(lhs.gobject_, rhs.gobject_);
    }
}


namespace Glib
{
    MAL::Date wrap(MalgtkDate* object, bool take_copy)
    {
        return MAL::Date(object, take_copy);
    }
}
