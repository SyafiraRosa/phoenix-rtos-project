#!/bin/bash
# setup.sh — Install dependencies Phoenix-RTOS di GitHub Codespaces
# Dijalankan otomatis saat postCreateCommand (cepat, <2 menit)
# Build toolchain dilakukan manual setelah container siap.

set -e

echo "════════════════════════════════════════════════════"
echo "  🔧 Phoenix-RTOS Environment Setup (No Docker)   "
echo "════════════════════════════════════════════════════"

# Verifikasi kita di Ubuntu (bukan Alpine)
if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "📋 OS Detected: $PRETTY_NAME"
    if [[ "$ID" != "ubuntu" ]]; then
        echo "⚠️  WARNING: Expected Ubuntu but got $ID"
        echo "   Pastikan container sudah di-rebuild!"
    fi
fi

# ─── Install semua dependency yang dibutuhkan ─────────────────────────────────
echo ""
echo "📦 [1/3] Installing required build tools..."

sudo apt-get update -q && sudo apt-get install -y \
    build-essential \
    mtd-utils \
    autoconf \
    texinfo \
    genext2fs \
    libtool \
    libhidapi-dev \
    libhidapi-hidraw0 \
    python3 \
    wget \
    bc \
    rsync \
    bzip2 \
    xz-utils \
    qemu-system-x86 \
    scons

echo "✅ Build tools installed."

# ─── Init submodules ───────────────────────────────────────────────────────────
echo ""
echo "📦 [2/3] Initializing git submodules..."
git submodule update --init --recursive
echo "✅ Submodules ready."

# ─── Siapkan direktori toolchain ──────────────────────────────────────────────
echo ""
echo "📁 [3/3] Preparing toolchain directory..."

TOOLCHAIN_DIR="$HOME/toolchains"
TOOLCHAIN_BIN="$TOOLCHAIN_DIR/i386-pc-phoenix/i386-pc-phoenix/bin"
mkdir -p "$TOOLCHAIN_DIR"

# Tambahkan PATH ke .bashrc (persisten)
if ! grep -q "i386-pc-phoenix" "$HOME/.bashrc" 2>/dev/null; then
    echo "export PATH=\$PATH:$TOOLCHAIN_BIN" >> "$HOME/.bashrc"
    echo "✅ PATH entry added to .bashrc"
fi

echo "✅ Ready."

# ─── Done ──────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════════════════"
echo "  ✅ Setup awal selesai!                           "
echo "════════════════════════════════════════════════════"
echo ""
echo "  ⚠️  LANGKAH SELANJUTNYA — Build toolchain dulu:"
echo "  Jalankan sekali saja (±30-60 menit):"
echo ""
echo "  bash .devcontainer/build-toolchain.sh"
echo ""
echo "  Setelah toolchain selesai, baru build Phoenix-RTOS:"
echo "  TARGET=ia32-generic-qemu CONSOLE=serial \\"
echo "    ./phoenix-rtos-build/build.sh all"
echo ""
echo "  Jalankan QEMU:"
echo "  ./run-qemu.sh"
echo "════════════════════════════════════════════════════"
