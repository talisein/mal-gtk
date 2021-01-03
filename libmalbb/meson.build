libmalbb_src = [
  'rng.cpp',
  'oauth.cpp'
]

libmalbb_deps = [glibmm_dep, gtkmm_dep, pcg_cpp_dep]

libmalbb_inc = include_directories('.')
libmalbb = static_library('malbb', libmalbb_src,
                          dependencies: libmalbb_deps,
                          include_directories: libmalbb_inc)

libmalbb_dep = declare_dependency(link_with: libmalbb,
                                  include_directories: libmalbb_inc,
                                  dependencies: libmalbb_deps)

subdir('tests')
