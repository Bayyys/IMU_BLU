from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QMainWindow,
    QVBoxLayout,
    QWidget,
    QComboBox,
)
import bleak


class BLEWin(QMainWindow):
    def __init__(
        self, parent=None, flags: Qt.WindowType = Qt.WindowType.Window
    ) -> None:
        super().__init__(parent=parent, flags=flags)
        self.setWindowTitle("BLE")
        self.resize(800, 600)
        self.widget = QWidget()
        self.vlayout = QVBoxLayout()
        self.widget.setLayout(self.vlayout)
        self.setCentralWidget(self.widget)

        self.cb_ble_device = QComboBox()


def main():
    import sys

    from PyQt6.QtWidgets import QApplication

    app = QApplication(sys.argv)
    win = BLEWin(flags=Qt.WindowType.Window)
    win.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
