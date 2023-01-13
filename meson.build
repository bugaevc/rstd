project('rstd', 'cpp', license: 'MIT OR Apache-2.0')

inc = include_directories('include')
subdir('src')
rstd = declare_dependency(link_with: rstd_lib, include_directories: inc)
subdir('test')
