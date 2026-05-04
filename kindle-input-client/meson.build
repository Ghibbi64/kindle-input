project(
  'template-kindle-gtk',
  'cpp',
  version: 'v0.0.1',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=1.1'
)

gtk_dep = dependency('gtk+-2.0')
thread_dep = dependency('threads')

# Recursively collect all .cpp files under src/
sources = run_command(
  'find', 'src', '-name', '*.cpp', '-type', 'f',
  check: true
).stdout().strip().split('\n')

include_dirs = include_directories(
  './include/',
)

xxd_exe = find_program('xxd')
xxd_gen = generator(
  xxd_exe,
  output: '@PLAINNAME@.h',
  arguments: ['-i', '-C', '-n', '@PLAINNAME@', '@INPUT@', '@OUTPUT@']
)
gen_sources = xxd_gen.process(['src/main.ui.xml'])

executable(
  'template_kindle_gtk',
  sources + gen_sources,
  include_directories: include_dirs,
  dependencies: [
    gtk_dep,
    thread_dep,
  ],
  cpp_args: '-static-libstdc++',
  link_args: ['-static-libstdc++']
)
