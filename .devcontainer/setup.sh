#!/bin/bash
# setup.sh — Install build dependencies untuk Phoenix-RTOS di GitHub Codespaces
# Dijalankan otomatis via postCreateCommand (harus CEPAT dan TIDAK boleh gagal)
# Submodule init dan toolchain build dilakukan MANUAL setelah container siap.

set -e

echo "════════════════════════════════════════════════════"
echo "  🔧 Installing Phoenix-RTOS Build Dependencies   "
echo "════════════════════════════════════════════════════"

echo ""
echo "📋 OS: $(. /etc/os-release && echo "$PRETTY_NAME")"
echo ""

# Install semua dependency dari apt (Ubuntu 22.04)
echo "📦 Installing packages via apt-get..."

sudo apt-get update -q
sudo apt-get install -y --no-install-recommends \
    build-essential \
    mtd-utils \
    autoconf \
    automake \
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
    scons \
    patch

echo "✅ Packages installed."

# Siapkan direktori toolchain dan set PATH
TOOLCHAIN_BIN="$HOME/toolchains/i386-pc-phoenix/i386-pc-phoenix/bin"
if ! grep -q "i386-pc-phoenix" "$HOME/.bashrc" 2>/dev/null; then
    echo "export PATH=\$PATH:$TOOLCHAIN_BIN" >> "$HOME/.bashrc"
fi

echo ""
echo "════════════════════════════════════════════════════"
echo "  ✅ Dependency setup selesai!                     "
echo "════════════════════════════════════════════════════"
echo ""
echo "  Lanjutkan secara MANUAL di terminal:"
echo ""
echo "  1️⃣  Init submodules:"
echo "     git submodule update --init --recursive"
echo ""
echo "  2️⃣  Build cross-compiler (sekali saja, ~30-60 menit):"
echo "     bash .devcontainer/build-toolchain.sh"
echo ""
echo "  3️⃣  Build Phoenix-RTOS:"
echo "     TARGET=ia32-generic-qemu CONSOLE=serial \\"
echo "       ./phoenix-rtos-build/build.sh all"
echo "════════════════════════════════════════════════════"
