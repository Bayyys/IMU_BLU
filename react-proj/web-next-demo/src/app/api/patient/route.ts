// GET 方法: 返回{code: 200, msg: { count: number, users: User[] }}
// 用户信息列表，支持根据id和name查询
// 分页查询

import { PrismaClient } from "@prisma/client";
import { NextRequest, NextResponse } from "next/server";

const prisma = new PrismaClient();

export const GET = async (req: NextRequest) => {
  const params = req.nextUrl.searchParams;
  const id_query = params.get("id");
  const id = parseInt(id_query ?? "0");
  const name_query = params.get("name");
  const name = name_query ?? "";
  const res = await prisma.user.findMany({
    where: {
      id: id ? { equals: id } : undefined,
      name: name ? { equals: name } : undefined,
    },
  });
  const count = await prisma.user.count();

  return NextResponse.json({
    code: 200,
    msg: "success",
    data: {
      count,
      users: res,
    },
  });
};

export const POST = async (req: NextRequest) => {
  const data = await req.json();
  const res = await prisma.user.create({
    data,
  });

  return NextResponse.json({ code: 200, msg: "success", data: res });
};
