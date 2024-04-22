import FontAwesome from "@expo/vector-icons/FontAwesome";
import React from "react";

import { View } from "@/components/Themed";
import { Stack } from "expo-router";
import { Tabs } from "expo-router";

// You can explore the built-in icon families and icons on the web at https://icons.expo.fyi/
function TabBarIcon(props: {
  name: React.ComponentProps<typeof FontAwesome>["name"];
  color: string;
}) {
  return <FontAwesome size={28} style={{ marginBottom: -3 }} {...props} />;
}

export default function Subtab() {
  return (
    <Tabs
      screenOptions={{
        headerShown: false,
      }}
    >
      <Tabs.Screen
        name="subtab1"
        options={{
          title: "Subtab 1",
        }}
      />
      <Tabs.Screen
        name="subtab2"
        options={{
          title: "Subtab 2",
        }}
      />
    </Tabs>
  );
}
