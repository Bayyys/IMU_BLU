import { StatusBar } from "expo-status-bar";
import { Button, Platform, StyleSheet } from "react-native";

import { Text, View } from "@/components/Themed";
import { router } from "expo-router";

export default function ModalScreen() {
  return (
    <View style={styles.container}>
      <Button
        title="跳转到首页"
        onPress={() => {
          router.replace("/");
        }}
      ></Button>

      {/* Use a light status bar on iOS to account for the black space above the modal
      <StatusBar style={Platform.OS === "ios" ? "light" : "auto"} /> */}
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
