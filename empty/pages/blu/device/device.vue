<template>
	<view>
		<uni-row>
			<uni-col :span="18">
				<uni-section class="mb-10" :title="device.name" :sub-title="device.deviceId" type="line" title-font-size="20px" sub-title-font-size="10px"></uni-section>
			</uni-col>
			<uni-col :span="6" style="columns: ">
				连接状态
				<uni-icons type="smallcircle-filled" :color="icon_color"></uni-icons>
			</uni-col>
		</uni-row>
	</view>
	<view>
		<uni-row style="padding: 5px">
			<uni-col :span="12">
				<button @click="connectDevice">连接</button>
			</uni-col>
			<uni-col :span="12">
				<button @click="disconnDevice">断开</button>
			</uni-col>
		</uni-row>
	</view>
	<!-- 	<view>
		<textarea v-model="text"></textarea>
	</view> -->
	<!-- <button @click="startLine">click</button> -->
	<view><LEchart ref="chart" @finished="init"></LEchart></view>
</template>

<script setup>
import * as echarts from 'echarts';
import LEchart from '@/uni_modules/lime-echart/components/l-echart/l-echart.vue';
import { useDeviceStore } from '@/stores/device.js';
import { onMounted, onUnmounted, ref, reactive } from 'vue';
const chart = ref()
const deviceStore = useDeviceStore();
const device = deviceStore.device;
const icon_color = ref('red');
const text = ref('hello');
const LEN = 100;
const timer = null;
const state = reactive({ options: {} });
state.options = {
	xAxis: [
		{
			type: 'category',
			boundaryGap: true,
			scale: true,
			data: (function () {
				var now = new Date();
				var res = [];
				var len = LEN;
				while (len--) {
					res.unshift(now.toLocaleTimeString().replace(/^\D*/, ''));
					now = new Date(now - 2000);
				}
				return res;
			})(),
		}
	],
	yAxis: [
		{
			type: 'value',
			scale: true,
			name: '角度(°)',
			min: -180,
			max: 180,
		}
	],
	legend: {
		data: ['X', 'Y', 'Z']
	},
	series: [
		{
			name: 'X',
			type: 'line',
			xAxisIndex: 0,
			yAxisIndex: 0,
			data: (function () {
				var res = [];
				var len = LEN;
				while (len--) {
					res.push(0);
				}
				return res;
			})()
		},
		{
			name: 'Y',
			type: 'line',
			xAxisIndex: 0,
			yAxisIndex: 0,
			data: (function () {
				var res = [];
				var len = LEN;
				while (len--) {
					res.push(0);
				}
				return res;
			})()
		},
		{
			name: 'Z',
			type: 'line',
			xAxisIndex: 0,
			yAxisIndex: 0,
			data: (function () {
				var res = [];
				var len = LEN;
				while (len--) {
					res.push(0);
				}
				return res;
			})()
		}
	]
};

// 连接设备
const connectDevice = () => {
	uni.showLoading({
		title: '连接中...'
	})
	uni.createBLEConnection({
		deviceId: device.deviceId,
		success: (res) => {
			if (res.code) {
				uni.showToast({
					icon: 'error',
					title: '连接失败'
				});
				return;
			}
			icon_color.value = 'green';
			console.log('createBLEConnection', res);
			setTimeout(() => {
				startNotify();
				uni.hideLoading();
			}, 3000);
		},
		fail(res) {
			console.log(res);
			uni.showToast({
				icon: 'error',
				title: '连接失败'
			});
		}
	});
};

// 开始监听
const startNotify = () => {
	uni.notifyBLECharacteristicValueChange({
		deviceId: device.deviceId,
		serviceId: deviceStore.UUID_SERVICE,
		characteristicId: deviceStore.UUID_CHAR_NOTIFY,
		state: true,
		success() {
			uni.showToast({
				icon: 'success',
				title: '设备监听成功'
			});
		},
		fail(res) {
			console.log('notifyBLECharacteristicValueChange', res);
		}
	});
	console.log('开始监听');
};

// 读取信息
const startRead = () => {
	uni.readBLECharacteristicValue({
		deviceId: device.deviceId,
		serviceId: deviceStore.UUID_SERVICE,
		characteristicId: deviceStore.UUID_CHAR_NOTIFY,
		success(res) {
			console.log('startRead', res.errCode);
		}
	});
};

// 断开设备
const disconnDevice = () => {
	uni.closeBLEConnection({
		deviceId: device.deviceId,
		success: (res) => {
			icon_color.value = 'red';
			console.log(res);
		}
	});
};

// 展示服务(Services)
const showServices = () => {
	uni.getBLEDeviceServices({
		deviceId: device.deviceId,
		success(res) {
			deviceStore.setServices(res.services);
			console.log(res.services);
			console.log('device getBLEDeviceServices: ', deviceStore.services);
		}
	});
};

// 展示特征值(Characteristic)
const showCharacteristics = () => {
	deviceStore.services.forEach((service) => {
		uni.getBLEDeviceCharacteristics({
			deviceId: device.deviceId,
			serviceId: service.uuid,
			success(res) {
				service.characteristics = res.characteristics;
				// console.log('device getBLEDeviceCharacteristics: ', res.characteristics);
			}
		});
	});
	console.log(deviceStore.services);
};

// 蓝牙状态改变
uni.onBLEConnectionStateChange((res) => {
	console.log(`设备 ${res.deviceId} 状态改变, ${res.connected ? '连接' : '断开'}`);
	if (!res.connected) {
		disconnDevice();
	}
});

// ArrayBuffer转16进度字符串示例
const ab2hex = (buffer) => {
	var dataView = new DataView(buffer);
	var roll = ((dataView.getUint8(3) << 8) | ((dataView.getUint8(2) / 32768) * 180)) % 180;
	var pitch = ((dataView.getUint8(5) << 8) | ((dataView.getUint8(4) / 32768) * 180)) % 180;
	var yaw = ((dataView.getUint8(7) << 8) | ((dataView.getUint8(6) / 32768) * 180)) % 180;
	console.log(roll, pitch, yaw);
	

	// const hexArr = Array.prototype.map.call(new Uint8Array(buffer), function (bit) {
	// 	return ('00' + bit.toString(16)).slice(-2);
	// });
	// // text.value += hexArr.join('');
	var axisData = new Date().toLocaleTimeString().replace(/^\D*/, '');

	var data0 = state.options.series[0].data;
	var data1 = state.options.series[1].data;
	var data2 = state.options.series[2].data;
	data0.shift();
	data0.push(roll);
	data1.shift();
	data1.push(pitch);
	data2.shift();
	data2.push(yaw);

	state.options.xAxis[0].data.shift();
	state.options.xAxis[0].data.push(axisData);

	chart.value.setOption(state.options);
};

// 开始监听
uni.onBLECharacteristicValueChange((res) => {
	ab2hex(res.value)
	// console.log(ab2hex(res.value));
	// console.log(res.value);
	// console.log(ab2hex(res.value));
});

onUnmounted(() => {
	disconnDevice();
	uni.stopBluetoothDevicesDiscovery({});
	uni.showToast({
		icon: 'success',
		title: '关闭蓝牙连接'
	});
});


const init = async () => {
	const c = await chart.value.init(echarts);
	c.setOption(state.options);
};
</script>

<style lang="scss"></style>
