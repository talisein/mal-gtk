project('mal-gtk', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.4.0',
        default_options : ['c_std=c11',
                           'cpp_std=c++14',
                           'buildtype=release'])

gtk_dep    = dependency('gtk+-3.0')
cpp_compiler = meson.get_compiler('cpp')
if (gtk_dep.version().version_compare('<3.23.0') and cpp_compiler.has_argument('-Wno-parentheses'))
  add_project_arguments('-Wno-parentheses', language: 'cpp')
endif


subdir('libmalgtk')
subdir('tools')
subdir('libmalgtkmm')
subdir('src')
