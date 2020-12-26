gobj_dep = dependency('gobject-2.0', version : '>=2.44.0')
glib_dep = dependency('glib-2.0',    version : '>=2.44.0')
xml_dep  = dependency('libxml-2.0',  version : '>=2.7.8')

libmalgtk_deps = [gobj_dep, glib_dep, xml_dep]

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

gnome = import('gnome')

libmalgtk_enums = gnome.mkenums_simple('malgtk_enumtypes',
                                       sources : ['malgtk_malitem.h', 'malgtk_anime.h', 'malgtk_manga.h'])

libmalgtk_inc = include_directories('.')

libmalgtk = static_library('malgtk', libmalgtk_srcs, libmalgtk_enums,
                            dependencies : libmalgtk_deps,
                            include_directories: libmalgtk_inc)

libmalgtk_dep = declare_dependency(link_with: libmalgtk,
                                   include_directories: libmalgtk_inc,
                                   dependencies: libmalgtk_deps)

subdir('tests')
