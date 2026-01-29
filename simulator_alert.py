import time
import random
import telebot
from datetime import datetime

# ========== CONFIG ==========
BOT_TOKEN = "7783119065:AAGborkBD2CaUiAXqF6djJR9dUBXOLr2e1A"
CHAT_ID = "6343958007"

ALERT_COOLDOWN = 30  # seconds
# ============================

bot = telebot.TeleBot(BOT_TOKEN)
last_alert_time = 0

def simulate_sensor_data():
    """
    Simulate distance (cm) and speed (cm/s)
    Similar to your ESP32 logic
    """
    distance = random.uniform(20, 200)
    speed = random.uniform(0, 30)
    return distance, speed

def send_telegram_alert(distance, speed):
    msg = (
        "🚨 COLLISION RISK DETECTED!\n\n"
        f"Distance: {distance:.2f} cm\n"
        f"Approach Speed: {speed:.2f} cm/s\n"
        f"Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n"
        "Status: Fast approaching object detected."
    )
    bot.send_message(CHAT_ID, msg)
    print("Telegram alert sent!")

def main():
    global last_alert_time

    print("=== Collision Detection Simulator Started ===")
    print("Simulating sensor data...\n")

    while True:
        distance, speed = simulate_sensor_data()

        print("--------------------------------")
        print(f"Sim Distance: {distance:.2f} cm")
        print(f"Sim Speed   : {speed:.2f} cm/s")

        danger = False

        # Same logic as ESP32 (conceptually)
        if speed > 15.0 and distance < 120.0:
            print("⚠️ FAST APPROACH DETECTED (SIMULATED)")
            danger = True

        current_time = time.time()

        if danger and (current_time - last_alert_time > ALERT_COOLDOWN):
            send_telegram_alert(distance, speed)
            last_alert_time = current_time

        time.sleep(3)

if __name__ == "__main__":
    main()
