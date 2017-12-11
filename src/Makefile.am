bin_PROGRAMS = mal-gtk

AM_CPPFLAGS = $(CURL_CFLAGS) $(GLIBMM_CFLAGS) $(GTKMM_CFLAGS) $(LIBXML_CFLAGS) $(LIBSECRET_CFLAGS)
mal_gtk_LDADD = $(CURL_LIBS) $(GLIB_LIBS) $(GLIBMM_LIBS) $(GTKMM_LIBS) $(LIBXML_LIBS) $(LIBSECRET_LIBS)

AM_CXXFLAGS=-fno-omit-frame-pointer
AM_CFLAGS=-std=c11 $(GLIB_CFLAGS)

mal_gtk_CFLAGS=-std=c11 $(WARN_CFLAGS)
mal_gtk_LDFLAGS=$(WARN_LDFLAGS)
mal_gtk_CXXFLAGS=-Wall -Wextra -pedantic $(WARN_CXXFLAGS) -Wno-redundant-decls -I../libmalgtk

mal_gtk_SOURCES = main.cpp                                                   \
                  application.cpp                  application.hpp           \
                  user_info.cpp                    user_info.hpp             \
                  mal.cpp                          mal.hpp                   \
                  malitem.cpp                      malitem.hpp               \
                  anime.cpp                        anime.hpp                 \
                  manga.cpp                        manga.hpp                 \
                  xml_reader.cpp                   xml_reader.hpp            \
                  xml_writer.cpp                   xml_writer.hpp            \
                  anime_serializer.cpp             anime_serializer.hpp      \
                  manga_serializer.cpp             manga_serializer.hpp      \
                  text_util.cpp                    text_util.hpp             \
                                                   active.hpp                \
                                                   message_dispatcher.hpp    \
                                                   callback_dispatcher.hpp   \
                  gui/malgtk_cellrenderer_score.c  gui/malgtk_cellrenderer_score.h \
                  gui/cellrendererscore.cpp        gui/cellrendererscore.hpp \
                  gui/private/cellrendererscore_p.hpp                        \
                  gui/main_window.cpp              gui/main_window.hpp       \
                  gui/password_dialog.cpp          gui/password_dialog.hpp   \
                  gui/malitem_list_view.cpp        gui/malitem_list_view.hpp \
                  gui/anime_list_view.cpp          gui/anime_list_view.hpp   \
                  gui/manga_list_view.cpp          gui/manga_list_view.hpp   \
                  gui/increment_entry.cpp          gui/increment_entry.hpp   \
                  gui/fancy_label.cpp              gui/fancy_label.hpp       \
                  gui/date_widgets.cpp             gui/date_widgets.hpp


