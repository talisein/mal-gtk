bin_PROGRAMS = mal-gtk

# list of sources for the 'helloWorld' binary
INCLUDES = $(CURL_CFLAGS) $(GLIBMM_CFLAGS) $(GTKMM_CFLAGS) $(LIBXML_CFLAGS) $(LIBSECRET_CFLAGS)
mal_gtk_LDADD = $(CURL_LIBS) $(GLIBMM_LIBS) $(GTKMM_LIBS) $(LIBXML_LIBS) $(LIBSECRET_LIBS)

AM_CXXFLAGS=-Wall -Wextra -g -pedantic -fno-omit-frame-pointer 
AM_CFLAGS=-Wall -std=c99 -fno-omit-frame-pointer

mal_gtk_SOURCES = main.cpp                                            \
				  application.cpp           application.hpp           \
				  user_info.cpp             user_info.hpp             \
		  		  mal.cpp                   mal.hpp                   \
		  		  malitem.cpp               malitem.hpp               \
		  		  anime.cpp                 anime.hpp                 \
		  		  manga.cpp                 manga.hpp                 \
		  		  anime_serializer.cpp      anime_serializer.hpp      \
		  		  manga_serializer.cpp      manga_serializer.hpp      \
		  		  text_util.cpp             text_util.hpp             \
		  		                            active.hpp                \
		  		  gui/main_window.cpp       gui/main_window.hpp       \
		  		  gui/password_dialog.cpp   gui/password_dialog.hpp   \
		  		  gui/malitem_list_view.cpp gui/malitem_list_view.hpp \
		  		  gui/anime_list_view.cpp   gui/anime_list_view.hpp   \
		  		  gui/manga_list_view.cpp   gui/manga_list_view.hpp   \
		  		  gui/increment_entry.cpp   gui/increment_entry.hpp   \
		  		  gui/fancy_label.cpp       gui/fancy_label.hpp       \
		  		  gui/date_widgets.cpp      gui/date_widgets.hpp



