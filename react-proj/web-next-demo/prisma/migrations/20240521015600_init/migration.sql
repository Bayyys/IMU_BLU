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
    "createAt" TEXT NOT NULL,
    "updateAt" TEXT NOT NULL
);
INSERT INTO "new_group" ("age", "basic", "basic_angle", "createAt", "desc", "id", "name", "oppt", "sex", "sur_interval", "sur_type", "surgery", "updateAt") SELECT "age", "basic", "basic_angle", "createAt", "desc", "id", "name", "oppt", "sex", "sur_interval", "sur_type", "surgery", "updateAt" FROM "group";
DROP TABLE "group";
ALTER TABLE "new_group" RENAME TO "group";
PRAGMA foreign_key_check("group");
PRAGMA foreign_keys=ON;
