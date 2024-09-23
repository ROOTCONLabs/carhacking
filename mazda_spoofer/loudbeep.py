import can
import random
import time

# Initialize CAN bus (use virtual CAN if you're testing without real hardware)
# Adjust the interface to match your system's setup
bus = can.interface.Bus(channel='can0', bustype='socketcan')

def fuzz_can_id_340():
    # CAN ID to fuzz
    can_id = 0x340

    # Define how many fuzzing iterations you want
    fuzz_iterations = 100

    for _ in range(fuzz_iterations):
        # Generate random 8-byte data
        data = [random.randint(0, 255) for _ in range(8)]

        # Create a CAN message with the fuzzed data
        msg = can.Message(arbitration_id=can_id, data=data, is_extended_id=False)

        try:
            # Send the CAN message
            bus.send(msg)
            print(f"Sent message with ID {hex(can_id)}: Data {data}")

        except can.CanError:
            print("Message transmission failed")

        # Wait a short time between messages to avoid flooding
        time.sleep(1)

if __name__ == "__main__":
    fuzz_can_id_340()
