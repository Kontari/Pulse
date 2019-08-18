from distutils.core import setup, Extension
setup(name='monitor', version='1.1', \
    ext_modules=[Extension('monitor', ['monitor_module.c'])])