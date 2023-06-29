# multi-transcode
A simple multithreaded transcoder for benchmarking [oneVPL](https://spec.oneapi.io/onevpl/latest/index.html). It takes an h.264 elementary stream and transcodes it to h.262. It is based upon the [hello-transcode](https://github.com/oneapi-src/oneVPL/tree/master/examples/api2x/hello-transcode).

| Flag     | Meaning                                                            |
|----------|--------------------------------------------------------------------|
| -s       | Use software acceleration (default off).                           |
| -n NUM   | How many parallell transcodings (1:N) that should run (default 1). |
| -h       | Print help message                                                 |

Example usage:
~~~bash
multi-transcode -n 12 ./input.ts
~~~

Expected output:  
12 files in the same directory with names t0\_out.262, t1\_out.262 ... t11\_out.262.

## Dependencies
Follow the official [installation guide](https://github.com/oneapi-src/oneVPL#installation-and-usage) for oneVPL. Required minimal library version is 2.2.

## Building
~~~bash
mkdir build && cd build && cmake .. && make
~~~
