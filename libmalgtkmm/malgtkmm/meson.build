glibmm_dep = dependency('glibmm-2.4', version : '>=2.44.0')
gmmproc_dir = glibmm_dep.get_pkgconfig_variable('gmmprocdir')
gmmproc = find_program(join_paths(gmmproc_dir, 'gmmproc'))

malgtkmm_targets = []

malgtkmm_private_dir = join_paths(meson.current_build_dir(), 'private')
python_mod = import('python3')
python = python_mod.find_python()
private_tgt = custom_target('malgtkmm_generate_priv_dir',
                            output : 'private',
                            command : [python, '-c',
                                       'import os\nos.makedirs("' + malgtkmm_private_dir + '", exist_ok=True)'])

malgtkmm_targets += custom_target('malgtkmm_date',
                                  depends: [malgtk_defs, private_tgt],
                                  input: malgtkmm_date_tmpls,
                                  output:  ['date.h', 'date.cc'],
                                  command: [gmmproc,
                                            '--defs', malgtkmm_defsdir,
                                            'date',
                                            malgtkmm_srcdir,
                                            meson.current_build_dir()])

malgtkmm_targets += custom_target('malgtkmm_malitem',
                                  depends: [malgtk_defs, private_tgt],
                                  input: malgtkmm_malitem_tmpls,
                                  output:  ['malitem.h', 'malitem.cc'],
                                  command: [gmmproc,
                                            '--defs', malgtkmm_defsdir,
                                            'malitem',
                                            malgtkmm_srcdir,
                                            meson.current_build_dir()])
                                            
malgtkmm_targets += custom_target('malgtkmm_anime',
                                  depends: [malgtk_defs, private_tgt],
                                  input: malgtkmm_anime_tmpls,
                                  output:  ['anime.h', 'anime.cc'],
                                  command: [gmmproc,
                                            '--defs', malgtkmm_defsdir,
                                            'anime',
                                            malgtkmm_srcdir,
                                            meson.current_build_dir()])

malgtkmm_targets += custom_target('malgtkmm_manga',
                                  depends: [malgtk_defs, private_tgt],
                                  input: malgtkmm_manga_tmpls,
                                  output:  ['manga.h', 'manga.cc'],
                                  command: [gmmproc,
                                            '--defs', malgtkmm_defsdir,
                                            'manga',
                                            malgtkmm_srcdir,
                                            meson.current_build_dir()])


generate_wrap_init = find_program(join_paths(gmmproc_dir, 'generate_wrap_init.pl'))
malgtkmm_targets += custom_target('wrap_init',
                                  input: malgtkmm_hdr_tmpls,
                                  output: 'wrap_init.cc',
                                  capture: true,
                                  command: [generate_wrap_init,
                                            '--parent_dir=malgtkmm',
                                            '--namespace=MALnew',
                                            '@INPUT@'])
                                            
gtkmm_dep = dependency('gtkmm-3.0', version : '>=3.0.0')
malgtkmm_deps = [glibmm_dep, gtkmm_dep, xml_dep, libmalgtk_dep]

libmalgtkmm = static_library('malgtkmm', malgtkmm_targets, 'init.cc',
                             include_directories: malgtkmm_inc,
                             dependencies: malgtkmm_deps)

libmalgtkmm_dep = declare_dependency(link_with: libmalgtkmm,
                                     include_directories: malgtkmm_inc,
                                     dependencies: malgtkmm_deps)



