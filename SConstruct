import os
import sys

#env = DefaultEnvironment(ENV=os.environ, TARGET_ARCH="x86_64")
env = DefaultEnvironment(ENV=os.environ)

env.Append(CPPPATH='#include/')
env.Append(LIBPATH='#lib/')

debug = 0;

platform = env['PLATFORM']

if platform.startswith('win'):
    Help("\nType: could use 'scons debug=1' to build the windows debug version.\n")

    TargetArch = env._dict['TARGET_ARCH']
    if not TargetArch:
        TargetArch = "x86"
    else:
        TargetArch = TargetArch.lower()

    debug = ARGUMENTS.get('debug', 0)
    if int(debug):
        env.Append(CCFLAGS = ['-DEBUG', '-EHsc', '-MTd', '-DCURL_STATICLIB'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-MT', '-DCURL_STATICLIB'])
    env.Append(LIBS = ["kernel32", "DbgHelp"])

    env.Install(Dir('#').abspath + "/include/curl-win", [Glob('src/curl-win/*.h')])

    if TargetArch == "amd64" or TargetArch == "x86_64":
        env.Append(LIBPATH=['#lib/windows/AMD64'])
        env.Install(Dir('#').abspath + "/include/openssl", [Glob('src/openssl-win64/openssl/*.h')])
    elif TargetArch == "x86":
        env.Append(LIBPATH=['#lib/windows/I386'])
        env.Install(Dir('#').abspath + "/include/openssl", [Glob('src/openssl-win86/openssl/*.h')])
    else:
        # no ia64
        raise Exception("Unknown Architecture: " + str(machine))
else:
    env.Append(CCFLAGS = ['-O2', '-fno-strict-aliasing', '-g'])

if platform.startswith('darwin'):
    env.Append(CCFLAGS = ['-I/usr/local/opt/openssl/include'])

Export('env')
Export('platform')
Export('debug')

env.SConscript(dirs=Flatten('src'))
