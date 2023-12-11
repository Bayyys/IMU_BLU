import {
	defineStore
} from 'pinia';

export const useDeviceStore = defineStore('device', {
	state: () => ({
		device: {},
		services: [],
	}),
	getters: {
		deviceName: (state) => state.device.name,
	},
	actions: {
		mDevice(item) {
			this.device = item
		},
		setServices(servicesList) {
			this.services = servicesList;
		} 
	},
});