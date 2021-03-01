project('miniaudio', 'c', version: '0.10.32')

compiler = meson.get_compiler('c')
c_flags = [
    '-D_GNU_SOURCE',
]
link_deps = []
os_deps = []
if target_machine.system() == 'darwin'
    os_deps += [
        dependency('AudioToolbox'),
        dependency('CoreAudio'),
        dependency('CoreFoundation'),
    ]
endif

vorbis_decoder = get_option('vorbis')
if vorbis_decoder == 'libvorbis'
    c_flags += ['-DUSE_OGGVORBIS']
    link_deps = [
        dependency('ogg'),
        dependency('vorbis'),
    ]
elif vorbis_decoder == 'stb'
    c_flags += ['-DUSE_STB_VORBIS']
endif

incdirs = ['.', 'research']

sources = [
	'miniaudio_engine.c',
]

incdirs = include_directories(incdirs)

lib = static_library(
    'miniaudio',
    sources,
    c_args: c_flags,
    include_directories: incdirs,
    dependencies: link_deps + os_deps,
)

miniaudio_dep = declare_dependency(
    include_directories: incdirs,
    dependencies: link_deps + os_deps,
    link_with: lib,
    version: meson.project_version()
)

meson.override_dependency('miniaudio', miniaudio_dep)

# vim: set ts=4 sts=4 sw=4 et:
