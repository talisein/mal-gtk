
method_defs = custom_target('methods.defs',
                            input: libmalgtk_hdrs,
                            output: 'malgtk_methods.defs',
                            command: [h2def, '@INPUT@'],
                            capture: true)


enum_defs =  custom_target('enums.defs',
                            input: [libmalgtk_hdrs, libmalgtk_enums[1]],
                            output: 'malgtk_enums.defs',
                            command: [enums, '@INPUT@'],
                            capture: true)

signal_defs = custom_target('signals.defs',
                          output: 'malgtk_signals.defs',
                          command: [extra_defs_gen],
                          capture: true,
                          depends: libmalgtk)


malgtkmm_malitem_tmpls = files('malitem.ccg', 'malitem.hg')
malgtkmm_anime_tmpls = files('anime.ccg', 'anime.hg')
malgtkmm_manga_tmpls = files('manga.ccg', 'manga.hg')
malgtkmm_date_tmpls = files('date.ccg', 'date.hg')

malgtkmm_hdr_tmpls = files('date.hg','malitem.hg','anime.hg','manga.hg')

malgtkmm_defsdir = meson.current_build_dir()
malgtkmm_srcdir = meson.current_source_dir()

cat = find_program('cat')
malgtk_defs = custom_target('malgtk.defs',
                            input: [method_defs, enum_defs, signal_defs],
                            output: 'malgtk.defs',
                            command: [cat, '@INPUT@'],
                            capture: true)




