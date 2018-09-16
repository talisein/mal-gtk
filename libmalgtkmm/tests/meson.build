malitemmm = executable('malitemmm_tests', 'malitem.cpp',
                       dependencies: libmalgtkmm_dep,
                       override_options: ['warning_level=1', 'werror=false'])

test('malitemmm', malitemmm, args : '--tap')
