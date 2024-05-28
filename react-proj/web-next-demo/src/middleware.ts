// export { auth as middleware } from "@/auth";
// import NextAuth from "next-auth";
// import { authConfig } from "./auth.config";

import { NextRequest, NextResponse } from "next/server";

// export default NextAuth(authConfig).auth;

export function middleware(req: NextRequest) {
  const pathname = req.nextUrl.pathname;
  console.log("🚀 ~ middleware ~ pathname:", pathname);
  if (pathname !== "/") {
    return NextResponse.redirect(new URL("/", req.url));
  }
  // 判断是否访问的是根路径
  // if (req.url === "/") {
  //   console.log(req.url);
  //   return NextResponse.redirect(new URL("/", req.url));
  // }
}

export const config = {
  // https://nextjs.org/docs/app/building-your-application/routing/middleware#matcher
  matcher: ["/((?!api|_next/static|_next/image|.*\\.png$).*)"],
};
