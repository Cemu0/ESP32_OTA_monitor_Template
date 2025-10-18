from SCons.Script import Import
import os
import sys
Import("env")
FLAG_FILE = os.path.join(env["PROJECT_DIR"], ".disable_uart")

# def before_serial_upload(source, target, env):
"""Cancel UART upload if the disable flag file exists."""
if os.path.exists(FLAG_FILE):
    print("🚫 UART upload canceled — OTA flag file detected.")
    print(f"🗂️ Flag file: {FLAG_FILE}")
    sys.exit(0)
else:
    print("✅ No flag file found — proceeding with UART upload.")

