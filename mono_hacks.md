1. For some reason, libmono-2.0.so is not loaded when libmono-native is called via pinvoke, so we have to load it manually using:
`dlopen("/home/michael/projects/cmake_test/libraries/mono/lib/libmono-2.0.so", RTLD_GLOBAL);`
2. When mono raises an unhandled exception and libmono-native has failed to load, then the app will segfault because it 
has no way to write to the console. The workaround for this is to set `MONO_THREADS_SUSPEND=preemptive` before starting
so that it can dump correctly.
3. use `MONO_LOG_LEVEL=info` or `MONO_LOG_LEVEL=debug` to debug mono errors.
