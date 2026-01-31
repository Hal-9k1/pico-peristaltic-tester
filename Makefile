.PHONY: all client clean distclean pico

all: client pico

client:
	$(MAKE) -C client client

pico:
	mkdir -p pico-build
	cmake -DPICO_SDK_FETCH_FROM_GIT=ON -S pico -B pico-build
	cmake --build -B pico-build

clean:
	$(MAKE) -C client clean
	cmake --build -B pico-build --target clean

distclean:
	$(MAKE) -C client clean
	rm -rf pico-build
