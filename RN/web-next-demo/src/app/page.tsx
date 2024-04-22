"use client";
import { Button } from "antd";
import { on } from "events";
import { use, useCallback, useEffect, useRef, useState } from "react";

export default function Home() {
  // 定义相关数据
  const flag = useRef(true); // 是否第一次加载
  const [isSupport, setIsSupport] = useState(false); // 是否支持串口
  const isConnected = useRef(false); // 是否连接
  let port: any;
  let reader: any;
  let writer: any;
  let keepReading = true;
  let data_rest: any;

  // 添加串口的丽娜姐监听
  const addSerialListener = useCallback(() => {
    navigator.serial.addEventListener("connect", (e) => {
      console.log("连接设备", e);
    });
    navigator.serial.addEventListener("disconnect", (e) => {
      console.log("断开设备", e);
    });
  }, []);

  useEffect(() => {
    // 判断是否支持串口
    if (flag.current) {
      const isTemp = "serial" in navigator;
      console.log(`当前设备${isTemp ? "支持" : "不支持"}串口通信`);
      setIsSupport(isTemp);
      flag.current = false;
      addSerialListener();
    }
  }, [addSerialListener]);

  // buffer转16进制数组
  const buf2hex = (buffer: any) => {
    return Array.prototype.map
      .call(new Uint8Array(buffer), (x) => ("00" + x.toString(16)).slice(-2))
      .join(" ");
  };

  // buffer转字符串
  const buf2str = (buffer: any) => {
    return new TextDecoder().decode(buffer);
  };

  const readData = async () => {
    // 连接设备
    isConnected.current = true;
    try {
      await port.open({ baudRate: 115200 });
      reader = port.readable.getReader();
      writer = port.writable.getWriter();
    } catch (error) {
      console.log("连接失败", error);
    }

    while (port.readable && isConnected.current) {
      try {
        while (isConnected.current) {
          const { value, done } = await reader.read();
          if (done) {
            break;
          }
          if (value) {
            let data = Uint16Array.from(value);
            data = data_rest ? new Uint16Array([...data_rest, ...data]) : data;
            const idx = data.findIndex((item) => item === 0x55);
            if (idx !== -1) {
              // 找到包头
              data = data.slice(idx); // 截取包头之后的数据
              if (data.length >= 6 && data[1] === 0x62) {
                // 传感器连接包
                data = data.slice(0, 6); // 截取一个完整的数据包
                const connect_flag = [data[2], data[3], data[4], data[5]];
                for (let i = 0; i < connect_flag.length; i++) {
                  console.log(
                    `传感器${i + 1}连接状态：`,
                    connect_flag[i] === 0x01 ? "连接成功" : "连接失败"
                  );
                }
              } else if (data.length >= 26 && data[1] === 0x61) {
                // 传感器数据包
                data_rest = data.slice(26); // 保存剩余的数据
                data = data.slice(0, 26); // 截取一个完整的数据包
                // 分为四个传感器, 每个传感器roll, pitch, yaw
                // rollL, rollH
                // roll = (rollH << 8 | rollL) / 32768 * 180 (°)
                for (let i = 0; i < 4; i++) {
                  const roll =
                    (((data[2 + i * 6 + 1] << 8) | data[2 + i * 6]) / 32768) *
                    180;
                  const pitch =
                    (((data[2 + i * 6 + 3] << 8) | data[2 + i * 6 + 2]) /
                      32768) *
                    180;
                  const yaw =
                    (((data[2 + i * 6 + 5] << 8) | data[2 + i * 6 + 4]) /
                      32768) *
                    180;
                  console.log(
                    `传感器${
                      i + 1
                    }：roll: ${roll}, pitch: ${pitch}, yaw: ${yaw}`
                  );
                }
              } else {
                data_rest = data;
                continue;
              }
            }
          }
        }
      } catch (error) {
        console.log(error);
      } finally {
        reader.releaseLock();
        writer.releaseLock();
        console.log("finally", port.readable, keepReading);
      }
    }

    if (port) {
      try {
        await port.close();
        console.log("closed");
      } catch (error) {
        console.log(error);
      }
    }
  };

  // 连接设备
  const onConnect = async () => {
    if (!isSupport) {
      console.log("当前设备不支持串口通信");
      return;
    }
    if (isConnected.current) {
      console.log("当前设备已连接");
      return;
    }
    // 用户选择串口
    port = await navigator.serial.requestPort();
    keepReading = true;

    // 读取数据
    await readData();
  };

  // 断开连接
  const onDisconnect = async () => {
    if (isConnected.current) {
      isConnected.current = false;
      if (reader) {
        await reader.cancel();
      }
      if (port) {
        try {
          await port.close();
        } catch (error) {
          console.log(error);
        }
      }
    }
  };

  return (
    <main className="container">
      <div className="App">
        <Button type="primary" onClick={onConnect}>
          连接
        </Button>
        <Button type="primary" onClick={onDisconnect}>
          断开
        </Button>
      </div>
    </main>
  );
}
