<template>
	<view class="content">
		<image class="logo" src="/static/dice.png"></image>
		<view class="text-area">
			<text class="title">{{ msg }}</text>
			<button @click='jumpEcharts'>Echarts</button>
			<button @click='jumpBLU'>BLU</button>
		</view>
	</view>
</template>

<script setup>
import {ref} from 'vue'
import test1Vue from '../test1/test1.vue';
const msg = 'hello world';
const onload = ()=>{
	console.log("Enter inde.vue");
};
const jumpEcharts = ()=>{
	uni.navigateTo({
		url: '/pages/test1/test1'
	})
}
const jumpBLU =()=>{
	uni.navigateTo({
		url: '/pages/blu/blu'
	})
}
</script>

<style>
.content {
	display: flex;
	flex-direction: column;
	align-items: center;
	justify-content: center;
}

.logo {
	height: 200rpx;
	width: 200rpx;
	margin-top: 200rpx;
	margin-left: auto;
	margin-right: auto;
	margin-bottom: 50rpx;
}

.text-area {
	display: flex;
	justify-content: center;
}

.title {
	font-size: 36rpx;
	color: #8f8f94;
}
</style>
