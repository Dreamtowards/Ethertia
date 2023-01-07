sources = files([
    'nbt.cpp'
])

src = include_directories('.')

lib = library(meson.project_name(), sources, include_directories : includes, install: true, dependencies: module_deps)

nbtc = executable('nbtc', ['nbtc.cpp'], include_directories : includes, link_with : lib, install : true, dependencies : [module_deps])
