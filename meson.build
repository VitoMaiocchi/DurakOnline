project('websocketpp_example', 'cpp')

boost_dep = dependency('boost', modules : ['system'])
pthread_dep = dependency('threads')

executable('server', 'server.cpp', dependencies : [boost_dep, pthread_dep])
executable('client', 'client.cpp', dependencies : [boost_dep, pthread_dep])
