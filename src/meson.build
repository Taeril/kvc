
ver_arr = meson.project_version().split('.')
ver_data = configuration_data({
  'VERSION': meson.project_version(),
  'VERSION_MAJOR': ver_arr[0],
  'VERSION_MINOR': ver_arr[1],
  'VERSION_PATCH': ver_arr[2],
})
configure_file(
  input : 'version.hpp.in',
  output : 'version.hpp',
  configuration : ver_data
)


sources = files([
  'utils.cpp',
  'kvc.cpp',
])

kvc_lib = static_library('kvc', sources,
  install : true,
  gnu_symbol_visibility : 'hidden',
  dependencies: fmt_dep,
  include_directories: '.',
)
install_headers('kvc.hpp')

kvc_dep = declare_dependency(
  link_with: kvc_lib,
  include_directories: '.',
)


pkg = import('pkgconfig')
pkg.generate(
  name : meson.project_name(),
  filebase : meson.project_name(),
  description : 'Simple configuration parser library',
  subdirs : '.',
  libraries : kvc_lib,
  version : meson.project_version(),
)

