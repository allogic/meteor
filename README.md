# Setup

```sh
git clone -b main https://github.com/allogic/yami
git submodule init
```

# Windows MinGw

```sh
pacman -Syu
pacman -Sy base-devel cmake
```

# Linux Debian

```sh
apt-get update
apt-get install build-essential cmake
```

# Clang & Lld

```sh
build-llvm.sh
build-llvm.ps1
```

# Wayland

```sh
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```