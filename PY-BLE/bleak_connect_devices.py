import asyncio
import contextlib
import logging
from typing import Iterable

from bleak import BleakScanner, BleakClient


async def connect_to_device(
    lock: asyncio.Lock,
    name_or_address: str,
    notify_uuid: str,
):
    """
    Scan and connect to a device then print notifications for 10 seconds before
    disconnecting.

    Args:
        lock:
            The same lock must be passed to all calls to this function.
        name_or_address:
            The Bluetooth address/UUID of the device to connect to.
        notify_uuid:
            The UUID of a characteristic that supports notifications.
    """
    logging.info("starting %s task", name_or_address)

    try:
        async with contextlib.AsyncExitStack() as stack:
            async with lock:
                device = None
                con_flag = 0
                while device is None and con_flag < 3:
                    logging.info("scanning for %s", name_or_address)
                    device = await BleakScanner.find_device_by_address(name_or_address)

                    logging.info("stopped scanning for %s", name_or_address)

                    if device is None:
                        logging.info("%s rescan...", name_or_address)

                if device is None and con_flag >= 3:
                    logging.error("Failed to connect to %s", name_or_address)
                    return

                client = BleakClient(device)

                logging.info("connecting to %s", name_or_address)

                await stack.enter_async_context(client)

                logging.info("connected to %s", name_or_address)

                stack.callback(logging.info, "disconnecting from %s", name_or_address)

            def callback(_, data):
                logging.info("%s received %r", name_or_address, data)

            await client.start_notify(notify_uuid, callback)
            await asyncio.sleep(200.0)
            await client.stop_notify(notify_uuid)

        logging.info("disconnected from %s", name_or_address)

    except Exception:
        logging.exception("error with %s", name_or_address)


async def main(
    addresses: Iterable[str],
    uuids: Iterable[str],
):
    lock = asyncio.Lock()

    await asyncio.gather(
        *(
            connect_to_device(lock, address, uuid)
            for address, uuid in zip(addresses, uuids)
        )
    )


if __name__ == "__main__":
    device_mac_address_list = ["FF:E6:12:3B:08:A6", "D9:69:71:72:2D:1E"]
    device_char_uuid_list = [
        "0000FFE4-0000-1000-8000-00805F9A34FB",
        "0000FFE4-0000-1000-8000-00805F9A34FB",
    ]

    # 设置log级别
    logging.basicConfig(
        # filename="test.log",
        format="{asctime} {levelname} - {message}",
        style="{",
        datefmt="%m-%d %H:%M:%S",
        level=logging.INFO,
    )

    fhandler = logging.FileHandler(filename="test.log", mode="w")
    logging.getLogger().addHandler(fhandler)
    asyncio.run(
        main(
            device_mac_address_list,
            device_char_uuid_list,
        )
    )
