import asyncio
import dataclasses
import logging
import sys
from functools import cached_property

from PyQt6.QtCore import QThread, QObject, pyqtSignal
from PyQt6.QtWidgets import QApplication, QMainWindow
from bleak import BleakScanner, BleakClient, BLEDevice

import qasync

from ui.ble_demo_ui import Ui_BLE_DEMO


@dataclasses.dataclass
class QBleakClient(QObject):
    lock: asyncio.Lock
    device: BLEDevice | str
    client: BleakClient = None
    notify_received: bool = False
    notify_uuid: str = "0000FFE4-0000-1000-8000-00805F9A34FB"

    device_disconnected = pyqtSignal()
    con_error = pyqtSignal()

    def __post_init__(self):
        super().__init__()
        if isinstance(self.device, str):
            self.address = self.device
        else:
            self.address = self.device.address

    async def con(self):
        async with self.lock:
            device = None
            con_flag = 0
            while device is None and con_flag < 3:
                logging.info(f"Scanning for {self.address}")
                device = await BleakScanner.find_device_by_address(self.address)
                if device is None:
                    logging.info(f"{self.address} rescan...")

            if device is None or con_flag >= 3:
                self.con_error.emit()
                logging.error(f"Failed to connect to {self.address}")
                return

            self.device = device
            self.client = BleakClient(address_or_ble_device=device, disconnected_callback=self._handle_disconnect)
            await self.client.connect()
            if self.client and self.client.is_connected:
                logging.info(f"Connected to {self.address}")
            else:
                logging.error(f"Failed to connect to {self.address}")

    async def start(self):
        await self.client.start_notify(self.notify_uuid, self._handle_notify)
        logging.info(f"Start notify {self.address}")

    async def pause(self):
        await self.client.stop_notify(self.notify_uuid)
        self.notify_received = False
        logging.info(f"Stop notify {self.address}")

    async def discon(self):
        await self.client.disconnect()

    def _handle_notify(self, _, data: bytes) -> None:
        self.notify_received = True
        logging.info(f"{self.address} received {data}")

    def _handle_disconnect(self, _) -> None:
        logging.info(f"Disconnected from {self.address}")
        self.notify_received = False


class BleWin(QMainWindow, Ui_BLE_DEMO):
    def __init__(self) -> None:
        super().__init__()
        self.setupUi(self)
        self.btn_serach.clicked.connect(self.scan_device)
        self.btn_con.clicked.connect(self.device_connect)
        self.btn_start.clicked.connect(self.device_start_notify)
        self.btn_pause.clicked.connect(self.device_stop_notify)
        self.btn_discon.clicked.connect(self.device_disconnect)

        self.client = None
        self.address = [
            "CF:88:10:CB:0A:2E",
            "F3:8C:9A:CF:F0:D7",
            "D9:69:71:72:2D:1E",
            "FF:E6:12:3B:08:A6",
            "E5:4B:A7:C7:85:32",
            # "C8:E1:A5:A1:E8:3A",
        ]
        self.client_group = []
        self.lock = asyncio.Lock()

        self.address_index = 0

    @qasync.asyncSlot()
    async def device_connect(self):
        # if self.client:
        #     await self.client.stop()
        # device = self.cb_device.currentData()
        # if isinstance(device, BLEDevice):
        #     logging.info(f"Connecting to {device.address}")
        #     self.client = QBleakClient(self.lock, device)
        #     self.client.device_disconnected.connect(self.device_disconnect)
        #     await self.client.con()
        address = self.address[self.address_index]
        logging.info(f"Connecting to {address}")
        client = QBleakClient(self.lock, address)
        client.device_disconnected.connect(self.device_disconnect)
        await client.con()
        if client.client:
            self.client_group.append(client)
            self.address_index += 1

    @qasync.asyncSlot()
    async def device_start_notify(self):
        for client in self.client_group:
            await client.start()
        # if self.client:
        #     await self.client.start()

    @qasync.asyncSlot()
    async def device_stop_notify(self):
        for client in self.client_group:
            await client.pause()
        # if self.client:
        #     await self.client.pause()

    @qasync.asyncSlot()
    async def device_disconnect(self):
        for client in self.client_group:
            await client.discon()
        # if self.client:
        #     await self.client.discon()
        self.client_group = []

    @qasync.asyncSlot()
    async def scan_device(self):
        self.cb_device.clear()
        logging.info("Start BLE scanner...")
        devices = await BleakScanner.discover()
        logging.info("Finish BLE scanner.")
        for device in devices:
            logging.info(f"Found device {device.name} - {device.address}")
            if device.name and device.name.find("WT901") != -1:
                self.cb_device.addItem(f"{device.name} - {device.address}", device)
                logging.info(f"Add device {device.name} - {device.address} to combobox.")
        logging.info("Add devices to combobox.")


def main():
    logging.basicConfig(
        level=logging.INFO,
        format="[{levelname}] {asctime}: {message}",
        style="{",
        datefmt="%Y-%m-%d %H:%M:%S",
    )
    app = QApplication(sys.argv)

    loop = qasync.QEventLoop(app)
    asyncio.set_event_loop(loop)

    app_close_event = asyncio.Event()
    app.aboutToQuit.connect(app_close_event.set)

    window = BleWin()
    window.show()

    loop.run_until_complete(app_close_event.wait())
    loop.close()


if __name__ == "__main__":
    main()
