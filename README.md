# Pillowe's Toolkit

## 介绍
Pillowe's Toolkit 是一个基于 PyQt5 构建的桌面应用程序，旨在提供一个多功能工具集，支持文件操作、数据管理和简易的用户交互。该工具包含一个图形用户界面，并集成了系统托盘功能、数据持久化以及简单的用户提示机制。

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

## 使用说明
- **启动程序**：运行 `PilloweMain.py`，程序将加载主界面。
- **添加新条目**：点击“新增项”按钮，可以添加新的数据条目。
- **编辑数据**：双击列表中的条目以执行相关操作。
- **删除条目**：选择一个条目并点击“删除”按钮，确认后将永久删除该条目。
- **最小化到托盘**：点击“最小化”按钮，窗口将隐藏到系统托盘。
- **恢复窗口**：点击系统托盘图标并选择“恢复窗口”。
- **退出程序**：点击“退出”按钮或关闭窗口。

## 参与贡献
欢迎贡献代码或提出改进建议！请遵循以下步骤：
1. Fork 项目仓库。
2. 创建新分支 (`git checkout -b feature/new-feature`)。
3. 提交更改 (`git commit -m 'Add new feature'`)。
4. 推送分支 (`git push origin feature/new-feature`)。
5. 提交 Pull Request 并等待审核。

## 许可证
本项目采用 MIT 许可证。详情请参阅 [LICENSE](LICENSE) 文件。