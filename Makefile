include Config.mk

all: Search WebSearch ImageSearch VideoSearch RecursiveSearch

bond: Messages.bond
	gbc c++ Messages.bond

Search: Search.o Service.o Messages_types.o
WebSearch: WebSearch.o Service.o Messages_types.o
ImageSearch: ImageSearch.o Service.o Messages_types.o
VideoSearch: VideoSearch.o Service.o Messages_types.o
RecursiveSearch: RecursiveSearch.o Service.o Messages_types.o

clean:
	$(RM) *.o Search WebSearch ImageSearch VideoSearch RecursiveSearch

.PHONY: all bond clean
