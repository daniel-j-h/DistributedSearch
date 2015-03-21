CXX = g++-4.9
CXXFLAGS += -pthread -fdiagnostics-color=auto -fmax-errors=1 -std=c++14 -O2 -g -Wall -Wextra -pedantic -Wuninitialized -Wstrict-overflow=3 -Wshadow -fno-omit-frame-pointer -fno-inline
LDLIBS += -lstdc++ -pthread -static -lnanomsg -L/usr/local/lib/bond -lbond
LDFLAGS += -Wl,-O1 -Wl,--hash-style=gnu -Wl,--sort-common -Wl,--demangle -Wl,--build-id
