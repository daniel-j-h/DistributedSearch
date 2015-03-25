# Distributed Search Engine in C++14

Technical walkthrough: https://daniel-j-h.github.io/post/distributed-search-nanomsg-bond/

This project mimics Rob Pike's search example introducing Go's channels:

* Query different services (Web Search, Image Search, Video Search, ...)
* Gather and merge results
* Timeouts, replication, reconnection, and more!

Instead of using Go and its channels, this project makes use of nanomsg for communication and bond for serialization.

See:

* https://talks.golang.org/2012/concurrency.slide#42 (slides 42-52)
* https://www.youtube.com/watch?v=f6kdp27TYZs&t=1721 (starting from 28:40)

If you're interested in nanomsg and bond (or similar technologies, such as ZeroMQ and Cap'n Proto) this is probably the perfect project for you to get started :)


## Requirements:

* nanomsg for communication: https://github.com/nanomsg/nanomsg
* bond for serialization: https://github.com/Microsoft/bond

Note: RapidJSON is required as of now, see: https://github.com/Microsoft/bond/issues/44
As a quickfix build it from bond's thirdparty directory then copy the rapidjson/include/rapidjson directory to /usr/local/include/.

Message stubs were generated with bond's gbc using:

    gbc c++ Messages.bond

See also: https://microsoft.github.io/bond/manual/bond_cpp.html


## Usage:

To start the user facing Search service, you can issue queries against:

    ./Search

Now the Search service will show you no results yet, since there is no service for answering queries. Let's start some:

    ./WebSearch

Now you'll get WebSearch results. There is also a ImageSearch and a VideoSearch service. You can start and stop them at any time. Replication is also possible, but note that the results are unique. For example when two WebSearch services are started, the Search service receives the web results from both of them, but only shows them once.

By default the services communicate over TCP port 9995. You can change this if you want:

    ./Search "tcp://*:9555"
    ./WebSearch "tcp://localhost:9555"


Or using IPC:

    ./Search "ipc:///tmp/search.ipc"
    ./WebSearch "ipc:///tmp/search.ipc"


There is a RecursiveSearch service available, showing an example of how to build a service tree:

    ./Search "tcp://*:9995"
    ./RecursiveSearch "tcp://*:9996" "tcp://localhost:9995"
    ./WebSearch "tcp://localhost:9996"

With this setup, ./Search is the tree's root, with ./RecursiveSearch attached to it and ./WebSearch attached to the ./RecursiveSearch leaf. Attaching more services is possible on each layer of the tree. Just attach them to the subtree's specific root-service.

Note: if you try the recursive example on a single machine, you have to change the port for each layer, otherwise there would be no way to distinguish the root from internal tree nodes.


## License

Copyright © 2015 Daniel J. Hofmann

Distributed under the MIT License (MIT).
