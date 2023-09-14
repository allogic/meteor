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
sudo apt install git cmake python3 build-essential libvulkan1 vulkan-sdk
```

# Arch

```
sudo pacman -S git cmake python base-devel
```

# Wayland

```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```

# Compile

```
sudo python llvm.py cbi
sudo python project.py cbr test debug
sudo python project.py cbsr game debug
```
