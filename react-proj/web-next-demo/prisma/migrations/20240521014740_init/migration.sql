/*
  Warnings:

  - You are about to drop the column `created_at` on the `group` table. All the data in the column will be lost.
  - You are about to drop the column `email` on the `group` table. All the data in the column will be lost.
  - You are about to drop the column `tel` on the `group` table. All the data in the column will be lost.
  - You are about to drop the column `updated_at` on the `group` table. All the data in the column will be lost.
  - Added the required column `basic` to the `group` table without a default value. This is not possible if the table is not empty.
  - Added the required column `basic_angle` to the `group` table without a default value. This is not possible if the table is not empty.
  - Added the required column `createAt` to the `group` table without a default value. This is not possible if the table is not empty.
  - Added the required column `surgery` to the `group` table without a default value. This is not possible if the table is not empty.
  - Added the required column `updateAt` to the `group` table without a default value. This is not possible if the table is not empty.

*/
-- RedefineTables
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_group" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "sex" BOOLEAN DEFAULT true,
    "age" INTEGER NOT NULL,
    "oppt" TEXT,
    "basic_angle" INTEGER NOT NULL,
    "basic" TEXT NOT NULL,
    "surgery" BOOLEAN NOT NULL,
    "sur_type" TEXT,
    "sur_interval" INTEGER,
    "desc" TEXT,
    "createAt" DATETIME NOT NULL,
    "updateAt" DATETIME NOT NULL
);
INSERT INTO "new_group" ("age", "id", "name", "sex") SELECT "age", "id", "name", "sex" FROM "group";
DROP TABLE "group";
ALTER TABLE "new_group" RENAME TO "group";
PRAGMA foreign_key_check("group");
PRAGMA foreign_keys=ON;
