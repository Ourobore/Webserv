FROM debian:bullseye

# Setting up Docker https://docs.docker.com/engine/install/debian/
RUN apt-get update && apt-get -y install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    lsb-release \
    && curl -fsSL https://download.docker.com/linux/debian/gpg | \
    gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg \
    && echo \
    "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/debian \
    $(lsb_release -cs) stable" | \
    tee /etc/apt/sources.list.d/docker.list > /dev/null


# Setting up working environment
RUN apt-get update && apt-get -y install \
    # C++ dependencies
    clang \
    clangd \
    clang-format \
    make \
    valgrind \
    # Docker dependencies
    docker-ce\
    docker-ce-cli \
    containerd.io \
    # Utilities
    curl \
    git \
    netcat-openbsd \
    telnet \
    siege \
    # Install zsh and oh-my-zsh and make zsh the default shell
    zsh ;\
    cd / && sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" \
    && chsh -s $(command -v zsh)



# C++ dependencies:
#   clang
#   clang-format
#   make
#   valgrind

# Utilities:
#   git
#   docker
#   zsh
#   oh-my-zsh

# Webserv testing tools
#   curl
#   netcat
#   telnet
