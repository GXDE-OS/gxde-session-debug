# gxde-session-debug

## 背景

GXDE 桌面环境同时提供两种会话：Wayland 会话与 X11 会话。日常反馈的 Bug 中，有些只在 Wayland 会话下出现，另一些只在 X11 会话下复现。在真实登录会话中调试这些问题并不方便：难以在两种会话间快速切换，也无法做到隔离观察。

gxde-session-debug 的目标是提供一个用于调试 GXDE 会话的小工具。它以嵌套（nested）方式运行一个 Wayland 合成器或 X11 Server，并在隔离的进程环境中拉起完整的 GXDE 会话。

这样，调试者可以在一个可随时启停、互不干扰的嵌套会话里复现和排查 Wayland / X11 会话各自的 Bug，而不会影响自己当前正在使用的桌面。
