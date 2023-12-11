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
		<uni-row style="padding: 5px;">
			<uni-col :span="12">
				<button @click="connectDevice">连接</button>
			</uni-col>
			<uni-col :span="12">
				<button @click="disconnDevice">断开</button>
			</uni-col>
		</uni-row>
	</view>
	<view>
		<uni-row style="padding: 5px;">
			<uni-col :span="12">
				<button @click="showServices">服务</button>
			</uni-col>
			<uni-col :span="12">
				<button @click="showCharacteristics">特征值</button>
			</uni-col>
		</uni-row>
	</view>
</template>

<script setup>
import { useDeviceStore } from '@/stores/device.js';
import { onUnmounted, ref } from 'vue';
const deviceStore = useDeviceStore();
const device = deviceStore.device;
const icon_color = ref('red');

const connectDevice = () => {
	uni.createBLEConnection({
		deviceId: device.deviceId,
		success: (res) => {
			icon_color.value = 'green';
			console.log(res);
		},
		fail() {
			uni.showToast({
				icon: 'error',
				title: '连接失败'
			})
		}
	});
};

const disconnDevice = () => {
	uni.closeBLEConnection({
		deviceId: device.deviceId,
		success: (res) => {
			icon_color.value = 'red';
			console.log(res);
		}
	});
};

const showServices = ()=>{
	uni.getBLEDeviceServices({
		deviceId: device.deviceId,
		success(res){
			deviceStore.setServices(res.services);
			console.log(res.services);
			console.log('device getBLEDeviceServices: ', deviceStore.services);
		}
	})
}

const showCharacteristics = ()=>{
	deviceStore.services.forEach((service)=>{
		uni.getBLEDeviceCharacteristics({
			deviceId: device.deviceId,
			serviceId: service.uuid,
			success(res){
				service.characteristics = res.characteristics;
				// console.log('device getBLEDeviceCharacteristics: ', res.characteristics);
			}
		})
	})
	console.log(deviceStore.services);
}

onUnmounted(()=>{
	disconnDevice();
	uni.showToast({
		icon: 'success',
		title: '关闭蓝牙连接',
	})
})
</script>

<style lang="scss"></style>
