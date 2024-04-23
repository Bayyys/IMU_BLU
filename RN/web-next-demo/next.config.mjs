/** @type {import('next').NextConfig} */
const nextConfig = {
  images: {
    remotePatterns: [
      {
        protocol: "http",
        hostname: "bitiful.bayyys.cn",
        port: "",
      },
    ],
  },
};

export default nextConfig;
