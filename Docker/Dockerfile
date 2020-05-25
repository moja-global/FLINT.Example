# ==================================================================================================================
# Multiple docker - Part 1
#
# Docker to bionic based image for FLINT.example project
#
# Building this Docker: 
#    docker build --build-arg NUM_CPU=8 -t moja/flint.example:bionic .
#
# ==================================================================================================================

FROM mojaglobal/flint:bionic as builder
LABEL maintainer="info@moja.global"

ARG NUM_CPU=1
ARG DEBIAN_FRONTEND=noninteractive

ENV ROOTDIR /usr/local

WORKDIR $ROOTDIR/src

# FLINT.example
RUN git clone --depth 1 https://github.com/moja-global/FLINT.example.git FLINT.example \
    && mkdir -p FLINT.example/Source/build && cd FLINT.example/Source/build \
    && cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
             -DENABLE_TESTS:BOOL=OFF \
             -DENABLE_MOJA_FLINT_EXAMPLE_BASE:BOOL=ON \
             -DENABLE_MOJA_FLINT_EXAMPLE_ROTHC:BOOL=ON \
             -DBUILD_SHARED_LIBS=ON  \
             -DBoost_USE_STATIC_LIBS=OFF .. \
	&& make --quiet -j $NUM_CPU \
	&& make --quiet install/strip \
	&& make clean \
    && cd $ROOTDIR/src

RUN rm -r $ROOTDIR/src/*    
RUN rm $ROOTDIR/bin/cmake \
    && rm $ROOTDIR/bin/cpack \
    && rm $ROOTDIR/bin/ctest   
 
# ==================================================================================================================
# Multiple docker - Part 2
#
# This docker will use the libraries built above to have an image with just the required parts to run the example
#
# ==================================================================================================================
FROM ubuntu:18.04

# PROJ dependencies
RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
        libsqlite3-0 \
        curl unzip

# GDAL dependencies
RUN apt-get update -y \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
        python3-numpy python3-pip libpython3.6 ca-certificates \
        libgeos-3.6.2 libgeos-c1v5 \
        libcurl4 libexpat1 \
        libxerces-c3.2 \
        libzstd1 bash libpq5 libssl1.1 \
    # Clean up
    && apt-get autoremove -y \
    && apt-get clean -y \        
    && rm -rf /var/lib/apt/lists/*

# set environment variables
ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
ENV GDAL_DATA /usr/local/share/gdal
ENV GDAL_HTTP_VERSION 2

ENV PATH $ROOTDIR/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTDIR/lib:$LD_LIBRARY_PATH
ENV PYTHONPATH $ROOTDIR/lib:$PYTHONPATH

# ==================================================================================================================
# Moja libraries and Binaries
# ==================================================================================================================

COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/local/bin/ /usr/local/bin/
COPY --from=builder /usr/local/share/ /usr/local/share/
COPY --from=builder /usr/local/include/gdal_version.h /usr/local/include 

RUN ldconfig

# make symbolic links to modules so they can be found by the moja.cli program
RUN ln -s /usr/local/lib/libmoja.flint.example.*.so /usr/local/bin
