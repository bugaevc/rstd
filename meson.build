project('rstd', 'cpp',
  license: 'MIT OR Apache-2.0',
  default_options: ['cpp_args=-nostdinc++', 'cpp_std=c++11'])

inc = include_directories('include')
subdir('src')
rstd = declare_dependency(link_with: rstd_lib, include_directories: inc)
subdir('test')
