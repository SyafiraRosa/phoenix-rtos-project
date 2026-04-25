#!/bin/bash
#
# run-qemu.sh — Jalankan Phoenix-RTOS di QEMU (mode Codespaces/headless)
#
# Menggunakan serial console karena Codespaces tidak punya display grafis.
# Output Phoenix-RTOS akan muncul langsung di terminal ini.
#

DISK_IMAGE="_boot/ia32-generic-qemu/phoenix.disk"

if [ ! -f "$DISK_IMAGE" ]; then
    echo "❌ Disk image tidak ditemukan: $DISK_IMAGE"
    echo "   Jalankan build terlebih dahulu:"
    echo "   TARGET=ia32-generic-qemu CONSOLE=serial ./docker-build.sh all"
    exit 1
fi

echo "🚀 Menjalankan Phoenix-RTOS di QEMU (serial mode)..."
echo "   Tekan Ctrl+A kemudian X untuk keluar dari QEMU"
echo "─────────────────────────────────────────────────"

qemu-system-i386 \
    -hda "$DISK_IMAGE" \
    -nographic \
    -serial mon:stdio \
    -m 256 \
    -net nic,model=rtl8139 \
    -net user,hostfwd=tcp::5556-:23 \
    -no-reboot
