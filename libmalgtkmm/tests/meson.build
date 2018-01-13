malitemmm = executable('malitemmm_tests', 'malitem.cpp',
                     include_directories: libmalgtkmm_inc,
                     link_with: libmalgtkmm,
                     dependencies: libmalgtkmm_deps,
                     override_options: ['warning_level=1', 'werror=false'])

test('malitemmm', malitemmm, args : '--tap')