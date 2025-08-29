import can
import isotp
import time

# CAN IDs
TX_ID = 0x7E1  # Tester -> ECU
RX_ID = 0x7E9  # ECU -> Tester

# Set up CAN interface using the updated 'interface' keyword
can_bus = can.interface.Bus(interface='socketcan', channel='can0', bitrate=500000)

# ISOTP Address config
address = isotp.Address(isotp.AddressingMode.Normal_11bits, txid=TX_ID, rxid=RX_ID)
stack = isotp.CanStack(bus=can_bus, address=address)

def send_and_receive(hex_str, timeout=1.0):
    # Convert hex string to bytes
    payload = bytes.fromhex(hex_str)
    stack.send(payload)

    # Wait for response with timeout
    start = time.time()
    while not stack.available():
        stack.process()
        if time.time() - start > timeout:
            raise TimeoutError("Timeout waiting for response")
        time.sleep(0.01)
    stack.process()
    return stack.recv()

def main():
    print("[*] Sending Seed Request (27 01)...")
    response = send_and_receive("27 01")

    if not response or response[0] != 0x67 or response[1] != 0x01:
        print("[!] Unexpected or no response:", response.hex() if response else "None")
        return

    seed = response[2:]
    seed_hex = seed.hex().upper()
    seed_int = int.from_bytes(seed, byteorder='big')
    print(f"[+] Received Seed: 0x{seed_hex}")

    # XOR with a known key
    SECRET = 0x12345678
    key_int = seed_int ^ SECRET
    key_bytes = key_int.to_bytes(4, byteorder='big')
    key_hex = key_bytes.hex().upper()
    print(f"[+] Calculated Key: 0x{key_hex}")

    print("[*] Sending Key (27 02)...")
    key_payload = "27 02 " + ' '.join(f"{b:02X}" for b in key_bytes)
    response = send_and_receive(key_payload)

    if response and response[0] == 0x67 and response[1] == 0x02:
        print("[✅] Security Access Granted!")
    else:
        print("[❌] Security Access Failed! Response:", response.hex() if response else "None")

if __name__ == "__main__":
    try:
        main()
    finally:
        # Clean shutdown of CAN bus
        can_bus.shutdown()
