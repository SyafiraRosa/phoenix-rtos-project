#!/bin/bash
# build-toolchain.sh — Build cross-compiler i386-pc-phoenix dari source
#
# Jalankan MANUAL setelah container siap (±30-60 menit):
#   bash .devcontainer/build-toolchain.sh
#
# Hanya perlu dijalankan SEKALI. Setelah itu toolchain tersimpan di ~/toolchains/

set -e

TOOLCHAIN_DIR="$HOME/toolchains"
TOOLCHAIN_BIN="$TOOLCHAIN_DIR/i386-pc-phoenix/i386-pc-phoenix/bin"

echo "════════════════════════════════════════════════════"
echo "  🔨 Building i386-pc-phoenix Toolchain            "
echo "  Estimasi waktu: 30-60 menit                     "
echo "════════════════════════════════════════════════════"
echo ""

# Verifikasi OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "📋 OS: $PRETTY_NAME"
    if [[ "$ID" != "ubuntu" ]]; then
        echo "❌ ERROR: Harus dijalankan di Ubuntu! Detected: $ID"
        echo "   Lakukan 'Rebuild Container' terlebih dahulu di Codespaces."
        exit 1
    fi
fi

# Cek apakah toolchain sudah ada
if [ -f "$TOOLCHAIN_BIN/i386-pc-phoenix-gcc" ]; then
    echo "✅ Toolchain sudah ada di: $TOOLCHAIN_BIN"
    echo "   Versi: $("$TOOLCHAIN_BIN/i386-pc-phoenix-gcc" --version | head -1)"
    echo ""
    echo "   Untuk rebuild paksa, hapus dulu:"
    echo "   rm -rf $TOOLCHAIN_DIR/i386-pc-phoenix"
    exit 0
fi

mkdir -p "$TOOLCHAIN_DIR"

echo "📁 Install dir: $TOOLCHAIN_DIR/i386-pc-phoenix"
echo "⏳ Mulai build... (jangan tutup terminal ini)"
echo ""

START_TIME=$(date +%s)

(cd phoenix-rtos-build/toolchain/ && \
    ./build-toolchain.sh i386-pc-phoenix "$TOOLCHAIN_DIR/i386-pc-phoenix")

END_TIME=$(date +%s)
ELAPSED=$(( (END_TIME - START_TIME) / 60 ))

echo ""
echo "════════════════════════════════════════════════════"
echo "  ✅ Toolchain berhasil dibangun! (${ELAPSED} menit)  "
echo "════════════════════════════════════════════════════"
echo ""
echo "  Aktifkan toolchain di sesi terminal ini:"
echo "  export PATH=\$PATH:$TOOLCHAIN_BIN"
echo ""
echo "  Atau source .bashrc (sudah diset otomatis):"
echo "  source ~/.bashrc"
echo ""
echo "  Cek toolchain:"
echo "  i386-pc-phoenix-gcc --version"
echo ""
echo "  Build Phoenix-RTOS:"
echo "  TARGET=ia32-generic-qemu CONSOLE=serial \\"
echo "    ./phoenix-rtos-build/build.sh all"
echo "════════════════════════════════════════════════════"
