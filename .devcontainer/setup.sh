#!/bin/bash
# setup.sh — Install Phoenix-RTOS build dependencies di GitHub Codespaces
# Dijalankan otomatis saat postCreateCommand

set -e

echo "🔧 Installing Phoenix-RTOS build dependencies..."

sudo apt-get update -q

sudo apt-get install -y \
    make \
    gcc \
    g++ \
    gcc-multilib \
    g++-multilib \
    libc6-dev-i386 \
    lib32gcc-11-dev \
    genext2fs \
    e2fsprogs \
    qemu-system-x86 \
    scons \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libssl-dev \
    libusb-1.0-0-dev \
    bzip2 \
    xz-utils \
    python3 \
    python3-pip \
    wget \
    bc \
    rsync

echo "📦 Initializing git submodules..."
git submodule update --init --recursive

echo ""
echo "✅ Semua dependency sudah terinstall!"
echo "👉 Untuk build, jalankan:"
echo "   TARGET=ia32-generic-qemu source phoenix-rtos-build/build.sh all"
