include $(top_srcdir)/glib-tap.mk

AM_CFLAGS = -g $(MALGTK_CFLAGS) $(MAINTAINER_CFLAGS)
LDADD = $(GLIB_LIBS) $(GOBJECT_LIBS) $(top_srcdir)/src/malgtk_malitem.o $(top_srcdir)/src/malgtk_enumtypes.o $(top_srcdir)/src/malgtk_date.o

AM_CPPFLAGS = \
	$(MALGTK_DEBUG_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src

test_programs = \
	malitem \
	$(NULL)


