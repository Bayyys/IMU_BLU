<template>
	<view>
		<button @click="startLine">开始</button>
		<view class="ec">
			<LEchart ref="chart" @finished="init"></LEchart>
		</view>
	</view>
</template>

<script setup>
import * as echarts from 'echarts';
import LEchart from '@/uni_modules/lime-echart/components/l-echart/l-echart.vue';
import { onMounted, onUnmounted, reactive, ref } from 'vue';
let chart = ref(null);
let LEN = 100;
const timer = null;
const state = reactive({ options: {} });
state.options = {
	legend: [
		{
			data: ['X', 'Y', 'Z']
		}
	],
	xAxis: [
		{
			type: 'category',
			boundaryGap: true,
			data: (function () {
				var now = new Date();
				var res = [];
				var len = LEN;
				while (len--) {
					res.unshift(now.toLocaleTimeString().replace(/^\D*/, ''));
					now = new Date(now - 2000);
				}
				return res;
			})()
		}
	],
	yAxis: [
		{
			type: 'value',
			scale: true,
			name: '角度(°)'
		}
	],
	series: [
		{
			name: 'X',
			type: 'line',
			xAxisIndex: 0,
			yAxisIndex: 0,
			data: (() => {
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
			data: (() => {
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
			data: (() => {
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
const init = async () => {
	const c = await chart.value.init(echarts);
	c.setOption(state.options);
};
const startLine = ()=>{
	timer = setInterval(() => {
		var axisData = new Date().toLocaleTimeString().replace(/^\D*/, '');
	
		var data0 = state.options.series[0].data;
		var data1 = state.options.series[1].data;
		var data2 = state.options.series[2].data;
		data0.shift();
		data0.push(Math.round(Math.random() * 1000));
		data1.shift();
		data1.push((Math.random() * 10 + 5).toFixed(1) - 0);
	
		state.options.xAxis[0].data.shift();
		state.options.xAxis[0].data.push(axisData);
	
		chart.value.setOption(state.options);
	}, 10);
}
onUnmounted(() => {
	// 销毁循环定时器
	clearInterval(timer);
});
</script>

<style lang="scss">
	.ec {
		width: 100%;
		height: auto;
	}
</style>
