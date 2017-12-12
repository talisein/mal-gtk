

anime   = executable('anime_tests',   'anime.c',
                     include_directories: libmalgtk_inc,
                     link_with: libmalgtk,
                     dependencies: libmalgtk_deps,
                     override_options: ['warning_level=1', 'werror=false'])
malitem = executable('malitem_tests', 'malitem.c',
                     include_directories: libmalgtk_inc,
                     link_with: libmalgtk,
                     dependencies: libmalgtk_deps,
                     override_options: ['warning_level=1', 'werror=false'])
manga   = executable('manga_tests',   'manga.c',
                     include_directories: libmalgtk_inc,
                     link_with: libmalgtk,
                     dependencies: libmalgtk_deps,
                     override_options: ['warning_level=1', 'werror=false'])

test('anime',   anime,   args : '--tap')
test('malitem', malitem, args : '--tap')
test('manga',   manga,   args : '--tap')
