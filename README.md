# multi-transcode
This program is meant to be used as a way to benchmark the transcoding capabilities of [oneVPL](https://spec.oneapi.io/onevpl/latest/index.html). It takes an input transportstream with video encoded in H.264, demuxes the stream and transcodes the video to H.262 which is then multiplexed again.

| Flag | Meaning                                                            |
|------|--------------------------------------------------------------------|
| -v   | Verbose (default off).                                             |
| -h   | Use hardware acceleration (default off).                           |
| -n   | How many parallell transcodings (1:N) that should run (default 1). |

Example usage:
~~~bash
multi-transcode -h -n 12 ./input.ts
~~~

## Build
~~~bash
mkdir build && cd build && cmake .. && make
~~~

## Roadmap
[x] Transcode example works on NUC  
[ ] Create the project skeleton  
[ ] Simple transcoding, according to requirements, works on NUC  
[ ] Two parallell transcode streams works on NUC  
[ ] 1:N pipelining implemented  
[ ] Scale as close to 30 streams as possible  

## Resources/ideas
Basically this program but implemented in an older API, [here](https://github.com/Intel-Media-SDK/MediaSDK/tree/master/samples/sample_multi_transcode), used [pipelines](https://www.intel.com/content/www/us/en/developer/articles/technical/exploring-1n-transcoding-pipelines-with-intel-media-server-studio.html) which might be of interest.

For benchmarking: https://github.com/intel/media-delivery/blob/master/doc/benchmarks/intel-iris-xe-max-graphics/intel-iris-xe-max-graphics.md

## Why
A company wanted to evaluate the performance of transcoding from H.264 to H.262. Specifically what performance boost one gets with HWA (biggest importance was on encoding).

The goal was to come as close to 30 parallell transcoded transport streams as possible. The hardware was an Intel NUC 13 Pro (the last CPU to have HWA for H.262). Quantity > quality.

## Benchmarking
The benchmarking was made between this program and ffmpeg. The company wanted to see if it was possible to get better performance by speaking directly to the oneVPL API than going through ffmpeg (which also makes use of it).

To get a general pointer to how the two compared to eachother the `time` program was used in the following way.
~~~bash
time ffmpeg -loglevel quiet -i <INPUT> -c:v mpeg2video <OUTPUT>
~~~

A deeper analysis was had with the help of a profiler.

# Hardware

## Intel NUC 13 Pro
C: CPU Raptor Lake, sista CPU:n från Intel som har stöd för HW-acceleration av MPEG2.
Intel® Iris® Xe Graphics, Xe, Raptor Lake-P
13th generation Core I7 (raptor lake)

vainfo: "iHD driver indicates support for the QSV and VA-API interfaces."
step 7: https://jellyfin.org/docs/general/administration/hardware-acceleration/intel/

## Installation
Ubuntu Desktop 23.04 (kernel 6.2.0-23-generic)

~~~bash
sudo apt update && sudo apt upgrade -y && \
suda apt install -y vainfo libvpl-dev libva-dev intel-media-va-driver-no-free
~~~

It should then be possible to run https://github.com/oneapi-src/oneVPL/tree/master/examples/api2x/hello-transcode

