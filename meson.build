project('cache_sim', 'cpp',
    version : '0.1',
    license : [ 'MIT' ],
    meson_version : '>= 1.4.0',
    default_options : [ 'warning_level=3', 'buildtype=debugoptimized', 'cpp_std=c++20' ]
)

# Variables tracking sources and libraries
library_path = []
project_sources = [
    # main
    'src/main.cpp',

    # utils
    'src/utils.cpp',

    # cache configuration
    'src/config.cpp',

    # memqueue
    'src/memqueue/memqueue.cpp',
]

project_headers = [
    # memqueue
    'include/memqueue/memqueue.hpp',

    # utils
    'include/utils.hpp',

    # cache configuration headers
    'include/config.hpp',
    'include/cache_stats.hpp',

    # Cache structure files
    'include/structures/structures.hpp',
    'include/structures/replacement.hpp',
]

headers = include_directories('include')

build_args = [
    '-DPROJECT_NAME=' + meson.project_name(),
    '-DPROJECT_VERSION=' + meson.project_version(),
    '-Wall',
    '-Wextra,'
]

project_target = executable(
    meson.project_name(),
    project_sources,
    include_directories : headers,
    c_args : build_args,
)
