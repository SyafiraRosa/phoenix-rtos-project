#!/bin/bash
# patch-kernel.sh — Apply custom syscall implementations to phoenix-rtos-kernel submodule
#
# Dijalankan SEBELUM build:
#   bash patch-kernel.sh
#   TARGET=ia32-generic-qemu CONSOLE=serial ./phoenix-rtos-build/build.sh all
#
# File kernel yang dimodifikasi disimpan di _kernel_patches/ dan
# di-copy ke submodule yang menunjuk ke upstream (tanpa modifikasi).

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PATCHES_DIR="$SCRIPT_DIR/_kernel_patches"
KERNEL_DIR="$SCRIPT_DIR/phoenix-rtos-kernel"

echo "════════════════════════════════════════════════════"
echo "  🔧 Applying Custom Kernel Patches                "
echo "════════════════════════════════════════════════════"
echo ""

if [ ! -d "$KERNEL_DIR" ]; then
    echo "❌ phoenix-rtos-kernel tidak ditemukan."
    echo "   Jalankan dulu: git submodule update --init --recursive"
    exit 1
fi

if [ ! -d "$PATCHES_DIR" ]; then
    echo "❌ _kernel_patches/ tidak ditemukan."
    exit 1
fi

echo "📁 Patching kernel files..."

# include/syscalls.h — tambah entry syscall baru
cp "$PATCHES_DIR/syscalls.h" "$KERNEL_DIR/include/syscalls.h"
echo "  ✅ include/syscalls.h"

# syscalls.c — handler userspace -> kernel
cp "$PATCHES_DIR/syscalls.c" "$KERNEL_DIR/syscalls.c"
echo "  ✅ syscalls.c"

# proc/threads.c & .h — implementasi setBaseQuanta, getBaseQuanta, dll
cp "$PATCHES_DIR/proc/threads.c" "$KERNEL_DIR/proc/threads.c"
cp "$PATCHES_DIR/proc/threads.h" "$KERNEL_DIR/proc/threads.h"
echo "  ✅ proc/threads.c"
echo "  ✅ proc/threads.h"

# proc/process.c & .h — implementasi setQuantaForProcess, getQuantaForProcess
cp "$PATCHES_DIR/proc/process.c" "$KERNEL_DIR/proc/process.c"
cp "$PATCHES_DIR/proc/process.h" "$KERNEL_DIR/proc/process.h"
echo "  ✅ proc/process.c"
echo "  ✅ proc/process.h"

# posix/ — implementasi maxChildrenInRanges, setVariable, getVariable
cp "$PATCHES_DIR/posix/posix.c"         "$KERNEL_DIR/posix/posix.c"
cp "$PATCHES_DIR/posix/posix.h"         "$KERNEL_DIR/posix/posix.h"
cp "$PATCHES_DIR/posix/posix_private.h" "$KERNEL_DIR/posix/posix_private.h"
echo "  ✅ posix/posix.c"
echo "  ✅ posix/posix.h"
echo "  ✅ posix/posix_private.h"

echo ""
echo "════════════════════════════════════════════════════"
echo "  ✅ Kernel patch selesai! Siap untuk build.       "
echo "════════════════════════════════════════════════════"
echo ""
echo "  Sekarang jalankan:"
echo "  TARGET=ia32-generic-qemu CONSOLE=serial \\"
echo "    ./phoenix-rtos-build/build.sh all"
echo "════════════════════════════════════════════════════"
