include Config.mk

all: Search WebSearch ImageSearch VideoSearch

Messages: Messages.bond
	gbc c++ Messages.bond

Search: Search.o Messages_types.o
WebSearch: WebSearch.o Messages_types.o
ImageSearch: ImageSearch.o Messages_types.o
VideoSearch: VideoSearch.o Messages_types.o

clean:
	$(RM) *.o Search WebSearch ImageSearch VideoSearch

.PHONY: all options clean
