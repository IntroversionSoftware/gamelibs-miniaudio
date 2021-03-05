project('miniaudio', 'c', version: '0.10.32')

compiler = meson.get_compiler('c')
link_deps = [
    dependency('ogg'),
    dependency('vorbis'),
]

incdirs = ['.', 'research']

sources = [
	'miniaudio_engine.c',
]

incdirs = include_directories(incdirs)

lib = static_library(
    'miniaudio',
    sources,
    include_directories: incdirs,
    dependencies: link_deps,
)

miniaudio_dep = declare_dependency(
    include_directories: incdirs,
    dependencies: link_deps,
    link_with: lib,
    version: meson.project_version()
)

meson.override_dependency('miniaudio', miniaudio_dep)

# vim: set ts=4 sts=4 sw=4 et:
