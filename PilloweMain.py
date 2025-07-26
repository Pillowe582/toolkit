import asyncio
import json
import os
import sys

import psutil as psutil
import pyperclip
import qasync as qasync
import winshell
from win32com.client import Dispatch
from PyQt5 import uic  # 导入uic模块
from PyQt5.QtCore import QFileInfo, QUrl, QCoreApplication, Qt
from PyQt5.QtGui import QIcon, QDesktopServices
from PyQt5.QtWidgets import QMainWindow, QListWidgetItem, QFileDialog, QApplication, QMessageBox, QFileIconProvider, \
    QDialog, QSystemTrayIcon, QAction, QMenu, QDialogButtonBox

QCoreApplication.setAttribute(Qt.AA_DisableHighDpiScaling)
data_list = {}

ver = "1.5.0"


def check_is_solo():
    pid = []
    for proc in psutil.process_iter():
        if proc.name() == "PilloweMain.exe":
            pid.append(proc.pid)
    print("existing PID: ", pid)
    global self_path
    if len(pid) == 1:
        self_pid = psutil.Process(pid[0])
        self_path = os.path.dirname(self_pid.exe())
        print('Selfpath: ', self_path)
        return True
    elif len(pid) == 0:
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg.setIcon(QMessageBox.Warning)
        msg.setWindowTitle("似乎在编辑器中运行？")
        msg.setText("建议先打开一个已经编译好的exe，否则可能出现特性")
        msg.setStandardButtons(QMessageBox.Ok | QMessageBox.Close)
        msg.exec_()
        self_path = "C:"
        return True
    else:
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg.setIcon(QMessageBox.Warning)
        msg.setWindowTitle("程序重复运行！")
        msg.setText("一山不容二虎，请尝试清理后台或重新启动")
        msg.setStandardButtons(QMessageBox.Ok | QMessageBox.Close)
        msg.exec_()
        return False


def get_icon(file_path):
    file_info = QFileInfo(file_path)
    icon_provider = QFileIconProvider()
    return icon_provider.icon(file_info)


def save():
    with open('data.json', 'w') as file:
        json.dump(data, file, indent=4)


class Changelog(QDialog):
    def __init__(self):
        super().__init__()
        uic.loadUi("assets/changelog.ui", self)
        self.resize(1080, 720)
        self.setWindowIcon(QIcon("assets/MainIcon.ico"))


class Settings(QDialog):
    def __init__(self):
        super().__init__()
        uic.loadUi("assets/settings.ui", self)
        self.resize(1080, 720)
        self.setWindowIcon(QIcon("assets/MainIcon.ico"))
        self.autostart_enabled = self.check_autostart()
        self.autostart.setChecked(self.autostart_enabled)
        self.dialogbtn.clicked.connect(self.refresh_settings)

    def refresh_settings(self, button):
        if self.dialogbtn.standardButton(button) == QDialogButtonBox.Apply:
            autostart = self.autostart.isChecked()
            self.toggle_autostart(autostart)

    @staticmethod
    def add_to_startup():
        print("Adding to autostart...")
        try:
            startup_folder = winshell.startup()
            print(f"Startup Folder: {startup_folder}")
            shortcut_path = os.path.join(startup_folder, "PilloweMain.lnk")
            shell = Dispatch('WScript.Shell')
            shortcut = shell.CreateShortCut(shortcut_path)
            shortcut.TargetPath = f"{self_path}\PilloweMain.exe"
            shortcut.WorkingDirectory = self_path
            shortcut.Description = "Pillowe's Toolkit"
            shortcut.save()
            print("开机自启动设置成功")
            return True
        except Exception as e:
            print(f"操作失败: {e}")
            return False

    @staticmethod
    def remove_from_startup():
        try:
            startup_folder = winshell.startup()
            # lnk_path=
            os.remove(startup_folder + '\PilloweMain.lnk')
            print("移除开机自启项成功")
        except Exception as e:
            print(f"移除开机自启项失败：{e}")

    @staticmethod
    def check_autostart():
        """检查是否已设置开机自启动"""
        print("Autostart Checking...")
        print(sys.platform)
        if sys.platform == 'win32':
            startup_folder = winshell.startup()
            link_path = startup_folder + '\PilloweMain.lnk'
            if os.path.isfile(link_path):
                shell = Dispatch('WScript.Shell')
                link = shell.CreateShortCut(link_path)
                print(f"Is Autostarting, link file: {link_path}")
                if not link.TargetPath == self_path + "\PilloweMain.exe":
                    print("Not current path, reset.")
                    Settings.add_to_startup()
                return True
            else:
                print("Not autostarting")
                return False

    def toggle_autostart(self, enable: bool):
        """切换开机自启动状态"""
        if enable:
            self.add_to_startup()
        else:
            self.remove_from_startup()

    def closeEvent(self, event):
        window.settings = None
        super(Settings, self).closeEvent(event)


def empty_json():
    if os.path.exists('data.json'):
        exist = True
    else:
        exist = False
        temp = {
            "0": ["新增项", "https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4", "assets/MainIcon.ico"]}
        with open("data.json", 'w', encoding="utf-8") as f:
            json.dump(temp, f, ensure_ascii=False, indent=4)

    print("data.json exists: ", exist)
    with open("data.json", "r", encoding="utf-8") as f:
        global data
        data = json.load(f)
    if not exist:
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg.setIcon(QMessageBox.Question)
        msg.setWindowTitle("Updated?")
        msg.setText("将以前的data.json文件（若有）复制到本程序根目录内")
        msg.setStandardButtons(QMessageBox.Yes)
        msg.exec_()


def surprise():
    QDesktopServices.openUrl(QUrl("https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4"))


class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        empty_json()
        modify_json_key('data.json')
        self.noticed = False
        self.tray_icon = None
        uic.loadUi("assets/toolkit.ui", self)
        self.resize(1440, 1080)
        self.setWindowIcon(QIcon("assets/MainIcon.ico"))
        self.browserbtn.clicked.connect(self.open_file_dialog)
        self.folderbtn.clicked.connect(self.open_folder_dialog)
        self.appendbtn.clicked.connect(self.append_new)
        self.removebtn.clicked.connect(self.remove_now)
        self.titleinput.textChanged.connect(self.align_title)
        self.applist.currentRowChanged.connect(self.refresh)
        self.applist.doubleClicked.connect(self.execute)
        self.pastebtn.clicked.connect(self.paste_site)
        self.ngguu.triggered.connect(surprise)
        self.changelog.triggered.connect(self.changelog_on)
        self.settings.triggered.connect(self.settings_on)
        self.exit.triggered.connect(self.quit)
        self.executebtn.clicked.connect(self.execute)
        self.changelog = None
        self.settings = None
        self.tray_setup()
        for i in data:
            self.load_new()
            self.applist.setCurrentRow(int(i))
            self.applist.currentItem().setIcon(get_icon(data[str(self.applist.row(self.applist.currentItem()))][2]))

    def tray_setup(self):
        self.tray_icon = QSystemTrayIcon(self)
        self.tray_icon.setIcon(QIcon("assets/MainIcon.ico"))
        self.tray_icon.setVisible(False)
        tray_menu = QMenu(self)
        restore_action = QAction("恢复窗口", self)
        restore_action.triggered.connect(self.restore_window)
        quit_action = QAction("退出", self)
        quit_action.triggered.connect(self.quit)
        cheat_action = QAction("不要点击", self)
        cheat_action.triggered.connect(surprise)
        tray_action = QAction("最小化", self)
        tray_action.triggered.connect(self.hide_to_tray)
        settings_action = QAction("设置...", self)
        settings_action.triggered.connect(self.settings_on)
        changelog_action = QAction("更新日志...", self)
        changelog_action.triggered.connect(self.changelog_on)
        tray_menu.addAction(restore_action)
        tray_menu.addAction(tray_action)
        tray_menu.addSeparator()
        tray_menu.addAction(settings_action)
        tray_menu.addAction(changelog_action)
        tray_menu.addSeparator()
        tray_menu.addAction(cheat_action)
        tray_menu.addAction(quit_action)
        self.tray_icon.setContextMenu(tray_menu)
        self.tray_icon.activated.connect(self.on_tray_icon_activated)

    def hide_to_tray(self):
        self.tray_icon.show()
        self.hide()  # 最小化到托盘时隐藏窗口
        self.tray_icon.setVisible(True)
        if not self.noticed:
            self.tray_icon.showMessage('已最小化至托盘', "本次启动期间不再提示。", QSystemTrayIcon.Information, 1000)
            self.noticed = True

    def restore_window(self):
        self.activateWindow()  # 激活窗口
        self.show()  # 从托盘恢复窗口
        self.tray_icon.setVisible(False)
        self.setWindowState(Qt.WindowNoState)

    def closeEvent(self, event):
        self.hide_to_tray()  # 窗口关闭时隐藏而不是退出
        event.ignore()  # 阻止窗口完全关闭

    def on_tray_icon_activated(self, reason):
        if reason == QSystemTrayIcon.Trigger:
            self.restore_window()

    def execute(self):
        asyncio.create_task(self.execute_async())

    async def execute_async(self):
        cmd = "start" + ' "" "' + os.path.normpath(data[str(self.applist.row(self.applist.currentItem()))][2]) + '"'
        print('cmd ready: ', cmd)
        await asyncio.create_subprocess_shell(cmd, stdout=asyncio.subprocess.DEVNULL, stderr=asyncio.subprocess.DEVNULL)
        print('execute success')

    def quit(self):
        self.setVisible(False)  # 先隐藏托盘图标
        for task in asyncio.all_tasks():
            if task is not asyncio.current_task():  # 排除自身
                task.cancel()
        QApplication.quit()

    def load_new(self):
        item = QListWidgetItem(QIcon("assets/MainIcon.ico"), "新增项")
        self.applist.addItem(item)

    def changelog_on(self):
        if not self.changelog:
            print("Opening Changelog")
            self.changelog = Changelog()
            self.changelog.setWindowModality(Qt.ApplicationModal)
        self.restore_window()
        self.changelog.show()

    def settings_on(self):
        print(self.settings)
        if not self.settings:
            print("Opening Settings")
            self.settings = Settings()
            self.settings.setWindowModality(Qt.ApplicationModal)
        self.restore_window()
        self.settings.show()

    def append_new(self):
        item = QListWidgetItem(QIcon("assets/MainIcon.ico"), "新增项")
        self.applist.addItem(item)
        data[str(len(data))] = ["新增项", "https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4",
                                "assets/MainIcon.ico"]
        save()

    def paste_site(self):
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg.setIcon(QMessageBox.Question)
        msg.setWindowTitle("？？？")
        msg.setText("要粘贴并覆盖吗")
        msg.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
        response = msg.exec_()
        if response == QMessageBox.Yes:
            site = pyperclip.paste()
            self.sitelbl.setHtml('<a href="' + site + '">' + site + '</a>')
            data[str(self.applist.row(self.applist.currentItem()))][1] = site
            save()

    def remove_now(self):
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg.setIcon(QMessageBox.Warning)
        msg.setWindowTitle("？？！")
        msg.setText("这样将会永久失去这一项！（真的很久！）")
        msg.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
        response = msg.exec_()
        if response == QMessageBox.Yes:
            current_row = self.applist.currentRow()  # 先保存当前行号
            if current_row >= 0:  # 确保有选中项
                # 先删除数据再移除UI项
                del data[str(current_row)]
                self.applist.takeItem(current_row)

                # 重新索引JSON数据
                modify_json_key("data.json")

                # 自动选择新项（如果有）
                if self.applist.count() > 0:
                    new_row = min(current_row, self.applist.count() - 1)
                    self.applist.setCurrentRow(new_row)
            save()

    def align_title(self):
        self.applist.currentItem().setText(self.titleinput.toPlainText())
        data[str(self.applist.row(self.applist.currentItem()))][0] = self.titleinput.toPlainText()
        save()

    def open_file_dialog(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(
            self, "选择文件", "", "所有文件 (*)", options=options
        )
        if file_name:
            self.pathlbl.setText(file_name)
            data[str(self.applist.row(self.applist.currentItem()))][2] = file_name
            get_icon(file_name)
            self.applist.currentItem().setIcon(get_icon(file_name))
            self.executebtn.setIcon(get_icon(file_name))
            save()

    def open_folder_dialog(self):
        folder = QFileDialog.getExistingDirectory(
            self, "选择文件夹"
        )
        if folder:
            self.pathlbl.setText(folder)
            data[str(self.applist.row(self.applist.currentItem()))][2] = folder
            get_icon(folder)
            self.applist.currentItem().setIcon(get_icon(folder))
            self.executebtn.setIcon(get_icon(folder))
            save()

    def refresh(self):
        datalist = data[str(self.applist.row(self.applist.currentItem()))]
        self.titleinput.setPlainText(datalist[0])
        self.sitelbl.setHtml('<a href="' + datalist[1] + '">' + datalist[1] + '</a>')
        self.pathlbl.setText(datalist[2])
        self.executebtn.setIcon(get_icon(datalist[2]))


def modify_json_key(file_path):
    values = list(data.values())
    length = len(data)
    data.clear()
    for x in range(0, length):
        data[str(x)] = values[x]
    with open(file_path, 'w', encoding='utf-8') as file:
        json.dump(data, file, ensure_ascii=False, indent=4)


def check_platform():
    if sys.platform == 'win32':
        return True
    else:
        print("Not Windows")
        msg0 = QMessageBox()
        msg0.setWindowIcon(QIcon("assets/MainIcon.ico"))
        msg0.setIcon(QMessageBox.Critical)
        msg0.setWindowTitle("水土不服！")
        msg0.setText("此程序目前仅支持Windows系统，淮南为橘淮北为枳")
        msg0.setStandardButtons(QMessageBox.Ignore | QMessageBox.Close)
        response = msg0.exec_()
        if response == QMessageBox.Ignore:
            print("Ignore!")
            return True
        else:
            print("Exit")
            return False


async def async_main():
    if check_platform() and check_is_solo():
        global window
        window = MainWindow()
        window.show()
        try:
            while True:
                await asyncio.sleep(1)
        except:
            print(f"Seems closed")


if __name__ == "__main__":
    qasync.run(async_main())
    QApplication.setQuitOnLastWindowClosed(False)
