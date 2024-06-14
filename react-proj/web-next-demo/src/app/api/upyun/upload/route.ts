import { NextRequest, NextResponse } from "next/server";
import upyun from "upyun";
import fs from "fs";

const service = new upyun.Service(
  "upyun-bayyy",
  "bayyy",
  "Npewi9LW9gGcGF1Zypsy1YKggCdgLY5N"
);
const client = new upyun.Client(service);

// 文件上传至又拍云
export const POST = async (req: NextRequest) => {
  const formData = await req.formData();
  const file = formData.get("file") as Blob | null;
  if (!file) {
    return NextResponse.json({
      code: 400,
      msg: "文件不存在",
    });
  }
  const buffer = Buffer.from(await file.arrayBuffer());
  const res = await client.putFile(`demo/${(file as any).name}`, buffer);
  return NextResponse.json({
    code: 200,
    msg: {
      res,
    },
  });
};
