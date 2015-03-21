# Communication And Serialization

Communication patterns (nanomsg) and serialization (bond).

More precisely this project mimics Rob Pike's search example introducing Go's channels:

* Query different services (Web Search, Image Search, Video Search, ...)
* Gather and merge results

Instead of using Go and its channels, this project makes use of nanomsg for communication and bond for serialization.

See:

* https://talks.golang.org/2012/concurrency.slide
* https://www.youtube.com/watch?v=f6kdp27TYZs


## Usage

    aptitude install TODO
    make


## License

Copyright © 2015 Daniel J. Hofmann

Distributed under the MIT License (MIT).
