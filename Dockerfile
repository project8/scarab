ARG img_repo=python
ARG img_tag=3.12.1-slim-bookworm

# This FROM line includes a label so that the dependencies can be built by themselves by using the `--target` argument of `docker build`
FROM ${img_repo}:${img_tag} AS base

ARG build_type=Release
ARG build_python=FALSE
ARG enable_testing=FALSE
ARG narg=2

# Most dependencies

RUN apt-get update && \
    apt-get clean && \
    apt-get --fix-missing  -y install \
        build-essential \
        cmake \
#        gdb \
        git \
        libboost-filesystem-dev \
        libboost-system-dev \
        libyaml-cpp-dev \
        rapidjson-dev && \
#        pybind11-dev \
#        wget && \
    rm -rf /var/lib/apt/lists/*

# use pybind11_checkout to specify a tag or branch name to checkout
ARG pybind11_checkout=master
ARG pybind11_repo=https://github.com/pybind/pybind11.git
ARG pybind11_name=pybind11
RUN cd /usr/local && \
    git clone ${pybind11_repo} && \
    cd ${pybind11_name} && \
    git checkout ${pybind11_checkout} && \
    mkdir build && \
    cd build && \
    cmake -DPYBIND11_TEST=FALSE .. && \
    make -j${narg} install && \
    cd / && \
    rm -rf /usr/local/${pybind11_name}

# use quill_checkout to specify a tag or branch name to checkout
ARG quill_checkout=v9.0.2
RUN cd /usr/local && \
    git clone https://github.com/odygrd/quill.git && \
    cd quill && \
    git checkout ${quill_checkout} && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j${narg} install && \
    cd / && \
    rm -rf /usr/local/quill


FROM base

# note that the build dir is *not* in source, this is so that the source can me mounted onto the container without covering the build target

COPY .git /usr/local/src/.git
COPY documentation /usr/local/src/documentation
COPY cmake /usr/local/src/cmake
COPY executables /usr/local/src/executables
COPY library /usr/local/src/library
COPY python /usr/local/src/python
COPY testing /usr/local/src/testing
COPY CMakeLists.txt /usr/local/src/CMakeLists.txt
COPY ScarabConfig.cmake.in /usr/local/src/ScarabConfig.cmake.in
COPY VERSION /usr/local/src/VERSION

RUN mkdir -p /usr/local/build && \
    cd /usr/local/build && \
    cmake ../src && \
    # unclear why I have to run cmake twice
    cmake -DCMAKE_BUILD_TYPE=${build_type} \
        -DCMAKE_INSTALL_PREFIX:PATH=/usr/local \ 
        -DScarab_ENABLE_TESTING:BOOL=${enable_testing} \
        -DScarab_BUILD_PYTHON:BOOL=${build_python} \
        -DPBUILDER_PY_INSTALL_IN_SITELIB=TRUE \
        ../src && \
    make -j${narg} install

# TODO: rm -rf /usr/local/src
