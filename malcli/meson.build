malcli_conf = configuration_data()
malcli_conf.set('malcli_appid', 'net.talinet.malgtk.cli')
malcli_conf.set('malcli_clientid', get_option('mal_client_id'))

configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: malcli_conf,
)

malcli_src = [
  'main.cpp'
]
malcli_deps = [libmalgtkmm_dep, gtkmm_dep, libmalbb_dep]

malcli = executable('mal-cli', malcli_src,
                    include_directories: include_directories('.'),
                    dependencies: malcli_deps,
                    c_args : ['-DG_LOG_DOMAIN="mal-cli"', '-DG_LOG_USE_STRUCTURED'],
                    cpp_args : ['-DG_LOG_DOMAIN="mal-cli"', '-DG_LOG_USE_STRUCTURED'],
                    install: true)
