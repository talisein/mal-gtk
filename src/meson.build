gobj_dep   = dependency('gobject-2.0', version : '>=2.44.0')
glib_dep   = dependency('glib-2.0',    version : '>=2.44.0')
glibmm_dep = dependency('glibmm-2.4',  version : '>=2.44.0')
# gtk_show_uri_on_window in 3.22
gtkmm_dep  = dependency('gtkmm-3.0',   version : '>=3.22.0')
xml_dep    = dependency('libxml-2.0',  version : '>=2.7.8')
# 7.33 for CURLOPT_XOAUTH2_BEARER
curl_dep   = dependency('libcurl',     version : '>=7.33.0')
secret_dep = dependency('libsecret-1', version : '>=0.12')
sigcpp_dep = dependency('sigc++-2.0',  version : '>=2.2.11')
gcr_dep    = dependency('gcr-base-3')
thread_dep = dependency('threads')
xmlpp_dep  = dependency('libxml++-3.0')

malgtk_deps = [gobj_dep, glib_dep, glibmm_dep, gtkmm_dep, xml_dep, curl_dep, secret_dep, thread_dep, gcr_dep, xmlpp_dep]

malgtk_src = files(['main.cpp',
                    'application.cpp',
                    'user_info.cpp',
                    'mal.cpp',
                    'malitem.cpp',
                    'anime.cpp',
                    'manga.cpp',
                    'xml_reader.cpp',
                    'xml_writer.cpp',
                    'anime_serializer.cpp',
                    'manga_serializer.cpp',
                    'text_util.cpp',
                    'mal_curl.cpp',
                    'logging.cpp',
                    'mal_api_unstable.cpp',
                    'gui/malgtk_cellrenderer_score.c',
                    'gui/cellrendererscore.cpp',
                    'gui/main_window.cpp',
                    'gui/password_dialog.cpp',
                    'gui/malitem_list_view.cpp',
                    'gui/anime_list_view.cpp',
                    'gui/manga_list_view.cpp',
                    'gui/increment_entry.cpp',
                    'gui/fancy_label.cpp',
                    'gui/date_widgets.cpp'])

malgtk = executable('mal-gtk', malgtk_src,
                    include_directories : libmalgtk_inc,
                    dependencies : malgtk_deps,
                    install      : true,
                    gui_app      : true,
                    c_args       : ['-DG_LOG_DOMAIN="mal-gtk"', '-DG_LOG_USE_STRUCTURED'],
                    cpp_args     : ['-DG_LOG_DOMAIN="mal-gtk"', '-DG_LOG_USE_STRUCTURED']
                    )

malcli_conf = configuration_data()
malcli_conf.set('malcli_appid', 'net.talinet.malgtk.cli')
malcli_conf.set('malcli_clientid', get_option('mal_client_id'))

configure_file(
  input: 'mal-cli_config.h.in',
  output: 'mal-cli_config.h',
  configuration: malcli_conf,
)

malcli_src = ['mal-cli_main.cpp'
             ]

malcli = executable('mal-cli', malcli_src,
                    include_directories: include_directories('.'),
                    dependencies: [libmalgtkmm_dep, gtkmm_dep],
                    c_args : ['-DG_LOG_DOMAIN="mal-cli"', '-DG_LOG_USE_STRUCTURED'],
                    cpp_args : ['-DG_LOG_DOMAIN="mal-cli"', '-DG_LOG_USE_STRUCTURED'],
                    install: true)
