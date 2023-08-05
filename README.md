# Setup

```sh
git clone -b main https://github.com/allogic/yami
git submodule init
git submodule update
```

# Windows

```sh
choco install make cmake
```

# Debian

```sh
apt-get install make cmake
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