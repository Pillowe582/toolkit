import json
import os
import sys
import asyncio
import pyperclip
import qasync as qasync
from PyQt5 import uic  # 导入uic模块
from PyQt5.QtCore import QFileInfo, QUrl
from PyQt5.QtGui import QIcon, QDesktopServices
from PyQt5.QtWidgets import QMainWindow, QListWidgetItem, QFileDialog, QApplication, QMessageBox, QFileIconProvider, \
    QDialog
if os.path.exists('data.json'):
    exist=True
else:
    exist=False
    temp={"0":["新增项", "https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4", "C:/"]}
    with open("data.json",'w',encoding="utf-8") as f:
        json.dump(temp, f, ensure_ascii=False, indent=4)
with open("data.json", "r", encoding="utf-8") as f:
    data = json.load(f)
print(data)
print(exist)
data_list = {}


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
        self.setWindowIcon(QIcon("assets/PilloweIcon.ico"))


class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        uic.loadUi("assets/toolkit.ui", self)
        self.resize(1440, 1080)
        self.setWindowIcon(QIcon("assets/PilloweIcon.ico"))
        self.browserbtn.clicked.connect(self.open_file_dialog)
        self.appendbtn.clicked.connect(self.append_new)
        self.removebtn.clicked.connect(self.remove_now)
        self.titleinput.textChanged.connect(self.align_title)
        self.applist.currentRowChanged.connect(self.refresh)
        self.pastebtn.clicked.connect(self.paste_site)
        self.ngguu.triggered.connect(self.surprise)
        self.changelog.triggered.connect(self.changelog_on)
        self.executebtn.clicked.connect(self.execute)
        self.changelog = None
        self.empty_json()
        for i in data:
            self.load_new()
            self.applist.setCurrentRow(int(i))
            self.applist.currentItem().setIcon(get_icon(data[str(self.applist.row(self.applist.currentItem()))][2]))

    def execute(self):
        asyncio.create_task(self.execute_async())

    async def execute_async(self):
        cmd = "start" + ' "" "' + os.path.normpath(data[str(self.applist.row(self.applist.currentItem()))][2]) + '"'
        print('cmd ready')
        await asyncio.create_subprocess_shell(cmd, stdout=asyncio.subprocess.DEVNULL, stderr=asyncio.subprocess.DEVNULL)
        print('success')

    def empty_json(self):
        if not exist:
            msg=QMessageBox()
            msg.setWindowIcon(QIcon("assets/PilloweIcon.ico"))
            msg.setIcon(QMessageBox.Question)
            msg.setWindowTitle("Updated?")
            msg.setText("将以前的data.json文件（若有）复制到本程序根目录内")
            msg.setStandardButtons(QMessageBox.Yes)
            msg.exec_()


    def load_new(self):
        item = QListWidgetItem(QIcon("assets/PilloweIcon.ico"), "新增项")
        self.applist.addItem(item)

    def surprise(self):
        QDesktopServices.openUrl(QUrl("https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4"))

    def changelog_on(self):
        if not self.changelog:
            self.changelog = Changelog()
        self.changelog.show()

    def append_new(self):
        item = QListWidgetItem(QIcon("assets/PilloweIcon.ico"), "新增项")
        self.applist.addItem(item)
        data[str(len(data))] = ["新增项", "https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4", "C:/"]
        save()

    def paste_site(self):
        msg = QMessageBox()
        msg.setWindowIcon(QIcon("assets/PilloweIcon.ico"))
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
        msg.setWindowIcon(QIcon("assets/PilloweIcon.ico"))
        msg.setIcon(QMessageBox.Warning)
        msg.setWindowTitle("？？？")
        msg.setText("这样将会永久失去这一项！（真的很久！）")
        msg.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
        response=msg.exec_()
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

    def refresh(self):
        data_list = data[str(self.applist.row(self.applist.currentItem()))]
        self.titleinput.setPlainText(data_list[0])
        self.sitelbl.setHtml('<a href="' + data_list[1] + '">' + data_list[1] + '</a>')
        self.pathlbl.setText(data_list[2])
        self.executebtn.setIcon(get_icon(data_list[2]))

    def closeEvent(self, event):
        for task in asyncio.all_tasks():
            if task is not asyncio.current_task():  # 排除自身
                task.cancel()


def modify_json_key(file_path):
    values = list(data.values())
    length = len(data)
    data.clear()
    for x in range(0, length):
        data[str(x)] = values[x]
    with open(file_path, 'w', encoding='utf-8') as file:
        json.dump(data, file, ensure_ascii=False, indent=4)


async def async_main():
    modify_json_key('data.json')
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    fut = asyncio.get_event_loop().create_future()
    print(fut)
    await fut
    sys.exit(app.exec_())
    print('bye')


if __name__ == "__main__":
    qasync.run(async_main())

