FROM mojaglobal/vscode-baseimage:bionic

ENV DEBIAN_FRONTEND=noninteractive

ARG NUM_CPU=6
#ARG FLINT_BRANCH=feature/gdal_meta_data_optional
ARG FLINT_BRANCH=develop

ENV ROOTDIR /usr/local
ENV PATH $ROOTDIR/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTDIR/lib:$ROOTDIR/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
ENV PYTHONPATH $ROOTDIR/lib:$PYTHONPATH
ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
ENV GDAL_DATA=/usr/local/share/gdal
ENV GDAL_HTTP_VERSION 2
ENV GDAL_HTTP_MERGE_CONSECUTIVE_RANGES YES
ENV GDAL_HTTP_MULTIPLEX YES

WORKDIR $ROOTDIR/src

# GET  moja.global
RUN git clone --recursive --depth 1 -b ${FLINT_BRANCH} https://github.com/moja-global/FLINT.git flint \
    && mkdir -p flint/Source/build && cd flint/Source/build \
    && cmake -DCMAKE_BUILD_TYPE=DEBUG  \
            -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
            -DENABLE_TESTS:BOOL=OFF \
            -DENABLE_MOJA.MODULES.GDAL=ON \
            -DENABLE_MOJA.MODULES.LIBPQ=ON \
            -DBoost_USE_STATIC_LIBS=OFF \
            -DBUILD_SHARED_LIBS=ON .. \
	&& make --quiet -j $NUM_CPU \
	&& make --quiet install \
#	&& make clean \
    && cd $ROOTDIR/src

RUN ln -s $ROOTDIR/lib/libmoja.modules.* $ROOTDIR/bin

# ln -s /usr/local/lib/libmoja.flint.example.*.so /usr/local/bin
# RUN ln -s $ROOTDIR/lib/libmoja.flint.example.*.so $ROOTDIR/bin

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN ldconfig

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
