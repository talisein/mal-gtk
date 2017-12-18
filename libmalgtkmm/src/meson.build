
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

cat = find_program('cat')

malgtk_defs = custom_target('malgtk.defs',
                            input: [method_defs, enum_defs, signal_defs],
                            output: 'malgtk.defs',
                            command: [cat, '@INPUT@'],
                            capture: true)


