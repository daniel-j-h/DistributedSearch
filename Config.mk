CXX = g++-4.9
CC = gcc-4.9
CXXFLAGS += -pthread -fdiagnostics-color=auto -fmax-errors=1 -std=c++14 -O2 -g -Wall -Wextra -pedantic -Wuninitialized -Wstrict-overflow=3 -Wshadow -fno-omit-frame-pointer -fno-inline
CXXFLAGS += -DBOND_COMPACT_BINARY_PROTOCOL
LDLIBS += -lstdc++ -lpthread -lnanomsg -lanl -ltbb -L/usr/local/lib/bond -Wl,-Bstatic -lbond -Wl,-Bdynamic
LDFLAGS += -Wl,-O1 -Wl,--hash-style=gnu -Wl,--sort-common -Wl,--demangle -Wl,--build-id
