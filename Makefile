.PHONY: all client clean distclean pico pico-sdk

all: client pico

client:
	$(MAKE) -C client client

pico: pico-sdk/.makestamp
	mkdir -p pico-build
	cmake -DPICO_SDK_PATH=$(realpath ./pico-sdk) -S pico -B pico-build
	cmake --build -B pico-build

pico-sdk/.makestamp:
	git clone https://github.com/raspberrypi/pico-sdk.git --depth=1 pico-sdk
	git -C pico-sdk submodule update --init --depth=1 --progress --recursive
	touch pico-sdk/.makestamp

clean:
	$(MAKE) -C client clean
	cmake --build -B pico-build --target clean

distclean:
	$(MAKE) -C client clean
	rm -rf pico-build
	rm -rf pico-sdk
