import can
import isotp
import struct
import time

# Configure the CAN bus interface
CAN_INTERFACE = "can0"  # Change to your actual interface
UDS_REQ_ID = 0x7E0  # ECU request ID for Toyota
UDS_RESP_ID = 0x7E8  # ECU response ID for Toyota

# Set up CAN bus
bus = can.interface.Bus(channel=CAN_INTERFACE, bustype='socketcan')

# Set up ISO-TP (ISO 15765-4) for UDS
stack = isotp.CanStack(bus, 
                       txid=UDS_REQ_ID, 
                       rxid=UDS_RESP_ID, 
                       params={'tx_padding': 0x00})

def send_uds_request(service, pid):
    """Send a UDS request and return the response"""
    request = bytes([service, pid])
    stack.send(request)

    # Wait for response
    timeout = 2
    start_time = time.time()
    while time.time() - start_time < timeout:
        stack.process()
        response = stack.recv()
        if response:
            return response
    return None

def decode_speed(response):
    """Decode vehicle speed from response"""
    if response and len(response) > 1:
        return response[1]  # Speed in km/h
    return None

def decode_accelerometer(response):
    """Decode accelerometer data from response"""
    if response and len(response) >= 3:
        x = struct.unpack('b', response[1:2])[0]  # X-axis acceleration
        y = struct.unpack('b', response[2:3])[0]  # Y-axis acceleration
        return x, y
    return None

def decode_mileage(response):
    """Decode mileage (Odometer) from response"""
    if response and len(response) >= 4:
        return struct.unpack('>I', response[1:4] + b'\x00')[0]  # Odometer in km
    return None

def decode_dtc(response):
    """Decode Diagnostic Trouble Codes (DTCs) from response"""
    dtcs = []
    if response and len(response) >= 3:
        num_dtcs = (len(response) - 1) // 2
        for i in range(num_dtcs):
            dtc_bytes = response[1 + i*2 : 3 + i*2]
            dtc = struct.unpack('>H', dtc_bytes)[0]
            dtcs.append(f"DTC {dtc:X}")
    return dtcs if dtcs else ["No DTCs found"]

# Store previous values to detect changes
prev_speed = None
prev_accel = None
prev_mileage = None
prev_dtcs = None

print("Starting vehicle monitoring... Press Ctrl+C to exit.")

try:
    while True:
        speed_resp = send_uds_request(0x22, 0xF40D)  # Vehicle Speed
        accel_resp = send_uds_request(0x22, 0xF440)  # Accelerometer Data
        mileage_resp = send_uds_request(0x22, 0xF431)  # Mileage (Odometer)
        dtc_resp = send_uds_request(0x19, 0x02)  # Read stored DTCs

        # Decode values
        speed = decode_speed(speed_resp)
        accel = decode_accelerometer(accel_resp)
        mileage = decode_mileage(mileage_resp)
        dtcs = decode_dtc(dtc_resp)

        # Print only if values change
        if speed != prev_speed:
            print(f"Speed: {speed} km/h")
            prev_speed = speed

        if accel != prev_accel:
            print(f"Accelerometer: {accel}")
            prev_accel = accel

        if mileage != prev_mileage:
            print(f"Mileage: {mileage} km")
            prev_mileage = mileage

        if dtcs != prev_dtcs:
            print(f"Diagnostic Trouble Codes: {dtcs}")
            prev_dtcs = dtcs

        time.sleep(1)  # Adjust as needed

except KeyboardInterrupt:
    print("\nMonitoring stopped.")
