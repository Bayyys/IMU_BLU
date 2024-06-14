import { NextRequest, NextResponse, ImageResponseOptions } from "next/server";
import { ImageResponse } from "next/og";
import upyun from "upyun";
import fs from "fs";
import { Stream } from "stream";

const service = new upyun.Service(
  "upyun-bayyy",
  "bayyy",
  "Npewi9LW9gGcGF1Zypsy1YKggCdgLY5N"
);
const client = new upyun.Client(service);

// 文件上传至又拍云
export const GET = async (req: Request) => {
  // 创建一个临时的文件写入流
  const tmpStream = fs.createWriteStream("test.jpg");
  const res = await client.getFile("test.jpg", tmpStream);
  const buffer = Buffer.from(fs.readFileSync("test.jpg"));
  const headers = new Headers();
  headers.set("content-Type", "image/jepg"); // 默认动作是浏览器展示
  return new NextResponse(buffer, { status: 200, headers: headers });
};
