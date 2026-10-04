# Phantom Throttle

**Category:** RAMN
## Solution

### 1. Figure out what's even on the bus

Bench had RAMN connected on one side and my CAN adapter on the other (`can0`). First thing, just watch it:

```bash
> cansniffer -c can0
15|ms | ID  | data ...     < can0 # l=20 h=100 t=500 slots=10 >
00010 | 024 | 00 00 E2 53 3C 4E 6E BF ...S<Nn.
00011 | 039 | 00 00 E2 52 AA 7E 69 C8 ...R.~i.
00011 | 062 | 0B 03 E2 52 F2 47 2E 1D ...R.G..
00099 | 077 | 06 01 30 3A B6 05 A7 1C ..0:....
00099 | 098 | 00 FF 30 3A B0 04 96 86 ..0:....
00100 | 150 | 04 FF 30 3A E7 93 F4 09 ..0:....
00099 | 1A7 | 00 01 30 3A 6A 5A CC 39 ..0:jZ.9
00100 | 1B8 | 01 00 30 3A 38 57 B2 80 ..0:8W..
00099 | 1BB | BA 00 30 3A A3 9E C1 4A ..0:...J
00100 | 1D3 | 00 00 30 3A 5D 30 0E 38 ..0:]0.8
```

`cansniffer` is nice here because it only redraws bytes that actually change, so you can just mash pedals/wheel one at a time and watch which line lights up.

Worked through them one input at a time:

|What I did|ID that moved|
|---|---|
|pressed brake|`0x024`|
|pressed accelerator|`0x039`|
|turned the wheel|`0x062`|
|moved the shifter|`0x077`|

Fully pressing the pedal:

```
00010 | 039 | 0F FF F7 06 E8 29 21 75 .....)!u
```

`0F FF` = 4095 = `0x0FFF`, so that's the top of a 12-bit scale and bytes 0-1 are the pedal position. That accounts for four of the IDs.
### 2. Tried tampering with the accelerator (no luck), let's find the RPM CAN ID instead

Tried tampering the accelerator (`0x039`) directly, forging higher values and holding it at max. No luck, no new CAN IDs showed up on `cansniffer` no matter what we sent.

The RPM ID is defined in the controller script that bridges RAMN to a CARLA simulation:

[`RAMN_Controller_CAN.py`, lines 81–88](https://github.com/ToyotaInfoTech/RAMN/blob/8ea9db078460e506f01e39182420848cc0b4bedb/scripts/carla/RAMN_Controller_CAN.py#L81-L88):

```python
rpm_state = round((100 * scal_vel))
...
canid = 0x43
r = random.randint(0,0xFFFFFFFF)
if USE_BIG_ENDIAN:
    data = [(rpm_state>>8)&0xFF, rpm_state&0xFF,0,0] + [(r >> i & 0xff) for i in (24,16,8,0)]
else:
    data = [rpm_state&0xFF,(rpm_state>>8)&0xFF,0,0] + [(r >> i & 0xff) for i in (24,16,8,0)]
msg = can.Message(arbitration_id=canid, data=data,is_extended_id=False)
self.bus.send(msg)
```

So `0x43` carries `rpm_state`, computed from `scal_vel`, the scaled vehicle velocity CARLA hands back on every tick. And critically: this whole block only runs while the script's `update_output()` loop is actively running against a live CARLA session. **That's why `0x43` was invisible in step 1.** RAMN wasn't connected to CARLA yet, so nothing was ever calling `update_output()`, so `0x43` simply didn't exist on the bus.
### 3. Connect RAMN to CARLA

On the Windows side, from `RAMN\scripts\carla`:

```
RAMN\scripts\carla> .\0_CARLA_SERVER_start.bat
RAMN\scripts\carla> .\3_CARLA_RAMN_auto_serial.bat
```

First script starts the CARLA server itself, second one is what actually runs `RAMN_Controller_CAN.py` and bridges it to RAMN over serial. As soon as that script started its update loop, a whole batch of new IDs showed up on `can0` that weren't there before:

```bash
> cansniffer -c can0
57|ms | ID  | data ...     < can0 # l=20 h=100 t=500 slots=23 >
00010 | 01A | 00 00 03 68 B5 65 00 49 ...h.e.I
00010 | 024 | 00 00 79 2C 40 71 61 ED ..y,@qa.
00010 | 02F | 0B FF 03 68 59 D6 99 20 ...hY.. 
00010 | 039 | 00 00 79 2B E3 E4 05 73 ..y+...s
00010 | 043 | 00 03 03 68 EC DB 46 4B ...h..FK
00010 | 058 | 0B CF 03 68 C9 33 F2 04 ...h.3..
00009 | 062 | 0B 04 79 2B 3E CB 0D A3 ..y+>...
00099 | 06D | 00 00 00 57 4B 1B 4B D4 ...WK.K.
00100 | 077 | 00 01 25 B6 75 73 77 EA ..%.usw.
00099 | 098 | 00 FF 25 B6 AF 2D 2D 55 ..%..--U
00099 | 0A2 | 00 00 00 57 4B 1B 4B D4 ...WK.K.
00099 | 150 | 04 FF 25 B6 F8 BA 4F DA ..%...O.
00099 | 1A7 | 00 00 25 B6 42 19 B5 EB ..%.B...
00099 | 1B8 | 01 00 25 B6 27 7E 09 53 ..%.'~.S
00100 | 1BB | 3A 00 25 B6 87 01 23 74 :.%...#t
00099 | 1C9 | 00 00 00 57 4B 1B 4B D4 ...WK.K.
00099 | 1D3 | 00 00 25 B6 42 19 B5 EB ..%.B...
```

That matches `update_output()` exactly: `0x1A`, `0x2F`, `0x43`, `0x58`, `0x6D`, `0x1C9` all appeared at once, the moment the script's loop started. `0x43` is right there: `00 03 03 68 EC DB 46 4B`. Bytes 0-1 track the sim's speed/RPM readout as the car moves. This is the one that mattered, not a pedal or wheel input, a _computed_ value coming out of the bridge script.
### 4. Forge it

```bash
043  [8]  08 AE 00 00  9C 3D 1F 4A
043  [8]  08 AE 00 00  61 E8 B7 02
043  [8]  08 AF 00 00  D5 40 8A 19
```

First two frames: same RPM value (`08 AE`), completely different last 4 bytes. So that trailer isn't checking anything, it's just filler. Bytes 2-3 being zero here versus the live `03 68` seen in step 3 is just two different moments; nothing to read into it either way. Nothing about this frame looks protected, so the plan was simple: just send one with whatever value I wanted.

The threshold for this challenge was `> 80`, so I picked 90 to clear it with margin rather than sit right on the boundary. I didn't confirm an exact unit conversion against an on-screen speed/RPM readout on this bench, so treat the km/h framing below as a round-number target, not a verified scale. It's using the same order of magnitude as the `round(100 * scal_vel)` formula in the source:

>"90 km/h"-equivalent -> 90 / 3.6 * 100 = 2500 = 0x09C4

```bash
cansend can0 043#09C40000DEADBEEF
```
### 5. Find the reward frames

Had `cansniffer -c can0` open in one terminal before sending the forged frame, specifically so I'd notice if anything new showed up. Right after `cansend`, a line appeared that hadn't been there in any earlier capture:

```
65|ms | ID  | data ...     < can0 # l=20 h=100 t=500 slots=12 >
00009 | 024 | 00 00 65 16 AF F5 1A CD ..e.....
00009 | 039 | 0F FF 65 15 AE 80 E8 B2 ..e.....
00009 | 062 | 0B 02 65 15 7A F7 96 80 ..e.z...
00099 | 077 | 06 01 70 81 B7 20 6D B0 ..p.. m.
00099 | 098 | 00 FF 70 81 B1 21 5C 2A ..p..!\*
00099 | 150 | 04 FF 70 81 E6 B6 3E A5 ..p...>.
00099 | 1A7 | 00 01 70 81 6B 7F 06 95 ..p.k...
00100 | 1B8 | 01 00 70 81 39 72 78 2C ..p.9rx,
00100 | 1BB | 3A 00 70 81 99 0D 52 0B :.p...R.
00099 | 1D3 | 00 00 70 81 5C 15 C4 94 ..p.\...
00024 | 7A0 | 02 59 9C 7B E2 58 A4    .Y.{.X.
```

That's `0x7A0`. Not an ID I knew to look for in advance, it's just what lit up when I diffed the bus before and after. Only one line for it, though, and the payload starts with `02`. `cansniffer` only shows the latest frame per ID, so since this one cycles through index `00`, `01`, `02`, a single snapshot can never show the complete value. We can't get the full picture from `cansniffer` alone, so switched to `candump` filtered to just this ID:

```bash
> candump can0,7A0:7FF
can0  RX - -  7A0   [8]  00 5C CF 6A A2 71 97 43
can0  RX - -  7A0   [8]  01 89 7A E3 5D 95 60 E9
can0  RX - -  7A0   [7]  02 59 9C 7B E2 58 A4
```

Stitched the payloads together in index order:

```
index 0:  5C CF 6A A2 71 97 43
index 1:  89 7A E3 5D 95 60 E9
index 2:  59 9C 7B E2 58 A4
-----------------------------------------------------------
5C CF 6A A2 71 97 43 89 7A E3 5D 95 60 E9 59 9C 7B E2 58 A4   (20 bytes)
```

Not printable ASCII, so it's encoded.
### 6. Break the encoding

Flags here are `chv{...}`, which gives four known plaintext bytes at a known offset, the start of the blob. If it's repeating-key XOR:

```
key = ciphertext XOR known_plaintext

ciphertext  5C   CF   6A   A2
plaintext   63   68   76   7B   ('c' 'h' 'v' '{')
XOR         3F   A7   1C   D9
```

Checked the key length by applying those same 4 bytes to the next 3 ciphertext bytes:

```
ciphertext  71   97   43
key         3F   A7   1C   (wrapped back to key[0])
XOR         4E   30   5F   =  'N' '0' '_'
```

`N0_` falls out clean, so the key is exactly 4 bytes. Ran it over the whole blob:

```python
python3 -c "
ct = bytes.fromhex('5CCF6AA271974389' '7AE35D9560E9599C' '7BE258A4')
key = bytes(ct[i] ^ b'chv{'[i] for i in range(4))
print('key :', key.hex(' '))
print('flag:', bytes(c ^ key[i%4] for i,c in enumerate(ct)).decode())"
```

```
key : 3f a7 1c d9
flag: chv{N0_PEDAL_NEEDED}
```