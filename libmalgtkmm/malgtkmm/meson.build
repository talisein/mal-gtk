glibmm_dep = dependency('glibmm-2.4', version : '>=2.44.0')
gmmproc_dir = glibmm_dep.get_pkgconfig_variable('gmmprocdir')
gmmproc = find_program(join_paths(gmmproc_dir, 'gmmproc'))

malgtkmm_targets = []

private_dir = join_paths(meson.build_root(), 'libmalgtkmm', 'malgtkmm', 'private')
python = find_program('python3')
private_tgt = custom_target('glibmm_generate_priv_dir',
                            output : 'private',
                            command : [python, '-c',
                                       'import os\nos.makedirs("' + private_dir + '", exist_ok=True)'])

malgtkmm_targets += custom_target('date',
                                  depends: [malgtk_defs, private_tgt],
                                  input: [join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'date.hg'), join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'date.ccg')],
                                  output:  ['date.h', 'date.cc'],
                                  command: [gmmproc,
                                            '--defs', join_paths(meson.build_root(), 'libmalgtkmm', 'src'),
                                            'date',
                                            join_paths(meson.current_source_dir(), '..', 'src'),
                                            meson.current_build_dir()])

malgtkmm_targets += custom_target('malitem',
                                  depends: [malgtk_defs, private_tgt],
                                  input: [join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'malitem.hg'), join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'malitem.ccg')],
                                  output:  ['malitem.h', 'malitem.cc'],
                                  command: [gmmproc,
                                            '--defs', join_paths(meson.build_root(), 'libmalgtkmm', 'src'),
                                            'malitem',
                                            join_paths(meson.current_source_dir(), '..', 'src'),
                                            meson.current_build_dir()])
                                            
malgtkmm_targets += custom_target('anime',
                                  depends: [malgtk_defs, private_tgt],
                                  input: [join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'anime.hg'), join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'anime.ccg')],
                                  output:  ['anime.h', 'anime.cc'],
                                  command: [gmmproc,
                                            '--defs', join_paths(meson.build_root(), 'libmalgtkmm', 'src'),
                                            'anime',
                                            join_paths(meson.current_source_dir(), '..', 'src'),
                                            meson.current_build_dir()])

malgtkmm_targets += custom_target('manga',
                                  depends: [malgtk_defs, private_tgt],
                                  input: [join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'manga.hg'), join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'manga.ccg')],
                                  output:  ['manga.h', 'manga.cc'],
                                  command: [gmmproc,
                                            '--defs', join_paths(meson.build_root(), 'libmalgtkmm', 'src'),
                                            'manga',
                                            join_paths(meson.current_source_dir(), '..', 'src'),
                                            meson.current_build_dir()])


gmmproc = find_program(join_paths(gmmproc_dir, 'gmmproc'))
generate_wrap_init = find_program(join_paths(gmmproc_dir, 'generate_wrap_init.pl'))
malgtkmm_targets += custom_target('wrap_init',
                                  depends: [malgtk_defs, private_tgt],
                                  input: [join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'anime.hg'), join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'manga.hg'),join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'malitem.hg'),join_paths(meson.source_root(), 'libmalgtkmm', 'src', 'date.hg')],
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



