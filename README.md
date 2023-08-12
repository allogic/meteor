# Setup

```
git clone -b main https://github.com/allogic/meteor
git submodule init
git submodule update
```

# Windows

```
choco install git cmake python3
```

# Debian

```
apt-get install git cmake python3 build-essential libvulkan1 vulkan-sdk
```

# Wayland

```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```

# Compile

```
python meteor cbr test debug
python meteor cbr vulkan debug
```
