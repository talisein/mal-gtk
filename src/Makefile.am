bin_PROGRAMS = mal-gtk

# list of sources for the 'helloWorld' binary
INCLUDES = $(CURL_CFLAGS) $(GLIBMM_CFLAGS) $(GTKMM_CFLAGS) $(LIBXML_CFLAGS) $(LIBSECRET_CFLAGS)
mal_gtk_LDADD = $(CURL_LIBS) $(GLIBMM_LIBS) $(GTKMM_LIBS) $(LIBXML_LIBS) $(LIBSECRET_LIBS)

AM_CXXFLAGS=-Wall -Wextra -g -pedantic -fno-omit-frame-pointer 
AM_CFLAGS=-Wall -std=c99 -fno-omit-frame-pointer

mal_gtk_SOURCES = main.cpp application.hpp application.cpp user_info.cpp user_info.hpp password_dialog.cpp password_dialog.hpp anime_list_view.cpp anime_list_view.hpp main_window.cpp main_window.hpp anime.cpp anime.hpp anime_serializer.cpp anime_serializer.hpp mal.cpp mal.hpp manga_serializer.cpp manga_serializer.hpp manga.hpp manga.cpp manga_list_view.cpp manga_list_view.hpp malitem.cpp malitem.hpp malitem_list_view.cpp malitem_list_view.hpp increment_entry.cpp increment_entry.hpp fancy_label.hpp fancy_label.cpp text_util.cpp text_util.hpp date_widgets.cpp date_widgets.hpp



