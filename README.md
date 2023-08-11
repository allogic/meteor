# Setup

```sh
git clone -b main https://github.com/allogic/meteor
git submodule init
git submodule update
```

# Windows

```sh
choco install git cmake python3
```

# Debian

```sh
apt-get install git cmake python3 build-essential libvulkan1 vulkan-sdk
```

# Wayland

```sh
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```

# Compile

```sh
python meteor cbr test debug
python meteor cbr vulkan debug
```
