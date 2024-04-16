import time
import sys
import PyQt6.QtBluetooth as BLE
import PyQt6.QtCore as QtCore


class Client(QtCore.QCoreApplication):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.agent = BLE.QBluetoothDeviceDiscoveryAgent()
        self.agent.setLowEnergyDiscoveryTimeout(5000)

        self.agent.deviceDiscovered.connect(self.discovery_device_discovered)
        self.agent.errorOccurred.connect(self.discovery_error)
        self.agent.finished.connect(self.discovery_finished)

        self.agent.start(self.agent.DiscoveryMethod.LowEnergyMethod)

        self.timer = QtCore.QTimer(self.agent)
        self.timer.start(10000)
        self.timer.timeout.connect(self.display_status)

        self.controller = None
        self.serviceUUID: list[BLE.QBluetoothUuid] = list()
        self.SERVICE_UUID = BLE.QBluetoothUuid("{0000ffe5-0000-1000-8000-00805f9a34fb}")

    def discovery_device_discovered(self, info: BLE.QBluetoothDeviceInfo):
        if info.name().find("WT") != -1:
            print(f"Device discovered: ", end=" ")
            print(f"Name: {info.name()}", end=" ")
            print(f"Address: {info.address().toString()}", end=" ")
            print(f"Service UUIDs: {info.serviceUuids()}")

    def discovery_error(self, error: BLE.QBluetoothDeviceDiscoveryAgent.Error):
        print(f"Error: {error}")

    def display_status(self):
        self.timer.stop()
        # self.agent.deleteLater()
        # self.discovery_finished()

    def discovery_finished(self):
        for dev in self.agent.discoveredDevices():
            if dev.name().find("WT") != -1:
                print(f"连接设备: {dev.name()}, Address: {dev.address().toString()}")
                print(f"CoreConfig: {dev.coreConfigurations()}")
                self.controller = BLE.QLowEnergyController.createCentral(
                    dev
                )  # 创建连接
                self.controller.connected.connect(self.controller_connected)
                self.controller.errorOccurred.connect(self.controller_error)
                self.controller.disconnected.connect(self.controller_disconnected)
                self.controller.serviceDiscovered.connect(
                    self.controller_service_discovered
                )
                self.controller.discoveryFinished.connect(
                    self.controller_discovery_finished
                )
                self.controller.connectToDevice()
                self.agent.stop()
                break

    def controller_connected(self):
        print("Connected to device")
        self.controller.discoverServices()

    def controller_error(self, error: BLE.QLowEnergyController.Error):
        print(f"Controller Error: {error}")

    def controller_disconnected(self):
        print("Disconnected from device")
        self.controller.deleteLater()
        self.controller = None

    def controller_service_discovered(self, uuid: BLE.QBluetoothUuid):
        print(f"Service discovered: {uuid}")
        self.serviceUUID.append(uuid)

    def controller_discovery_finished(self):
        # self.controller.disconnectFromDevice()  # 断开连接
        print(f"Discovery finished")
        for uuid in self.serviceUUID:
            print(f"Service UUID: {uuid}, {self.SERVICE_UUID}")
            if uuid == self.SERVICE_UUID:
                self.serviceObj = self.controller.createServiceObject(uuid)
                self.serviceObj.stateChanged.connect(self.service_state_changed)
                self.serviceObj.errorOccurred.connect(self.service_error)
                self.serviceObj.discoverDetails()

    def service_state_changed(self, state: BLE.QLowEnergyService.ServiceState):
        print(f"服务状态变化通知: {state}")
        if state == BLE.QLowEnergyService.ServiceState.DiscoveringService:
            print(f"正在搜索服务特征...")
            self.serviceObj.discoverDetails()
        elif state == BLE.QLowEnergyService.ServiceState.ServiceDiscovered:
            print(
                f"搜索服务特征完成. 共计{len(self.serviceObj.characteristics())}个特征."
            )
            for ch in self.serviceObj.characteristics():
                print(f"Characteristic: {ch.uuid().toString()}")

    def service_error(self, error: BLE.QLowEnergyService.ServiceError):
        print(f"Service error: {error}")


if __name__ == "__main__":
    client = Client(sys.argv)
    client.exec()
    sys.exit(client.exec())
