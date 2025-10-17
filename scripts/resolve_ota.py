Import("env")
import subprocess, re

def resolve_mdns(hostname):
    """Ping the .local hostname to extract the IP address."""
    print(f"🔍 Resolving {hostname} ...")
    for i in range(2):
        try:
            result = subprocess.check_output(["ping", "-c", "1", hostname], text=True)
            match = re.search(r"\(([\d\.]+)\)", result)
            if match:
                ip = match.group(1)
                print(f"🌐 {hostname} resolved to {ip}")
                return ip
            else:
                print("⚠️ No IP found in ping result")
        except Exception as e:
            print(f"❌ Failed to resolve {hostname}: {e}")
    return None

def before_upload(source, target, env):
    upload_port = env.get("UPLOAD_PORT", "")
    if upload_port.endswith(".local"):
        ip = resolve_mdns(upload_port)
        if ip:
            env.Replace(UPLOAD_PORT=ip)
            print(f"✅ Using resolved IP for OTA: {ip}")
        else:
            print("⚠️ Could not resolve mDNS name, using original value.")

env.AddPreAction("upload", before_upload)
