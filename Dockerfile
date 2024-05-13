# Docker file for the image used to build and test `oberon-lang`

FROM ubuntu:latest
LABEL version="1.0"

WORKDIR /root

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
                make cmake clang libzstd-dev libcurl4-openssl-dev libedit-dev zlib1g-dev \
                llvm-dev libboost-filesystem-dev libboost-program-options-dev \
                python3-pip && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
RUN pip3 install lit filecheck sphinx --break-system-packages

ENV CC=clang
ENV CXX=clang++

WORKDIR /
