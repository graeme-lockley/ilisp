FROM mcr.microsoft.com/vscode/devcontainers/base:debian-10

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
   && apt-get -y install --no-install-recommends entr make

# RUN apt install lsb-release wget software-properties-common \

RUN apt-get -y install lsb-release wget software-properties-common \
    && wget https://apt.llvm.org/llvm.sh \
    && chmod +x llvm.sh \
    && sudo ./llvm.sh 9 \
    && rm ./llvm.sh