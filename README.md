# Pillowe's Toolkit

## 介绍
- Pillowe's Toolkit 是一个基于 PyQt5 构建的桌面应用程序，旨在提供一个用户自定义的多功能工具集，支持文件操作、数据管理和简易的用户交互。该工具包含一个图形用户界面，并集成了系统托盘功能、数据持久化以及简单的用户提示机制。
- **目前只在Windows上运行正常！**
## 软件架构
该项目采用 Python 编写，主要依赖以下技术：
- **PyQt5**：用于构建图形用户界面和事件驱动的交互逻辑。
- **asyncio/qasync**：用于处理异步任务和事件循环。
- **psutil**：用于检测当前运行的进程。
- **pyperclip**：用于剪贴板操作。
- **本地文件存储**：通过 `data.json` 文件进行数据持久化。

主要功能模块包括：
- **主窗口 (MainWindow)**：提供核心功能的用户界面。
- **日志窗口 (Changelog)**：显示更新日志。
- **系统托盘支持**：允许最小化到系统托盘并恢复。
- **数据管理**：通过 JSON 文件进行数据存储和修改。

## 安装教程
1. 确保已安装 Python 3.x。
2. 安装所需的依赖库：
   ```bash
   pip install PyQt5 qasync psutil pyperclip
   ```
3. 下载项目源码：
   ```bash
   git clone https://gitee.com/pillowe/toolkit.git
   ```
4. 进入项目目录并运行主程序：
   ```bash
   cd toolkit
   python PilloweMain.py
   ```
5. 其实应该直接下载发行版
## 使用说明

- **即食**

## 参与贡献
欢迎贡献代码或提出改进建议！请遵循以下步骤：
1. Fork 项目仓库。
2. 创建新分支 (`git checkout -b feature/new-feature`)。
3. 提交更改 (`git commit -m 'Add new feature'`)。
4. 推送分支 (`git push origin feature/new-feature`)。
5. 提交 Pull Request 并等待审核。
