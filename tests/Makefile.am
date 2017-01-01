include $(top_srcdir)/glib-tap.mk

AM_CFLAGS = -g $(MALGTK_CFLAGS) $(MAINTAINER_CFLAGS) $(GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBXML_CFLAGS) -Werror -Wall
LDADD = $(GLIB_LIBS) $(GOBJECT_LIBS) $(LIBXML_LIBS) $(top_srcdir)/src/malgtk_manga.o $(top_srcdir)/src/malgtk_malitem.o $(top_srcdir)/src/malgtk_enumtypes.o $(top_srcdir)/src/malgtk_date.o $(top_srcdir)/src/malgtk_anime.o $(top_srcdir)/src/malgtk_gtree.o $(top_srcdir)/src/malgtk_xml.o

AM_CPPFLAGS = \
	$(MALGTK_DEBUG_CFLAGS) \
	-I$(top_srcdir)	\
	-I$(top_builddir) \
	-I$(top_srcdir)/src \
	$(NULL)


test_programs = \
	malitem \
	manga \
	anime \
	$(NULL)


