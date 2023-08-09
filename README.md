# Setup

```sh
git clone -b main https://github.com/allogic/meteor
git submodule init
git submodule update
```

# Windows

```sh
choco install git cmake
```

# Debian

```sh
apt-get install git cmake build-essential
```

# Clang

```sh
build_llvm.sh
build_llvm.ps1
```

# Wayland

```sh
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```

# Compile

```sh
build_project.sh <project> <type>
build_project.ps1 <project> <type>
```

# Misc

```sh
clang -print-resource-dir
```