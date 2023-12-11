<template>
	<view>
		<h1>Blutooth</h1>
		<button @click="openBLU">打开蓝牙</button>
		<button @click="startSearch">开始搜索</button>
		<button @click="stopSearch">停止搜索</button>
		<button @click="showSearchList">展示搜索设备</button>
		<button @click="showConnectedList">展示连接设备</button>
		<uni-section title="搜索蓝牙设备" type="line">
			<uni-group v-for="item in state.searchList" :key="item.deviceId" :title="item.name" mode="card">
				<view>
					deviceId: {{ item.deviceId }}
					<button @click="connectDevice(item)">连接</button>
				</view>
				<view>RSSI: {{ item.RSSI }}</view>
			</uni-group>
		</uni-section>
	</view>
</template>

<script setup>
import { onMounted, onUnmounted, reactive } from 'vue';
import { useDeviceStore } from '@/stores/device.js';
const deviceStore = useDeviceStore();

const state = reactive({
	searchList: [
		// #ifndef APP
		{
			name: '123',
			deviceId: 123,
			RSSI: '-56',
			advertisServiceUUIDs: []
		},
		{
			name: '456',
			deviceId: 456,
			RSSI: '-56',
			advertisServiceUUIDs: []
		}
		// #endif
	]
});

// 连接设备: 跳转到蓝牙设备主页
const connectDevice = (device) => {
	deviceStore.mDevice(device);
	uni.navigateTo({
		url: '/pages/blu/device/device'
	})
};


// #ifdef APP
// 打开蓝牙
const openBLU = () => {
	uni.openBluetoothAdapter({
		// 初始化蓝牙模块(其他蓝牙相关API必须在此API调用之后再使用)
		success(res) {
			// 获取本机蓝牙适配器状态
			uni.getBluetoothAdapterState({
				//蓝牙的匹配状态
				success: (res_state) => {
					console.log('蓝牙' + (res_state.discovering ? '正在搜索设备' : '未在搜索设备'));
					console.log('蓝牙适配器' + (res_state.available ? '可用' : '不可用'));
				},
				fail(error) {
					uni.showToast({
						icon: 'none',
						title: '查看手机蓝牙是否打开'
					});
				}
			});
		},
		fail(err) {
			console.log('failed', err);
		}
	});
};

// 开始搜索
const startSearch = () => {
	// 开始搜索附近的蓝牙外围设备
	uni.startBluetoothDevicesDiscovery({
		allowDuplicatesKey: false,
		success: (res) => {
			console.log('startBluetoothDevicesDiscovery success', res);
			// 发现外围设备
			uni.onBluetoothDeviceFound((res) => {
				// 监听寻找到新设备的事件
				var devices = res.devices;
				devices.forEach((item) => {
					if (item.name) {
						console.log(item);
						state.searchList = [
							...state.searchList.filter((fItem) => {
								return fItem.deviceId != item.deviceId;
							}),
							item
						];
					}
				});
			});
		},
		fail: (err) => {
			uni.showToast({
				icon: 'none',
				title: '蓝牙适配器初始化失败'
			});
		}
	});
};

// 展示连接设备
const showConnectedList = () => {
	// 获取在蓝牙模块生效期间所有已发现的蓝牙设备
	uni.getConnectedBluetoothDevices({
		success(res) {
			console.log(res.deviceId);
		}
	});
};

// 展示搜索设备(数组信息)
const showSearchList = () => {
	console.log(state.searchList);
};

// 停止搜索
const stopSearch = () => {
	uni.stopBluetoothDevicesDiscovery({
		// 停止搜寻附近的蓝牙外围设备
		success(res) {
			uni.showToast({
				icon: 'none',
				title: '停止搜索附近的蓝牙外围设备'
			});
		}
	});
};

// 监听蓝牙适配器状态变化事件
uni.onBluetoothAdapterStateChange((res) => {
	console.log('蓝牙适配器状态变化, 现在为:' + (res.available ? '开启状态' : '关闭状态') + ', ' + (res.discovering ? '搜索状态' : '停止状态') + '.');
});

onMounted(() => {
	openBLU();
});

onUnmounted(() => {
	// 停止搜寻附近的蓝牙外围设备
	uni.stopBluetoothDevicesDiscovery();
	// 关闭蓝牙模块 (与 openBluetoothAdapter 成对调用)
	uni.closeBluetoothAdapter({
		success() {
			console.log('关闭蓝牙模块成功');
		}
	});
});
// #endif
</script>

<style lang="scss"></style>
