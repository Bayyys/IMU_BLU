import asyncio
import logging

from PyQt6.QtCore import QThread
from bleak import BleakScanner, BleakClient


class ScanThread(QThread):
    def __init__(self, parent=None, comboBox=None) -> None:
        super().__init__(parent=parent)
        self.comboBox = comboBox
        self.is_running = True
        self.devices = []

    async def scan_device(self):
        if not self.is_running:
            return
        logging.info("Start scanning...")
        devices = await BleakScanner.discover()
        logging.info("Scanning done.")
        if not self.is_running:
            return
        self.devices.clear()
        self.comboBox.clear()
        logging.info(f"Clear current devices.")
        for device in devices:
            if device.name and device.name.find("WT899") != -1 and device not in self.devices:
                logging.info(f"Find device: {device.name} - {device.address}")
                self.devices.append(device)
                self.comboBox.addItem(f"{device.name} - {device.address}")
        await asyncio.sleep(3)

    def run(self):
        self.is_running = True
        while self.is_running:
            asyncio.run(self.scan_device())

    def stop(self):
        self.is_running = False


class Device(QThread):
    def __init__(self, parent, lock, address="C8:E1:A5:A1:E8:3A", uuid="0000FFE4-0000-1000-8000-00805F9A34FB") -> None:
        super().__init__(parent=parent)
        self.lock = lock
        self.address = address
        self.uuid = uuid
        self.is_running = True
        self.client = None
        self.start_flag = False

    async def connect_client(self):
        async with self.lock:
            device = None
            con_flag = 0
            while device is None and con_flag < 3:
                logging.info(f"scanning for {self.address}")
                device = await BleakScanner.find_device_by_address(self.address)
                if device is None:
                    logging.info(f"{self.address} rescan...")
                con_flag += 1

            if device is None and con_flag >= 3:
                logging.error(f"Failed to connect to {self.address}")
                return

            self.client = BleakClient(address_or_ble_device=device, disconnected_callback=self.disconnect_callback)
            await self.client.connect()
            logging.info(f"connected to {self.address}")
            self.is_running = True

    def disconnect_callback(self, client: BleakClient):
        self.is_running = False
        logging.info(f"Disconnected from {client.address}")

    async def start_notify_helper(self):
        def callback(_, data):
            # logging.info(f"{self.address} received {data}")
            print(data)

        await self.client.start_notify(self.uuid, callback)
        logging.info(f"Start notify {self.address}.")

    async def stop_notify_helper(self):
        await self.client.stop_notify(self.uuid)
        logging.info(f"Stop notify {self.address}.")

    async def device_disconnect(self):
        await self.client.disconnect()
        logging.info(f"Disconnected from {self.address}.")

    def run(self):
        asyncio.run(self.connect_client())
        while self.is_running:
            QThread.msleep(1000)
        logging.info(f"Device thread stop running.")
        if self.client:
            asyncio.run(self.device_disconnect())
        logging.info(f"Device thread stop.")

    def notify_start(self):
        loop = asyncio.get_event_loop()
        loop.run_until_complete(self.start_notify_helper())

    def notify_stop(self):
        loop = asyncio.get_event_loop()
        loop.run_until_complete(self.stop_notify_helper())

    def stop(self):
        self.is_running = False
        logging.info(f"Device thread stop.")


class Device:
    def __init__(self, lock, address, uuid):
        self.lock = lock
        self.address = address
        self.uuid = uuid
        self.client = None

    async def notify(self):
        async with self.lock:
            device = None
            con_flag = 0
            while device is None and con_flag < 3:
                logging.info("scanning for %s", self.address)
                device = await BleakScanner.find_device_by_address(self.address)
                if device is None:
                    logging.info("%s rescan...", self.address)
                con_flag += 1

            if device is None and con_flag >= 3:
                logging.error("Failed to connect to %s", self.address)
                return

            self.client = BleakClient(device)
            await self.client.connect()
            logging.info("connected to %s", self.address)

        def callback(_, data):
            logging.info("%s received %r", self.address, data)

        await self.client.start_notify(self.uuid, callback)
        await asyncio.sleep(15.0)
        await self.disconnect()

    async def disconnect(self):
        await self.client.disconnect()


def scan_device():
    async def scan():
        devices = await BleakScanner.discover()
        for d in devices:
            if d.name:
                print(d)

    loop = asyncio.get_event_loop()
    loop.run_until_complete(scan())


def main():
    # device_mac_address_list = ["FF:E6:12:3B:08:A6"]
    # device_char_uuid_list = [
    #     "0000FFE4-0000-1000-8000-00805F9A34FB",
    # ]
    device_mac_address_list = ["FF:E6:12:3B:08:A6", "D9:69:71:72:2D:1E"]
    device_char_uuid_list = [
        "0000FFE4-0000-1000-8000-00805F9A34FB",
        "0000FFE4-0000-1000-8000-00805F9A34FB",
    ]
    lock = asyncio.Lock()
    devices = [
        Device(lock, address, uuid)
        for address, uuid in zip(device_mac_address_list, device_char_uuid_list)
    ]

    loop = asyncio.get_event_loop()
    loop.run_until_complete(asyncio.gather(*(d.notify() for d in devices)))


if __name__ == "__main__":
    logging.basicConfig(
        format="[{levelname}] {asctime}: {message}",
        style="{",
        datefmt="%m-%d %H:%M:%S",
        level=logging.INFO,
    )
    scan_device()
    # main()
