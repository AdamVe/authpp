FROM ubuntu:23.04

ENV DEBIAN_FRONTEND="noninteractive" TZ="Europe/Stockholm"

RUN apt-get update && apt-get -y install \
    ccache \
    cmake \
    curl \
    g++ \
    gcc \
    git \
    libssl-dev \
    libgtkmm-4.0-dev \
    make \
    unzip \
    wget \
  && apt autoclean \
  && rm -rf /var/lib/apt/lists/*
