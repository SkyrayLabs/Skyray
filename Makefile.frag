$(srcdir)/deps/libuv/out/Debug/libuv.a:
	cd $(srcdir)/deps/libuv && ./gyp_uv.py -f make && make -C out CFLAGS=-fPIC

all: $(srcdir)/deps/libuv/out/Debug/libuv.a $(all_targets)
	@echo
	@echo "Build complete."
	@echo "Don't forget to run 'make test'."
	@echo
