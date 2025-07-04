## What is a Rollback / Rolling-Pwn Attack

A Rollback / Rolling-Pwn attack is not really a new replay attack against remote keyless entry systems and key fobs but a new term for time-agnostic replay attacks despite having rolling codes. Some people have already disclosed this in the past, but researchers Levente Csikor and others published a better PoC about it and its prevalence in newer vehicles. A rolling code system in keyless entry systems is one of the solutions to prevent replay attacks. However, some vehicle receivers will accept a sliding window of code to avoid accidental key pressed by design. By sending the valid captured signals in a consecutive sequence to some vehicles, it will resynchronized the counter. Once the counter is resynced, commands from the previous cycle of the counter should work again. Therefore, these valid captured signals from the key fob can be used to unlock the car indefinitely.

## List of Vehicles in the PH Car Hacking Scene Affected by Rollback / Rolling-Pwn Attack

1. [2017 Toyota Rush](https://youtube.com/shorts/lYTrSDCGbJs?feature=share) (Reported to Toyota via Hackerone and was acknowledged)
2. [2017 Mazda 2 Sedan](https://youtube.com/shorts/apWLPRXZiHM?feature=share) (FCC ID: SKE13E-01)
3. [2017 Honda Brio V](https://www.youtube.com/shorts/r1GabTsF6UI) (Manufacturer Part Number:	5WK49327)
4. Aftermarket Code Alarm CAT4P (FCC ID: H5OT66)
5. [2017 Toyota Wigo S](https://www.youtube.com/shorts/vL3yjYKuOoU) - It has been observed that this was not reproducible in Flipper Zero but only in HackRF One. (Reported to Toyota via Hackerone and was acknowledged)
6. 2010 Nissan Navara (FCC ID: KR55WY8462 | Fitments: 2007-2012)
7. [2017 Honda Mobilio RS NAVI](https://youtube.com/shorts/u10esbLgJkk) (Tested by Aj Dumanhug)
8. 2023 Toyota Rush (Reported to Toyota via Hackerone and was acknowledged)
9. [2022 Honda Civic](https://www.facebook.com/chvdotph/videos/480673784353597)
10. [2018 - 2020 Honda ADV](https://www.facebook.com/chvdotph/videos/1348622009471804) (Motorcycle)
11. [Honda Click 2019](https://www.facebook.com/chvdotph/videos/1399957767825557) (Motorcycle)

Facebook reels and videos: https://www.facebook.com/chvdotph/videos

## List of Vehicles affected by a simple Replay Attack
1. 2014 Ford EcoSport (Ford is aware of this issue when we reported this to Hackerone)
2. [Cobra Car Alarm 1000-PT-2 2022 Model (UNIVERSAL)](https://www.lazada.com.ph/products/cobra-car-alarm-1000-pt-2-2022-model-universal-i2477259510-s11351369524.html?spm=a2o4l.tm80167379.5934338580.1.69d2rzqHrzqHaw.69d2rzqHrzqHaw&priceCompare=skuId%3A11351369524%3Bsource%3Alazada-om%3Bsn%3Aea77dd71-2e4f-4124-839b-1150539b843b%3BoriginPrice%3A124900%3BvoucherPrice%3A124900%3Btimestamp%3A1676085341414)
3. [Aventail Basic Car Alarm System AV19](https://www.carsthetics.com/collections/security/products/aventail-basic-car-alarm-system-av19)
4. [2010 Suzuki Swift](https://www.facebook.com/100000559941479/videos/296237223373961/) (discovered by Henry of Cellular Assault Village)

The Vehicles enumerated were tested by the Car Hacking Village PH Team and some contributors.

## Vehicles Fully Tested but not Vulnerable
1. 2020 Toyota Hilux Conquest (FCC ID: BM1EW)
2. 2019 Mitsubishi Montero GLS (FCC ID: GHR-M004)
3. 2006 Toyota Fortuner (FCC ID: B42TA)
4. 2017 Ford Everest 
5. 2019 Toyota Hilux (FCC ID: BA2TA)
6. 2020 Nissan X-Trail 4x4 (FCC ID: S180144102)
7. 2017 Toyota Wigo (Tested using Flipper Zero at one time but not with HackRF)
8. 2021 Geely Emgrand
9. 2021 Geely Coolray
10. 2021 Geely Okavango
11. 2017 Toyota Fortuner (FCC ID: BM1EW)
12. 2019 Toyota Land Cruiser VX (FCC ID: BJ2EW)
13. 2019 Honda Civic 
14. 2017 Ford Ranger Wildtrak 3.2
15. 2023 Suzuki Jimny 
16. 2015 Honda Jazz I-VTEC
17. 2019 Mitsubishi Xpander
18. 2007 Toyota Camry
19. 2004 Nissan X-Trail (CMII ID 2005DJ2120) - Manufactured by Korea Siemens Automotive Systems Corporation
20. 2023 Volkswagen T-Cross (FCC: FS1744M) - Manufactured by  HELLA Shanghai Electronics Co. Ltd


## References
- https://rollingpwn.github.io/rolling-pwn/
- https://medium.com/codex/rollback-important-details-about-the-new-keyfob-vulnerability-86ea5727f3d3
- https://www.pcmag.com/news/is-your-car-key-fob-vulnerable-to-this-simple-replay-attack
- https://docs.google.com/forms/d/e/1FAIpQLSfaz6l-V6MHUk0ze9vTuM-xEwjwivsmT_Ul8otEMTrwlOlHig/viewform
- https://www.facebook.com/chvdotph/videos

## Credits

The persons below were instrumental in letting us test their vehicles and hw.

- Jayson Vallente
- Bro. Sterling Rivera
- semprix
- mogul
- John Ray
- PJ (Kapag Mabango Kainin Mo)
- Napz
- Shawn
- Bro. Allan Castro
- Bro. Vovoi Trangia
- Bro. Peo Magaling
- Cinco Auto Clinic
- Nowel del Mundo
- Aj Dumanhug
- Henry Caga

Want to collaborate in this research? Hit us up as we have the right hw :)
