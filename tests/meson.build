
main_lib = static_library('main', 'main.cpp')

version_exe = executable('version', 'version.cpp', link_with: main_lib,
  dependencies: kvc_dep,
)
test('version', version_exe)

config_exe = executable('config', 'config.cpp', link_with: main_lib,
  dependencies: kvc_dep
)
test('config', config_exe, workdir: meson.current_source_dir())

arrays_exe = executable('arrays', 'arrays.cpp', link_with: main_lib,
  dependencies: kvc_dep
)
test('arrays', arrays_exe, workdir: meson.current_source_dir())

files_exe = executable('files', 'files.cpp', link_with: main_lib,
  dependencies: kvc_dep
)
test('files', files_exe, workdir: meson.current_source_dir())

