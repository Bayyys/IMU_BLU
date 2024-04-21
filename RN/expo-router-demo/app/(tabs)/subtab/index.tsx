import { View } from "@/components/Themed";
import { router } from "expo-router";
import React from "react";
import { Button, Text, StyleSheet } from "react-native";

export default function Three() {
  return (
    <View style={styles.container}>
      <Text style={styles.title}>页面3</Text>
      <View
        style={styles.separator}
        lightColor="#eee"
        darkColor="rgba(255,255,255,0.1)"
      />
      <Button title="跳转到页面1" onPress={() => router.replace("/(tabs)/")} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: "center",
    justifyContent: "center",
  },
  title: {
    fontSize: 20,
    fontWeight: "bold",
  },
  separator: {
    marginVertical: 30,
    height: 1,
    width: "80%",
  },
});
