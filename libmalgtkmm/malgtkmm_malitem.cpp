#include "malgtkmm_malitem.hpp"
#include "malgtk_enumtypes.h"

GType Glib::Value<MALnew::Priority>::value_type()
{
    return malgtk_malitem_priority_get_type();
}

GType Glib::Value<MALnew::ReconsumeValue>::value_type()
{
    return malgtk_malitem_reconsume_value_get_type();
}

namespace MALnew {

    const Glib::Class& MALnew::Malitem_Class::init()
    {
        if(!gtype_) // create the GType if necessary
        {
            // Glib::Class has to know the class init function to clone custom types.
            class_init_func_ = &MALnew::Malitem_Class::class_init_function;

            // This is actually just optimized away, apparently with no harm.
            // Make sure that the parent type has been created.
            //CppClassParent::CppObjectType::get_type();

            // Create the wrapper type, with the same class/instance size as the base type.
            register_derived_type(malgtk_malitem_get_type());

            // Add derived versions of interfaces, if the C type implements any interfaces:

        }

        return *this;
    }

    
    void Malitem_Class::class_init_function(void* g_class, void* class_data)
    {
        BaseClassType *const klass = static_cast<BaseClassType*>(g_class);
        CppClassParent::class_init_function(klass, class_data);
    }


    Glib::ObjectBase* Malitem_Class::wrap_new(GObject* o)
    {
        return new Malitem((MalgtkMalitem*)o);
    }


    MalgtkMalitem* Malitem::gobj_copy()
    {
        reference();
        return gobj();
    }

    Malitem::Malitem(const Glib::ConstructParams& construct_params) :
        Glib::Object(construct_params)
    {
    }

    Malitem::Malitem(MalgtkMalitem* castitem) :
        Glib::Object((GObject*)(castitem))
    {
    }

    Malitem::Malitem() :
        Glib::ObjectBase(nullptr),
        Glib::Object(Glib::ConstructParams(malitem_class_.init()))
    {
    }

    Malitem::Malitem(Malitem&& src) noexcept :
        Glib::Object(std::move(src))
    {
    }

    Malitem& Malitem::operator=(Malitem&& src) noexcept
    {
        Glib::Object::operator=(std::move(src));
        return *this;
    }

    Malitem::~Malitem() noexcept
    {
    }

    Malitem::CppClassType Malitem::malitem_class_; // initialize static member
  
    GType Malitem::get_type()
    {
        return malitem_class_.init().get_type();
    }

    GType Malitem::get_base_type()
    {
        return malgtk_malitem_get_type();
    }

    Glib::RefPtr<Malitem> Malitem::create()
    {
        return Glib::RefPtr<Malitem>( new MALnew::Malitem() );
    }

    /* Properties */
    Glib::PropertyProxy< int64_t > Malitem::property_mal_db_id()
    {
        return Glib::PropertyProxy< int64_t >(this, "mal-db-id");
    }    
    
    Glib::PropertyProxy< Glib::ustring >
    Malitem::property_series_title()
    {
        return Glib::PropertyProxy< Glib::ustring >(this, "series_title");
    }
    
    Glib::PropertyProxy< Glib::ustring >
    Malitem::property_preferred_title()
    {
        return Glib::PropertyProxy< Glib::ustring >(this, "preferred_title");
    }
    
    Glib::PropertyProxy< MAL::Date >
    Malitem::property_series_date_begin()
    {
        return Glib::PropertyProxy< MAL::Date >(this, "series_date_begin");
    }
    
    Glib::PropertyProxy< MAL::Date >
    Malitem::property_series_date_end()
    {
        return Glib::PropertyProxy< MAL::Date >(this, "series_date_end");
    }
    
    Glib::PropertyProxy_ReadOnly< Glib::ustring >
    Malitem::property_season_begin() const
    {
        return Glib::PropertyProxy_ReadOnly< Glib::ustring >(this, "season_begin");
    }
    
    Glib::PropertyProxy_ReadOnly< Glib::ustring >
    Malitem::property_season_end() const
    {
        return Glib::PropertyProxy_ReadOnly< Glib::ustring >(this, "season_end");
    }
    
    Glib::PropertyProxy< std::string >
    Malitem::property_image_url()
    {
        return Glib::PropertyProxy< std::string >(this, "image_url");
    }
    
    Glib::PropertyProxy< std::vector<Glib::ustring> >
    Malitem::property_series_synonyms()
    {
        return Glib::PropertyProxy< std::vector<Glib::ustring> >(this, "series_synonyms");
    }
    
    Glib::PropertyProxy< Glib::ustring >
    Malitem::property_series_synopsis()
    {
        return Glib::PropertyProxy< Glib::ustring >(this, "series_synopsis");
    }
    
    Glib::PropertyProxy< std::vector<Glib::ustring> >
    Malitem::property_tags()
    {
        return Glib::PropertyProxy< std::vector<Glib::ustring> >(this, "tags");
    }
    
    Glib::PropertyProxy< Glib::Date >
    Malitem::property_date_start()
    {
        return Glib::PropertyProxy< Glib::Date >(this, "date_start");
    }
    
    Glib::PropertyProxy< Glib::Date >
    Malitem::property_date_finish()
    {
        return Glib::PropertyProxy< Glib::Date >(this, "date_finish");
    }
    
    Glib::PropertyProxy< int64_t >
    Malitem::property_id()
    {
        return Glib::PropertyProxy< int64_t >(this, "id");
    }
    
    Glib::PropertyProxy< Glib::DateTime >
    Malitem::property_last_updated()
    {
        return Glib::PropertyProxy< Glib::DateTime >(this, "last_updated");
    }
    
    Glib::PropertyProxy< double >
    Malitem::property_score()
    {
        return Glib::PropertyProxy< double >(this, "score");
    }
    
    Glib::PropertyProxy< bool >
    Malitem::property_enable_reconsuming()
    {
        return Glib::PropertyProxy< bool >(this, "enable_reconsuming");
    }
    
    Glib::PropertyProxy< Glib::ustring >
    Malitem::property_fansub_group()
    {
        return Glib::PropertyProxy< Glib::ustring >(this, "fansub_group");
    }
    
    Glib::PropertyProxy< Glib::ustring >
    Malitem::property_comments()
    {
        return Glib::PropertyProxy< Glib::ustring >(this, "comments");
    }
    
    Glib::PropertyProxy< int >
    Malitem::property_downloaded_items()
    {
        return Glib::PropertyProxy< int >(this, "downloaded_items");
    }
    
    Glib::PropertyProxy< int >
    Malitem::property_times_consumed()
    {
        return Glib::PropertyProxy< int >(this, "times_consumed");
    }
    
    Glib::PropertyProxy< MALnew::ReconsumeValue >
    Malitem::property_reconsume_value()
    {
        return Glib::PropertyProxy< MALnew::ReconsumeValue >(this, "reconsume_value");
    }
    
    Glib::PropertyProxy< MALnew::Priority >
    Malitem::property_priority()
    {
        return Glib::PropertyProxy< MALnew::Priority >(this, "priority");
    }
    
    Glib::PropertyProxy< bool >
    Malitem::property_enable_discussion()
    {
        return Glib::PropertyProxy< bool >(this, "enable_discussion");
    }
    
    Glib::PropertyProxy< bool >
    Malitem::property_has_details()
    {
        return Glib::PropertyProxy< bool >(this, "has_details");
    }
    
    /* Methods */
    void
    Malitem::set_from_xml(xmlTextReaderPtr reader)
    {
        malgtk_malitem_set_from_xml(gobj(), reader);
    }
    
    void
    Malitem::get_xml(xmlTextWriterPtr writer) const
    {
        malgtk_malitem_get_xml(gobj(), writer);
    }
    
    void
    Malitem::add_synonym(const Glib::ustring& synonym)
    {
        malgtk_malitem_add_synonym(gobj(), synonym.c_str());
    }
    
    void
    Malitem::add_tag(const Glib::ustring& tag)
    {
        malgtk_malitem_add_synonym(gobj(), tag.c_str());
    }

    namespace {
        using cb_t = std::function<void (const Glib::ustring&)>;
        extern "C" {
            static gboolean foreach_wrapper(const gchar *str, gpointer user_data)
            {
                const cb_t* cb = static_cast<const cb_t*>(user_data);
                cb->operator()(Glib::convert_const_gchar_ptr_to_ustring(str));

                return FALSE;
            }
        }
    }
    
    void
    Malitem::foreach_synonym(const std::function<void (const Glib::ustring&)>& cb)
    {
        malgtk_malitem_foreach_synonym(gobj(), &foreach_wrapper, &const_cast<std::function<void (const Glib::ustring&)>&>(cb));
    }
    
    void
    Malitem::foreach_tag(const std::function<void (const Glib::ustring&)>& cb)
    {
        malgtk_malitem_foreach_tag(gobj(), &foreach_wrapper, &const_cast<std::function<void (const Glib::ustring&)>&>(cb));
    }
    
}

namespace Glib
{
    Glib::RefPtr<MALnew::Malitem> wrap(MalgtkMalitem* object, bool take_copy)
    {
        return Glib::RefPtr<MALnew::Malitem>( dynamic_cast<MALnew::Malitem*> (Glib::wrap_auto ((GObject*)(object), take_copy)));
    }
}

