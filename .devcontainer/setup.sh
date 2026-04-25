#!/bin/bash
# setup.sh — Build Phoenix-RTOS native toolchain di GitHub Codespaces (tanpa Docker)
# Dijalankan otomatis saat postCreateCommand
# Mengikuti: https://github.com/phoenix-rtos/phoenix-rtos-doc (Building using the native toolchain)

set -e

echo "════════════════════════════════════════════════════"
echo "  🔧 Phoenix-RTOS Native Build Setup (No Docker)  "
echo "════════════════════════════════════════════════════"

# ─── 1. Install semua dependency yang dibutuhkan ──────────────────────────────
echo ""
echo "📦 [1/4] Installing required build tools..."

sudo apt-get update -q && sudo apt-get install -y \
    build-essential \
    mtd-utils \
    autoconf \
    texinfo \
    genext2fs \
    libtool \
    libhidapi-dev \
    python3 \
    wget \
    bc \
    rsync \
    bzip2 \
    xz-utils \
    qemu-system-x86

echo "✅ Build tools installed."

# ─── 2. Init submodules ───────────────────────────────────────────────────────
echo ""
echo "📦 [2/4] Initializing git submodules..."
git submodule update --init --recursive
echo "✅ Submodules ready."

# ─── 3. Build cross-compiler toolchain ───────────────────────────────────────
echo ""
echo "🔨 [3/4] Building i386-pc-phoenix cross-compiler toolchain..."
echo "   ⚠️  Proses ini memakan waktu ±30-60 menit. Harap tunggu."
echo ""

TOOLCHAIN_DIR="$HOME/toolchains"
mkdir -p "$TOOLCHAIN_DIR"

if [ ! -f "$TOOLCHAIN_DIR/i386-pc-phoenix/i386-pc-phoenix/bin/i386-pc-phoenix-gcc" ]; then
    echo "   Toolchain belum ada, memulai build..."
    (cd phoenix-rtos-build/toolchain/ && \
        ./build-toolchain.sh i386-pc-phoenix "$TOOLCHAIN_DIR/i386-pc-phoenix")
    echo "✅ Toolchain berhasil dibangun."
else
    echo "✅ Toolchain sudah ada, skip build."
fi

# ─── 4. Set PATH ─────────────────────────────────────────────────────────────
echo ""
echo "🔧 [4/4] Configuring PATH..."

TOOLCHAIN_BIN="$TOOLCHAIN_DIR/i386-pc-phoenix/i386-pc-phoenix/bin"

# Tambahkan ke .bashrc agar persisten
if ! grep -q "i386-pc-phoenix" "$HOME/.bashrc"; then
    echo "export PATH=\$PATH:$TOOLCHAIN_BIN" >> "$HOME/.bashrc"
fi

# Export untuk sesi ini juga
export PATH="$PATH:$TOOLCHAIN_BIN"
echo "✅ PATH configured."

# ─── Done ─────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════════════════"
echo "  ✅ Setup selesai! Phoenix-RTOS siap di-build     "
echo "════════════════════════════════════════════════════"
echo ""
echo "  Cara build (TANPA Docker):"
echo "  TARGET=ia32-generic-qemu CONSOLE=serial \\"
echo "    ./phoenix-rtos-build/build.sh all"
echo ""
echo "  Jalankan QEMU setelah build:"
echo "  ./run-qemu.sh"
echo "════════════════════════════════════════════════════"
