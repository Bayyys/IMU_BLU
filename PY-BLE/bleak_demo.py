import asyncio
import logging

from bleak import BleakScanner, BleakClient


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
        self.client.stop_notify(self.uuid)
        await asyncio.sleep(15.0)
        await self.client.start_notify(self.uuid, callback)
        await self.disconnect()

    async def disconnect(self):
        await self.client.disconnect()


def scan_device():
    async def scan():
        devices = await BleakScanner.discover()
        for d in devices:
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
        format="{asctime} {levelname} - {message}",
        style="{",
        datefmt="%m-%d %H:%M:%S",
        level=logging.INFO,
    )
    # scan_device()
    main()
