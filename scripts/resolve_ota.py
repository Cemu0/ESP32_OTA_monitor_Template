Import("env")
import subprocess
import re
import socket
import os
import sys

MAX_ATTEMPT = 2
FLAG_FILE = os.path.join(env["PROJECT_DIR"], ".disable_uart")

def ping_host(host):
    """Ping the given host once, return True if reachable."""
    ping_cmd = ["ping", "-c", "1", "-W", "1", host] if platform.system() != "Windows" else ["ping", "-n", "1", "-w", "1000", host]
    try:
        subprocess.check_output(ping_cmd, stderr=subprocess.STDOUT, text=True)
        print(f"✅ Ping successful: {host}")
        return True
    except subprocess.CalledProcessError:
        print(f"⚠️ Ping failed: {host}")
        return False
    
def resolve_mdns(hostname):
    """Ping the .local hostname to extract its IP address."""
    print(f"🔍 Resolving {hostname} ...")
    for i in range(MAX_ATTEMPT):
        try:
            # Try to resolve via DNS (faster than ping)
            ip = socket.gethostbyname(hostname)
            if re.match(r"^\d+\.\d+\.\d+\.\d+$", ip):
                print(f"🌐 {hostname} resolved to {ip}")
                if ping_host(ip):
                    return ip
        except Exception:
            pass
        try:
            # Fallback: use ping command
            result = subprocess.check_output(["ping", "-c", "1", hostname], text=True)
            match = re.search(r"\(([\d\.]+)\)", result)
            if match:
                ip = match.group(1)
                print(f"🌐 {hostname} resolved to {ip}")
                return ip
        except Exception as e:
            print(f"❌ Attempt {i+1}: {e}")
    # 🧹 Delete the flag file to re-enable UART
    if os.path.exists(FLAG_FILE):
        try:
            os.remove(FLAG_FILE)
            print(f"🗑️ Deleted flag file: {FLAG_FILE}")
        except Exception as e:
            print(f"⚠️ Could not delete flag file: {e}")
    sys.exit(1)
    return None


def before_upload(source, target, env):
    upload_protocol = env.get("UPLOAD_PROTOCOL", "")
    upload_port = env.get("UPLOAD_PORT", "")

    # Only run for espota uploads
    if upload_protocol != "espota":
        return

    if upload_port.endswith(".local"):
        ip = resolve_mdns(upload_port)
        if ip:
            env.Replace(UPLOAD_PORT=ip)
            print(f"✅ Using resolved IP for OTA: {ip}")
            env.Replace(disable_serial_fallback_flag="yes")
        else:
            print("⚠️ Could not resolve mDNS name, falling back to UART upload...")


def after_upload(source, target, env):
    """After a successful OTA upload, set a flag file to disable UART fallback."""
    upload_protocol = env.get("UPLOAD_PROTOCOL", "")
    if upload_protocol == "espota":
        try:
            with open(FLAG_FILE, "w") as f:
                f.write("1\n")
            print(f"🚫 Created flag file to disable UART fallback: {FLAG_FILE}")
        except Exception as e:
            print(f"⚠️ Failed to create flag file: {e}")


env.AddPreAction("upload", before_upload)
env.AddPostAction("upload", after_upload)