# sample: python3 canIDfuzzer.py -i vcan0 -c 0x09A -n 200
# Use vcan0, fuzz CAN ID 0x09A for 200 iterations
import can
import random
import time
import argparse

def fuzz_can(interface, can_id, iterations, delay):
    # Initialize CAN bus
    bus = can.interface.Bus(channel=interface, bustype="socketcan")

    for _ in range(iterations):
        # Random 8-byte payload
        data = [random.randint(0, 255) for _ in range(8)]

        msg = can.Message(
            arbitration_id=can_id,
            data=data,
            is_extended_id=False
        )

        try:
            bus.send(msg)
            print(f"Sent ID {hex(can_id)}: Data {data}")
        except can.CanError:
            print("Message transmission failed")

        time.sleep(delay)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="CAN fuzzing script")
    parser.add_argument("-i", "--interface", default="can0",
                        help="CAN interface to use (default: can0)")
    parser.add_argument("-c", "--canid", type=lambda x: int(x, 0), required=True,
                        help="CAN ID in hex or decimal (e.g., 0x09A or 154)")
    parser.add_argument("-n", "--iterations", type=int, default=100,
                        help="Number of fuzzing iterations (default: 100)")
    parser.add_argument("-d", "--delay", type=float, default=0.1,
                        help="Delay between messages in seconds (default: 0.1)")
    args = parser.parse_args()

    fuzz_can(args.interface, args.canid, args.iterations, args.delay)
