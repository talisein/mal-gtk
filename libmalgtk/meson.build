libmalgtk_version = '0.4.0'
libmalgtk_soversion = '0'

gobj_dep = dependency('gobject-2.0', version : '>=2.44.0')
glib_dep = dependency('glib-2.0',    version : '>=2.44.0')
xml_dep  = dependency('libxml-2.0',  version : '>=2.7.8')

deps = [gobj_dep, glib_dep, xml_dep]

gnome = import('gnome')

libmalgtk_srcs = files(['malgtk_anime.c',
                        'malgtk_date.c',
                        'malgtk_gtree.c',
                        'malgtk_malitem.c',
                        'malgtk_manga.c',
                        'malgtk_xml.c'])

libmalgtk_hdrs = files(['malgtk_anime.h',
                        'malgtk_date.h',
                        'malgtk_gtree.h',
                        'malgtk_malitem.h',
                        'malgtk_manga.h',
                        'malgtk_xml.h'])

libmalgtk_enums = gnome.mkenums('malgtk_enumtypes',
                                c_template : 'malgtk_enumtypes.c.template',
                                h_template : 'malgtk_enumtypes.h.template',
                                sources: ['malgtk_malitem.h', 'malgtk_anime.h', 'malgtk_manga.h'])

libmalgtk = shared_library('malgtk', libmalgtk_srcs, libmalgtk_enums,
                            version      : libmalgtk_version,
                            soversion    : libmalgtk_soversion,
                            dependencies : deps, install: true)

libmalgtk_inc = include_directories('.')

subdir('tests')