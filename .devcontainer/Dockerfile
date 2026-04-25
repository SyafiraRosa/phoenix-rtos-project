FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install all build dependencies for Phoenix-RTOS
RUN apt-get update && apt-get install -y \
    # Basic tools
    git \
    make \
    wget \
    curl \
    xz-utils \
    bzip2 \
    unzip \
    zip \
    bc \
    rsync \
    python3 \
    python3-pip \
    # Cross-compilation for ia32 (x86 32-bit)
    gcc \
    g++ \
    gcc-multilib \
    g++-multilib \
    libc6-dev-i386 \
    lib32gcc-11-dev \
    # QEMU for emulation
    qemu-system-x86 \
    # Filesystem tools
    genext2fs \
    e2fsprogs \
    # Linker & binary tools
    binutils \
    # Additional utilities
    scons \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libssl-dev \
    libusb-1.0-0-dev \
    # Codespaces compatibility
    sudo \
    locales \
    ca-certificates \
    && locale-gen en_US.UTF-8 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set locale
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Set default target
ENV TARGET=ia32-generic-qemu

# Create non-root user (required by Codespaces)
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME
WORKDIR /workspaces/phoenix-rtos-project
