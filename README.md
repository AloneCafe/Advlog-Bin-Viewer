## Yet Another Viewer for ADVLOG.BIN

### Project Title
ADVLOG.BIN 查看器 (基于 MIT 许可证分发的自由软件)

开幕滑稽：Put on a smile face

![9c26b65d4e22cce2829762866ff2db34](https://user-images.githubusercontent.com/20834047/180846731-30410296-26cb-4763-a3a4-06a4c66b6880.png)

### Implemented in C++/Qt
- 在现代操作系统 Windows/Linux/MacOS 下打开并且分析 `ADVLOG.BIN` 日志文件

### Multi-language i18n supported
| 🇨🇳 简体中文 | 🇺🇸 English |

### Repo URL:
https://github.com/AloneCafe/Advlog-Bin-Viewer/

### Last stable release:

- v0.0.1 [MSVC/Qt6/Win64]  [Download AdvlogBinViewer.zip](https://github.com/AloneCafe/Advlog-Bin-Viewer/releases/download/0.0.1/AdvlogBinViewer.zip)

### ADVLOG.BIN 并不神秘
经典的 DOS 硬盘坏道扫描 & 恢复程序 MHDD.EXE 提供了 MAKELOG 命令，它将线性地扫描硬盘的扇区磁道，并且生成一种特定格式的二进制日志文件来记录每个磁道的扫描详情。

如果你不想用 Hex 编辑器来手动地进行分析（这通常十分棘手），也不想重复扫描磁盘（因为你有可能忘记去记录有问题的硬盘磁道），那么不妨试着使用本程序。

如果在此基础上，你正在找一个在非 DOS 系统（比如现代电脑或者上位机环境下）能打开这种格式的通用工具，那么此 Repo 提供的工具也许是一个较好的的选择。

### ADVLOG.BIN? Analyze it!

The classic DOS hard disk bad sector scanning & recovery program MHDD.EXE provides the MAKELOG command, which linearly scans the hard disk sectors and generates a binary log file in a specific format to record each track.

If you don't want to use the Hex editor to do the analysis manually (which is often tricky), and don't want to scan the disk repeatedly (because you might forget to record the problematic hard drive sectors), try this program.

If on this basis, you are looking for a general-purpose tool that can open this format on a non-DOS system (such as a modern computer or a PC environment), then the tool provided by this Repo may be a better choice.

### 动机与想法

- 我不喜欢 Qt，但它可以简单地实现跨平台。
- C++ 是我的主要技能，它对我的开发进度来说既快速又稳定。
- 二进制日志文件（通常叫做 “ADVLOG.BIN”）非常简单，但我无法从 Internet 上找到任何适合我的解决方案。
- 我的硬盘之前有一些问题，这就是我开发这个工具的原因。
- 可以暂且尝试使用工具，虽然它的表现不怎么稳定，因为可能存在不止一个 bug。

### Main feature:

打开并查看由 MHDD.EXE 程序生成的 ADVLOG.BIN 扫描日志文件，以便于硬盘维护者分析扫描结果。

Open & watch the ADVLOG.BIN file which MHDD.EXE that generated, help the hard disk engineer to analyze it.

### Thinkings:
- I am NOT prefer Qt, but it can simply implement cross platform.
- C++ is my main skill, it's fast & stable for my development progress.
- The binary log file (usually named "ADVLOG.BIN") is so simple, but I could not find out any solution that suitable for me from Internet.
- My disk used to taking some problem, that's why I developed this tool.
- Just try this, though it runs not good 'cause some bugs there.

### Screenshots:

<img src=https://user-images.githubusercontent.com/20834047/180843419-80ada95a-a2a6-4123-8df4-af9fd953e832.png width=60% />

<img src=https://user-images.githubusercontent.com/20834047/180843469-012dfc94-eb6f-4f01-877b-5dc3689f7871.png width=60% />


### Non-profit advice: 
作为替代，个人建议使用 Victoria 程序来扫描并修复现代操作系统环境下的硬盘扇区。

As an alternative, I personally recommend using the Victoria program to scan and repair hard drive sectors in modern OS.

### Reference: 

[https://real-world-systems.com/docs/MHDD_en_manual.html](https://real-world-systems.com/docs/MHDD_en_manual.html)

[https://hddguru.com/software/2005.10.02-MHDD/](https://hddguru.com/software/2005.10.02-MHDD/)

（参考文献链接）
