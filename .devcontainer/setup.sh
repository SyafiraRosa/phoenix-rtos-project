#!/bin/bash
# setup.sh — Persiapkan environment Phoenix-RTOS di GitHub Codespaces
# Dijalankan otomatis saat postCreateCommand

set -e

echo "🔧 Installing build tools..."
sudo apt-get update -q

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
    scons \
    ca-certificates \
    gnupg \
    lsb-release

# ─── Install Docker jika belum ada ──────────────────────────────────────────
if ! command -v docker &>/dev/null; then
    echo "🐳 Docker tidak ditemukan. Menginstall Docker..."
    curl -fsSL https://get.docker.com | sudo sh
    sudo usermod -aG docker "${USER:-vscode}"
    echo "✅ Docker berhasil diinstall."
else
    echo "✅ Docker sudah tersedia: $(docker --version)"
fi

# ─── Init submodules ─────────────────────────────────────────────────────────
echo "📦 Initializing git submodules..."
git submodule update --init --recursive

echo ""
echo "════════════════════════════════════════════════"
echo "  ✅ Setup selesai! Phoenix-RTOS siap di-build  "
echo "════════════════════════════════════════════════"
echo ""
echo "  1️⃣  Build:"
echo "     TARGET=ia32-generic-qemu CONSOLE=serial ./docker-build.sh all"
echo ""
echo "  2️⃣  Jalankan QEMU:"
echo "     ./run-qemu.sh"
echo ""
echo "  ⚠️  Jika muncul 'permission denied' untuk docker, jalankan:"
echo "     newgrp docker"
echo "════════════════════════════════════════════════"
