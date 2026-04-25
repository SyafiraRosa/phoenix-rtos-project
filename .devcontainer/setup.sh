#!/bin/bash
# setup.sh — Persiapkan environment Phoenix-RTOS di GitHub Codespaces
# Dijalankan otomatis saat postCreateCommand

set -e

echo "🔧 Installing build tools..."

sudo apt-get update -q

# Tools dasar dan QEMU (mode serial/nographic untuk Codespaces)
sudo apt-get install -y \
    make \
    wget \
    curl \
    xz-utils \
    bzip2 \
    unzip \
    python3 \
    python3-pip \
    qemu-system-x86 \
    genext2fs \
    e2fsprogs \
    bc \
    rsync \
    scons

echo "📦 Initializing git submodules..."
git submodule update --init --recursive

echo ""
echo "✅ Setup selesai!"
echo ""
echo "══════════════════════════════════════════"
echo "  CARA BUILD Phoenix-RTOS di Codespaces  "
echo "══════════════════════════════════════════"
echo ""
echo "  Build menggunakan Docker (WAJIB):"
echo "  TARGET=ia32-generic-qemu ./docker-build.sh all"
echo ""
echo "  Jalankan QEMU (serial mode):"
echo "  TARGET=ia32-generic-qemu CONSOLE=serial ./docker-build.sh qemu"
echo ""
echo "  Atau cukup:"
echo "  ./run-qemu.sh"
echo "══════════════════════════════════════════"
