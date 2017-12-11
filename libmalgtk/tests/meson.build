anime   = executable('anime_tests',   'anime.c',   include_directories: libmalgtk_inc, link_with: libmalgtk, dependencies: deps)
malitem = executable('malitem_tests', 'malitem.c', include_directories: libmalgtk_inc, link_with: libmalgtk, dependencies: deps)
manga   = executable('manga_tests',   'manga.c',   include_directories: libmalgtk_inc, link_with: libmalgtk, dependencies: deps)

test('anime',   anime,   args : '--tap')
test('malitem', malitem, args : '--tap')
test('manga',   manga,   args : '--tap')
