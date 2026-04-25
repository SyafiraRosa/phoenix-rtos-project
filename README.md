# Phoenix-RTOS Project — SyafiraRosa Fork

[![ci](https://github.com/phoenix-rtos/phoenix-rtos-project/actions/workflows/ci.yml/badge.svg)](https://github.com/phoenix-rtos/phoenix-rtos-project/actions/workflows/ci.yml)
[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/SyafiraRosa/phoenix-rtos-project)

> **Phoenix-RTOS** adalah sistem operasi real-time (RTOS) yang skalabel untuk IoT, berbasis microkernel sendiri. Dapat digunakan pada perangkat kecil berbasis mikrokontroler maupun sistem komputer canggih.

---

## 📋 Daftar Isi

- [Tentang Fork Ini](#-tentang-fork-ini)
- [Update yang Dilakukan](#-update-yang-dilakukan)
- [Cara Menjalankan di GitHub Codespaces](#-cara-menjalankan-di-github-codespaces)
- [Cara Build Manual (Lokal)](#-cara-build-manual-lokal)
- [Program Pengguna yang Ditambahkan](#-program-pengguna-yang-ditambahkan)
- [Dokumentasi Syscall Baru](#-dokumentasi-syscall-baru)
- [Struktur Direktori](#-struktur-direktori)
- [Referensi](#-referensi)

---

## 🔖 Tentang Fork Ini

Fork ini merupakan modifikasi dari [phoenix-rtos/phoenix-rtos-project](https://github.com/phoenix-rtos/phoenix-rtos-project) (versi `v3.1.0`) yang ditujukan untuk keperluan penelitian dan pengembangan sistem operasi Phoenix-RTOS.

Perubahan utama meliputi:
- Penambahan **syscall baru** pada kernel untuk manajemen scheduling
- Penambahan **syscall `maxChildrenInRanges`** untuk manajemen proses
- Penambahan **syscall `getVariable` / `setVariable`** untuk shared variable antar proses
- Penambahan beberapa **program user** sebagai demonstrasi dan pengujian syscall baru
- Konfigurasi **GitHub Codespaces** agar project dapat langsung dijalankan di browser

**Branch aktif:** `syafirarosa/scheduling-syscalls`

---

## ✨ Update yang Dilakukan

### 1. Penambahan Syscall Scheduling

File yang dimodifikasi:
- `phoenix-rtos-kernel` — implementasi syscall baru di kernel
- `libphoenix` — header dan binding library
- `_targets/build.project.ia32-generic` — konfigurasi build target

Syscall baru yang ditambahkan:

| Syscall | Deskripsi |
|---|---|
| `getBaseQuanta()` | Mendapatkan nilai base quanta scheduler |
| `setBaseQuanta(quanta)` | Mengatur nilai base quanta scheduler |
| `getQuantaForProcess(pid)` | Mendapatkan quanta untuk proses tertentu |
| `setQuantaForProcess(pid, quanta)` | Mengatur quanta untuk proses tertentu |
| `maxChildrenInRanges(min, max, &pid, &count)` | Mencari proses dengan jumlah child terbanyak dalam rentang |
| `getVariable()` | Membaca shared variable dari kernel |
| `setVariable(value)` | Menyimpan shared variable ke kernel |

### 2. Penambahan Program User (`_user/`)

| Program | Deskripsi |
|---|---|
| `getBaseQuanta` | Menampilkan nilai base quanta saat ini |
| `setBaseQuanta <value>` | Mengatur nilai base quanta |
| `getQuantaForProcess <pid>` | Menampilkan quanta proses berdasarkan PID |
| `setQuantaForProcess <pid> <value>` | Mengatur quanta proses berdasarkan PID |
| `maxChildrenInRanges` | Demo pencarian proses dengan child terbanyak |
| `infiniteProcesses <n>` | Membuat `n` child process yang berjalan terus (untuk testing) |
| `getVariable` | Membaca nilai shared variable |
| `setVariable <value>` | Menyimpan nilai shared variable |

### 3. Konfigurasi Jaringan (`rc.psh`)

File `_projects/ia32-generic-qemu/rootfs-overlay/etc/rc.psh` diperbarui:
- Menambahkan konfigurasi IP statis (`10.0.2.15`) via `ifconfig`
- Menambahkan `telnetd` untuk akses remote ke sistem Phoenix-RTOS via QEMU

### 4. Konfigurasi GitHub Codespaces

Ditambahkan `.devcontainer/devcontainer.json` agar project dapat langsung di-build di GitHub Codespaces tanpa setup manual.

---

## 🚀 Cara Menjalankan di GitHub Codespaces

GitHub Codespaces memungkinkan Anda menjalankan project ini langsung dari browser tanpa perlu install apapun di komputer lokal.

### Langkah 1 — Buka Codespace

1. Buka halaman repository di GitHub: `https://github.com/SyafiraRosa/phoenix-rtos-project`
2. Pastikan berada di branch **`syafirarosa/scheduling-syscalls`**
3. Klik tombol hijau **`<> Code`**
4. Pilih tab **`Codespaces`**
5. Klik **`Create codespace on syafirarosa/scheduling-syscalls`**

> Atau klik badge ini:
> [![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/SyafiraRosa/phoenix-rtos-project?quickstart=1)

### Langkah 2 — Tunggu Environment Siap

Codespace akan otomatis:
- Menarik Docker image `phoenixrtos/build`
- Menjalankan `git submodule update --init --recursive`
- Menampilkan pesan ✅ saat siap

### Langkah 3 — Build Project

Di terminal Codespace, jalankan:

```bash
# Build untuk target ia32-generic-qemu (default)
TARGET=ia32-generic-qemu ./phoenix-rtos-build/build.sh all
```

Atau gunakan Docker build helper:

```bash
TARGET=ia32-generic-qemu ./docker-build.sh all
```

### Langkah 4 — Jalankan di QEMU

Setelah build selesai, jalankan emulator:

```bash
# Install QEMU jika belum ada
sudo apt-get install -y qemu-system-x86

# Jalankan Phoenix-RTOS
TARGET=ia32-generic-qemu ./phoenix-rtos-build/build.sh qemu
```

> **Catatan:** QEMU mungkin tidak tersedia langsung di Codespaces. Alternatifnya, gunakan build artifact yang dihasilkan.

---

## 🛠️ Cara Build Manual (Lokal)

### Prasyarat

- Ubuntu 22.04 LTS (atau Docker)
- Tools: `git`, `make`, `gcc`, `qemu-system-x86`
- Atau cukup Docker (direkomendasikan)

### Clone Repository

```bash
git clone https://github.com/SyafiraRosa/phoenix-rtos-project.git
cd phoenix-rtos-project
git checkout syafirarosa/scheduling-syscalls
git submodule update --init --recursive
```

### Build dengan Docker (Direkomendasikan)

```bash
TARGET=ia32-generic-qemu ./docker-build.sh all
```

### Build Manual

```bash
# Install dependencies
sudo apt-get install -y make gcc-multilib g++-multilib \
    libusb-1.0-0-dev scons python3 qemu-system-x86

# Build
TARGET=ia32-generic-qemu source ./phoenix-rtos-build/build.sh all
```

### Jalankan di QEMU

```bash
TARGET=ia32-generic-qemu ./phoenix-rtos-build/build.sh qemu
```

---

## 📦 Program Pengguna yang Ditambahkan

Semua program berada di direktori `_user/` dan akan terinstall ke `/usr/bin/` dalam filesystem Phoenix-RTOS.

### `getBaseQuanta`

Menampilkan nilai base quanta scheduler saat ini.

```bash
# Di dalam shell Phoenix-RTOS:
/usr/bin/getBaseQuanta
# Output: Base quanta: 10
```

### `setBaseQuanta <value>`

Mengatur nilai base quanta scheduler.

```bash
/usr/bin/setBaseQuanta 20
# Output: Base quanta set to: 20
```

### `getQuantaForProcess <pid>`

Mendapatkan nilai quanta yang dialokasikan untuk proses tertentu.

```bash
/usr/bin/getQuantaForProcess 1
# Output: Process 1 has quanta 10
```

### `setQuantaForProcess <pid> <value>`

Mengatur nilai quanta untuk proses tertentu berdasarkan PID.

```bash
/usr/bin/setQuantaForProcess 1 15
# Output: Quanta for process 1 set to: 15
```

### `infiniteProcesses <n>`

Membuat `n` child process yang berjalan terus-menerus. Berguna untuk menguji syscall scheduling.

```bash
/usr/bin/infiniteProcesses 3
# Output: Create 3 children.
# (Proses berjalan terus, gunakan Ctrl+C untuk berhenti)
```

### `maxChildrenInRanges`

Demo lengkap untuk mencari proses dengan jumlah child terbanyak dalam berbagai rentang.

```bash
/usr/bin/maxChildrenInRanges
# Output:
# Creating child processes...
# Main process PID: 5
#
# === Test 1: Full range (0–1000) ===
# PID: 5, Children: 3
#
# === Test 2: Small range (0–10) ===
# PID: 5, Children: 3
# ...
```

### `getVariable`

Membaca nilai shared variable dari kernel.

```bash
/usr/bin/getVariable
# Output: Read value: 42
```

### `setVariable <value>`

Menyimpan nilai shared variable ke kernel.

```bash
/usr/bin/setVariable 42
# Output: Variable set to value: 42
```

---

## 📖 Dokumentasi Syscall Baru

### Scheduling Syscalls

Syscall scheduling tersedia melalui header `<sys/scheduling.h>` (dari `libphoenix`).

#### `getBaseQuanta()`

```c
#include <sys/scheduling.h>

int getBaseQuanta(void);
```

- **Deskripsi:** Mengembalikan nilai base quanta scheduler.
- **Return:** Nilai quanta (int positif), atau negatif jika error.

#### `setBaseQuanta(int quanta)`

```c
#include <sys/scheduling.h>

int setBaseQuanta(int quanta);
```

- **Deskripsi:** Mengatur nilai base quanta scheduler.
- **Parameter:** `quanta` — nilai quanta baru (harus > 0).
- **Return:** `1` jika berhasil, `0` jika gagal.

#### `getQuantaForProcess(int pid)`

```c
#include <sys/scheduling.h>

int getQuantaForProcess(int pid);
```

- **Deskripsi:** Mendapatkan nilai quanta untuk proses tertentu.
- **Parameter:** `pid` — Process ID target.
- **Return:** Nilai quanta proses, atau `-1` jika proses tidak ditemukan.

#### `setQuantaForProcess(int pid, int quanta)`

```c
#include <sys/scheduling.h>

int setQuantaForProcess(int pid, int quanta);
```

- **Deskripsi:** Mengatur nilai quanta untuk proses tertentu.
- **Parameter:** `pid` — Process ID, `quanta` — nilai quanta baru.
- **Return:** `1` jika berhasil, `0` jika gagal.

---

### Process Syscalls

#### `maxChildrenInRanges(int min, int max, int *pid, int *count)`

```c
#include <sys/maxchildreninranges.h>

int maxChildrenInRanges(int min, int max, int *pid, int *count);
```

- **Deskripsi:** Mencari proses yang memiliki jumlah child process paling banyak dalam rentang `[min, max]`.
- **Parameter:**
  - `min` — batas bawah jumlah child
  - `max` — batas atas jumlah child
  - `*pid` — output: PID proses yang ditemukan
  - `*count` — output: jumlah child proses tersebut
- **Return:** `0` jika ditemukan, `-1` jika tidak ada proses yang memenuhi kriteria.

---

### Variable Syscalls

#### `getVariable()`

```c
#include <sys/getsetvariable.h>

int getVariable(void);
```

- **Deskripsi:** Membaca nilai shared variable yang tersimpan di kernel.
- **Return:** Nilai integer yang tersimpan.

#### `setVariable(int value)`

```c
#include <sys/getsetvariable.h>

void setVariable(int value);
```

- **Deskripsi:** Menyimpan nilai integer ke shared variable di kernel.
- **Parameter:** `value` — nilai yang akan disimpan.

---

## 📁 Struktur Direktori

```
phoenix-rtos-project/
├── .devcontainer/
│   └── devcontainer.json          # ✨ Konfigurasi GitHub Codespaces
├── _projects/
│   └── ia32-generic-qemu/
│       ├── build.project          # ✏️ Diperbarui
│       └── rootfs-overlay/
│           └── etc/
│               └── rc.psh         # ✏️ Diperbarui (ifconfig + telnetd)
├── _targets/
│   └── build.project.ia32-generic # ✏️ Diperbarui
├── _user/
│   ├── Makefile                   # ✏️ Diperbarui (subdir baru)
│   ├── getBaseQuanta/             # ✨ Baru
│   ├── setBaseQuanta/             # ✨ Baru
│   ├── getQuantaForProcess/       # ✨ Baru
│   ├── setQuantaForProcess/       # ✨ Baru
│   ├── infiniteProcesses/         # ✨ Baru
│   ├── maxChildrenInRanges/       # ✨ Baru
│   ├── getVariable/               # ✨ Baru
│   └── setVariable/               # ✨ Baru
├── libphoenix/                    # ✏️ Diperbarui (header syscall baru)
├── phoenix-rtos-kernel/           # ✏️ Diperbarui (implementasi syscall)
├── docker-build.sh                # Script build via Docker
└── README.md                      # ✏️ Dokumentasi ini
```

> **Legend:** ✨ = File/direktori baru | ✏️ = Diperbarui dari upstream

---

## 📚 Referensi

- [Phoenix-RTOS Official Documentation](https://github.com/phoenix-rtos/phoenix-rtos-doc)
- [Building Phoenix-RTOS](https://github.com/phoenix-rtos/phoenix-rtos-doc/blob/master/building/README.md)
- [Running on QEMU (Quickstart)](https://github.com/phoenix-rtos/phoenix-rtos-doc/blob/master/quickstart/README.md)
- [Phoenix-RTOS Website](https://phoenix-rtos.com)
- [Upstream Repository](https://github.com/phoenix-rtos/phoenix-rtos-project)

---

<div align="center">

**Dibuat oleh [SyafiraRosa](https://github.com/SyafiraRosa)** · Fork dari phoenix-rtos v3.1.0

</div>
