# multi-transcode
A simple multithreaded transcoder using [oneVPL](https://spec.oneapi.io/onevpl/latest/index.html). It takes an h.264 elementary stream as input and transcodes it to NUM number of h.m2v files. It is based upon the [hello-transcode](https://github.com/oneapi-src/oneVPL/tree/master/examples/api2x/hello-transcode) example.

| Flag     | Meaning                                                            |
|----------|--------------------------------------------------------------------|
| -q       | Run quietly.                                                       |
| -s       | Use software acceleration (default off).                           |
| -o       | Output folder (default "./").                                      |
| -n NUM   | How many parallell transcodings (1:N) that should run (default 1). |
| -h       | Print help message.                                                |

Example usage:
~~~bash
./multi-transcode -n 12 ./input.264
~~~

Expected output:  
~~~bash
$ ls
input.264         t0_out.m2v   t11_out.m2v  t2_out.m2v  t4_out.m2v  t6_out.m2v  t8_out.m2v
multi-transcode*  t10_out.m2v  t1_out.m2v   t3_out.m2v  t5_out.m2v  t7_out.m2v  t9_out.m2v
~~~

## Dependencies
Follow the official [installation guide](https://github.com/oneapi-src/oneVPL#installation-and-usage) for oneVPL. Required minimal library version is 2.2.

## Building
~~~bash
mkdir build && cd build && cmake .. && make
~~~
