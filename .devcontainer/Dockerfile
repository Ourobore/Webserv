FROM debian:buster

# Setting up for Docker https://docs.docker.com/engine/install/debian/
#RUN apt-get update && apt-get -y install \
#    apt-transport-https \
#    ca-certificates \
#    curl \
#    gnupg \
#    lsb-release \
#    && curl -fsSL https://download.docker.com/linux/debian/gpg | \
#    gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg \
#    && echo \
#    "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
#    $(lsb_release -cs) stable" | \
#    tee /etc/apt/sources.list.d/docker.list > /dev/null


# Setting up working environment
RUN apt-get update && apt-get -y install \
    clang \
    clang-format \
#    docker-ce\
#    docker-ce-cli \
#    containerd.io \
    curl \
    git \
    make \
    netcat \
    telnet

# C++ dependencies:
#   clang
#   clang-format

# Utilities:
#   git
#   docker
#   make

# Webserv testing tools
#   curl
#   netcat
#   telent

#    docker-ce\
#    docker-ce-cli \
#    containerd.io \