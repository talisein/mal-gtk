project('mal-gtk', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.5.0',
        default_options : ['c_std=c18',
                           'cpp_std=c++2a',
                           'buildtype=release'])

gtk_dep    = dependency('gtk+-3.0')
cpp_compiler = meson.get_compiler('cpp')
if (gtk_dep.version().version_compare('<3.23.0') and cpp_compiler.has_argument('-Wno-parentheses'))
  add_project_arguments('-Wno-parentheses', language: 'cpp')
endif

cpp_compiler.has_header('version', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_constinit', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_constexpr_dynamic_alloc')
cpp_compiler.has_header_symbol('version', '__cpp_lib_span', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_constexpr_string_view', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_starts_ends_with', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_array_constexpr', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_integer_comparison_functions')
cpp_compiler.has_header_symbol('version', '__cpp_lib_bit_cast')
cpp_compiler.has_header_symbol('version', '__cpp_lib_stacktrace')
cpp_compiler.has_header_symbol('version', '__cpp_lib_source_location')

malgtk_datadir = get_option('datadir')
malgtk_pkgdatadir = join_paths(malgtk_datadir, meson.project_name())
malgtk_bindir = get_option('bindir')

update_desktop_database = find_program('update-desktop-database', required: false)
if (update_desktop_database.found())
  meson.add_install_script('meson_post_install.sh')
endif

boostut_proj = subproject('boostut')
boostut_dep = boostut_proj.get_variable('boostut_dep')

pcg_cpp_sp = subproject('pcg')
pcg_cpp_dep = pcg_cpp_sp.get_variable('pcg_cpp_dep')

subdir('libmalgtk')
subdir('tools')
subdir('libmalgtkmm')
subdir('src')
subdir('res')
