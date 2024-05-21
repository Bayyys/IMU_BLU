import { Button, StyleSheet } from "react-native";

import { Text, View } from "@/components/Themed";
import { router } from "expo-router";

export default function TabOneScreen() {
  return (
    <View style={styles.container}>
      <Text style={styles.title}>页面1</Text>
      <View
        style={styles.separator}
        lightColor="#eee"
        darkColor="rgba(255,255,255,0.1)"
      />
      <Button
        title="跳转到页面2"
        onPress={() => router.replace("/(tabs)/two")}
      />
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
