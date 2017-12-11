libmalgtkmm_version = '0.4.0'
libmalgtkmm_soversion = '0'

glibmm_dep = dependency('glibmm-2.4', version : '>=2.44.0')

malgtkmm_deps = [glibmm_dep, xml_dep]

libmalgtkmm_srcs = files(['malgtkmm_date.cpp',
                          'malgtkmm_malitem.cpp',
                          'malgtkmm_wrap_init.cpp'])

libmalgtkmm = shared_library('malgtkmm', libmalgtkmm_srcs,
                             include_directories: libmalgtk_inc,
                             dependencies: malgtkmm_deps,
                             link_with: libmalgtk)

libmalgtkmm_inc = include_directories('.')
