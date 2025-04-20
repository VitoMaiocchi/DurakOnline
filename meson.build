project('DurakOline', 'cpp', default_options : ['cpp_std=c++20'])

boost_dep = dependency('boost', modules : ['system'])
pthread_dep = dependency('threads')
rapidjson_dep = dependency('RapidJSON')

inc = include_directories('.')

src_files = [
  'Server/manager.cpp',
  'Server/network.cpp',
]

executable('server', src_files, dependencies : [boost_dep, pthread_dep])

executable('client_test', 'Test/client.cpp', dependencies : [boost_dep, pthread_dep])
executable('protocol_test', 'Test/protocol.cpp',
  include_directories : [inc], 
  dependencies : [rapidjson_dep]
)