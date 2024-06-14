import { NextRequest, NextResponse } from "next/server";
import upyun from "upyun";

const service = new upyun.Service(
  "upyun-bayyy",
  "bayyy",
  "Npewi9LW9gGcGF1Zypsy1YKggCdgLY5N"
);
const client = new upyun.Client(service);

// 文件上传至又拍云
export const GET = async (req: NextRequest) => {
  // 测试: 打印文件内容
  const res = await client.listDir("blog");
  return NextResponse.json({
    code: 200,
    msg: {
      res: res,
    },
  });
};
