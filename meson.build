project(
  'potentials-schema',
  version: '1.16.0',
  meson_version: '>=0.60.0',
  license: 'MIT',
)

# Consumers: subproject('potentials-schema').get_variable('potentials_capnp_file')
potentials_capnp_file = files('Potentials.capnp')
potentials_capnp_dir = meson.current_source_dir()

if not meson.is_subproject()
  add_languages('cpp', native: false, required: true)
  capnp_prog = find_program('capnp', required: true)
  capnpc_cpp_prog = find_program('capnpc-c++', required: true)
  capnp_rpc_dep = dependency('capnp-rpc', required: true)
  capnpc_dep = dependency('capnpc', required: true)

  potentials_capnp_cpp = custom_target(
    'potentials-capnp-cpp',
    input: potentials_capnp_file,
    output: ['Potentials.capnp.h', 'Potentials.capnp.c++'],
    command: [
      capnp_prog,
      'compile',
      '-o' + capnpc_cpp_prog.full_path() + ':' + '@OUTDIR@',
      '--src-prefix=' + meson.current_source_dir(),
      '@INPUT@',
    ],
  )

  has_charge_test = executable(
    'has_charge_test',
    ['tests/has_charge_test.cpp', potentials_capnp_cpp],
    dependencies: [capnp_rpc_dep, capnpc_dep],
    cpp_args: [
      '-std=c++17',
      '-DPOTENTIALS_SCHEMA_PATH="' + (meson.current_source_dir() / 'Potentials.capnp') + '"',
    ],
  )

  test('has_charge', has_charge_test)
endif
