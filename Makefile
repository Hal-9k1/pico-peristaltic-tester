.PHONY: all client clean

all: client

client:
	$(MAKE) -C client client

clean:
	$(MAKE) -C client clean
