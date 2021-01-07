FROM mcr.microsoft.com/vscode/devcontainers/base:debian-10
# FROM ubuntu:latest

# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
RUN apt-get update \
   && apt-get -y install --no-install-recommends entr make gnupg vim

## RUN apt install lsb-release wget software-properties-common \

RUN apt-get -y install lsb-release wget software-properties-common \
    && wget https://apt.llvm.org/llvm.sh \
    && chmod +x llvm.sh \
    && ./llvm.sh 9 \
    && rm ./llvm.sh

## RUN apt-get -y install libreadline-dev
RUN apt-get -y install libedit-dev
