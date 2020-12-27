#install_data('net.talinet.malgtk.cli.appdata.xml',
#             install_dir : datadir + '/appdata')
#install_data('net.talinet.malgtk.cli.gschema.xml',
#             install_dir : join_paths(get_option('datadir'), 'glib-2.0', 'schemas'))

service_conf = configuration_data()
service_conf.set('bindir', malgtk_bindir)
service_conf.set('malcli_name', malcli.name())
service_conf.set('malcli_appid', malcli_conf.get('malcli_appid'))

configure_file(
  input: 'net.talinet.malgtk.cli.service.in',
  output: 'net.talinet.malgtk.cli.service',
  configuration: service_conf,
  install_dir: join_paths(malgtk_datadir, 'dbus-1', 'services')
)

desktop_file = configure_file(
  input: 'net.talinet.malgtk.cli.desktop.in',
  output: 'net.talinet.malgtk.cli.desktop',
  configuration: service_conf,
  install_dir: join_paths(malgtk_datadir, 'applications')
)

desktop_validator = find_program('desktop-file-validate', required: false)
if (desktop_validator.found())
  test('validate desktop file', desktop_validator,
       args: desktop_file)
endif
