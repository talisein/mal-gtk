#pragma once

#include <glibmm.h>
#include <glibmm/private/object_p.h>
#include <glibmm/class.h>

#include "malgtk_malitem.h"
#include "malgtkmm_date.hpp"

using MalgtkMalitem = struct _MalgtkMalitem;
using MalgtkMalitemClass = struct _MalgtkMalitemClass;


/* PRIVATE */
namespace MALnew
{
    class Malitem;
    
    class Malitem_Class : public Glib::Class
    {
    public:
        using CppObjectType = Malitem;
        using BaseObjectType = MalgtkMalitem;
        using BaseClassType = MalgtkMalitemClass;
        using CppClassParent = Glib::Object_Class;
        using BaseClassParent = GObjectClass;

        friend class Malitem;

        const Glib::Class& init();

        static void class_init_function(void* g_class, void* class_data);

        static Glib::ObjectBase* wrap_new(GObject*);
    };
} // namespace MAL
/* END PRIVATE */

namespace MALnew {
    /* Enums here */
    enum class Priority : int {
        INVALID = MALGTK_MALITEM_PRIORITY_INVALID,
        LOW     = MALGTK_MALITEM_PRIORITY_LOW,
        MEDIUM  = MALGTK_MALITEM_PRIORITY_MEDIUM,
        HIGH    = MALGTK_MALITEM_PRIORITY_HIGH
    };

    enum class ReconsumeValue : int {
        INVALID   = MALGTK_MALITEM_RECONSUME_VALUE_INVALID,
        VERY_LOW  = MALGTK_MALITEM_RECONSUME_VALUE_VERY_LOW,
        LOW       = MALGTK_MALITEM_RECONSUME_VALUE_LOW,
        MEDIUM    = MALGTK_MALITEM_RECONSUME_VALUE_MEDIUM,
        HIGH      = MALGTK_MALITEM_RECONSUME_VALUE_HIGH,
        VERY_HIGH = MALGTK_MALITEM_RECONSUME_VALUE_VERY_HIGH,
    };
}

namespace Glib {
    template<>
    class Value<MALnew::Priority> : public Glib::Value_Flags<MALnew::Priority>
    {
    public:
        static GType value_type() G_GNUC_CONST;
    };

    template<>
    class Value<MALnew::ReconsumeValue> : public Glib::Value_Flags<MALnew::ReconsumeValue>
    {
    public:
        static GType value_type() G_GNUC_CONST;
    };
}

namespace MALnew {
    class Malitem : public Glib::Object {
    public:
        using CppObjectType  = Malitem;
        using CppClassType   = Malitem_Class;
        using BaseObjectType = MalgtkMalitem;
        using BaseClassType  = MalgtkMalitemClass;

        Malitem(const Malitem&) = delete;
        Malitem& operator=(const Malitem&) = delete;

    private:
        friend class Malitem_Class;
        static CppClassType malitem_class_;

    protected:
        explicit Malitem(const Glib::ConstructParams& construct_params);
        explicit Malitem(MalgtkMalitem* castitem);
        Malitem();
        
    public:
        Malitem(Malitem&& src) noexcept;
        Malitem& operator=(Malitem&& src) noexcept;

        ~Malitem() noexcept override;

        static GType get_type();
        static GType get_base_type();
        MalgtkMalitem*       gobj()       { return reinterpret_cast<MalgtkMalitem*>(gobject_); }
        const MalgtkMalitem* gobj() const { return reinterpret_cast<MalgtkMalitem*>(gobject_); }
        MalgtkMalitem* gobj_copy();

    public:
        static Glib::RefPtr<Malitem> create();

        /* properties */
        Glib::PropertyProxy< int64_t > property_mal_db_id();
        Glib::PropertyProxy< Glib::ustring > property_series_title();
        Glib::PropertyProxy< Glib::ustring > property_preferred_title();
        Glib::PropertyProxy< MAL::Date > property_series_date_begin();
        Glib::PropertyProxy< MAL::Date > property_series_date_end();
        Glib::PropertyProxy_ReadOnly< Glib::ustring > property_season_begin() const;
        Glib::PropertyProxy_ReadOnly< Glib::ustring > property_season_end() const;
        Glib::PropertyProxy< std::string > property_image_url();
        Glib::PropertyProxy< std::vector<Glib::ustring> > property_series_synonyms();
        Glib::PropertyProxy< Glib::ustring > property_series_synopsis();
        Glib::PropertyProxy< std::vector<Glib::ustring> > property_tags();
        Glib::PropertyProxy< Glib::Date > property_date_start();
        Glib::PropertyProxy< Glib::Date > property_date_finish();
        Glib::PropertyProxy< int64_t > property_id();
        Glib::PropertyProxy< Glib::DateTime > property_last_updated();
        Glib::PropertyProxy< double > property_score();
        Glib::PropertyProxy< bool > property_enable_reconsuming();
        Glib::PropertyProxy< Glib::ustring > property_fansub_group();
        Glib::PropertyProxy< Glib::ustring > property_comments();
        Glib::PropertyProxy< int > property_downloaded_items();
        Glib::PropertyProxy< int > property_times_consumed();
        Glib::PropertyProxy< MALnew::ReconsumeValue > property_reconsume_value();
        Glib::PropertyProxy< MALnew::Priority > property_priority();
        Glib::PropertyProxy< bool > property_enable_discussion();
        Glib::PropertyProxy< bool > property_has_details();
        
        /* methods */
        void set_from_xml(xmlTextReaderPtr reader);
        void get_xml(xmlTextWriterPtr writer) const;
        void add_synonym(const Glib::ustring& synonym);
        void add_tag(const Glib::ustring& tag);
        void foreach_synonym(const std::function<void (const Glib::ustring&)>& cb);
        void foreach_tag(const std::function<void (const Glib::ustring&)>& cb);
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
   */
    Glib::RefPtr<MALnew::Malitem> wrap(MalgtkMalitem* object, bool take_copy = false);
} //namespace Glib
