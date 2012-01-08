Import ('env')

name = 'biopp-filer'
inc = env.Dir('.')
ext_inc = env.Dir ('biopp-filer')
src = []
deps = []
CXXFLAGS = []

env.Append (CXXFLAGS=CXXFLAGS, CFLAGS=CXXFLAGS)
env.CreateHeaderOnlyLibrary(name, inc, ext_inc, deps)
