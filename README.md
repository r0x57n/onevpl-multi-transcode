# multi-transcode
A simple multithreaded transcoder for benchmarking [oneVPL](https://spec.oneapi.io/onevpl/latest/index.html). It takes an h.264 elementary stream and transcodes it to h.262. It is based upon the [hello-transcode](https://github.com/oneapi-src/oneVPL/tree/master/examples/api2x/hello-transcode) example.

| Flag     | Meaning                                                            |
|----------|--------------------------------------------------------------------|
| -q       | Run quietly.                                                       |
| -s       | Use software acceleration (default off).                           |
| -n NUM   | How many parallell transcodings (1:N) that should run (default 1). |
| -h       | Print help message.                                                |

Example usage:
~~~bash
./multi-transcode -n 12 ./input.264
~~~

Expected output:  
~~~bash
$ ls
input.264         t0_out.262   t11_out.262  t2_out.262  t4_out.262  t6_out.262  t8_out.262
multi-transcode*  t10_out.262  t1_out.262   t3_out.262  t5_out.262  t7_out.262  t9_out.262
~~~

## Dependencies
Follow the official [installation guide](https://github.com/oneapi-src/oneVPL#installation-and-usage) for oneVPL. Required minimal library version is 2.2.

## Building
~~~bash
mkdir build && cd build && cmake .. && make
~~~
