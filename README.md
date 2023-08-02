# Install

```sh
pacman -Syu
pacman -Sy base-devel mingw-w64-ucrt-x86_64-clang
```

```sh
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdgshell.c
```