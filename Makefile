LOGGER_IP ?= "10.0.0.224" # put log server IP in string
FTP_IP ?= 10.0.0.225 # put console IP here
.PHONY: all clean

all:
	cmake --toolchain=cmake/toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

send:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0
