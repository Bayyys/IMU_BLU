import {
	defineStore
} from 'pinia';

export const useDeviceStore = defineStore('device', {
	state: () => ({
		device: {},
		services: [],
		UUID_CHAR_WRITE: '0000FFF2-0000-1000-8000-00805F9B34FB',
		// UUID_SERVICE: '02F00000-0000-0000-0000-00000000FE00',
		// UUID_CHAR_NOTIFY: '02F00000-0000-0000-0000-00000000FF02',
		UUID_SERVICE: '0000FFF0-0000-1000-8000-00805F9B34FB',
		UUID_CHAR_NOTIFY: '0000FFF1-0000-1000-8000-00805F9B34FB'
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