include Config.mk

all: Search WebSearch ImageSearch VideoSearch

Search: Search.o
WebSearch: WebSearch.o
ImageSearch: ImageSearch.o
VideoSearch: VideoSearch.o

clean:
	$(RM) *.o Search WebSearch ImageSearch VideoSearch

.PHONY: all options clean
