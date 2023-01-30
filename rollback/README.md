## What is a Rollback Attack / Rolling-Pwn

A Rollback  / Rolling-Pwn attack is not really a new replay attack against remote keyless entry systems and keyfobs but a new term for time-agnostic replay attacks despite having rolling codes. Some people have already disclosed this in the past but researchers Levente Csikor and others published a better PoC about it and its prevalence in newer vehicles. A rolling code system in keyless entry systems is one of the solutions to prevent replay attack. However, some vehicle receivers will accept a sliding window of codes, to avoid accidental key pressed by design and then by sending the valid captyre signals in a consecutive sequence to some vehicles, it will resynchronized the counter. Once counter is resynced, commands from the previous cycle of the counter should work again. Therefore, these valid captured signals from the keyfob can be used to unlock the car indefinitely.

## List of Vehicles in the PH Car Hacking Scene Affected by Rollback / Rolling-Pwn Attack

1. [2017 Toyota Rush](https://youtube.com/shorts/lYTrSDCGbJs?feature=share) (need to get back to Jayson on this)
2. [2017 Mazda 2 Sedan](https://youtube.com/shorts/apWLPRXZiHM?feature=share) (FCC ID: SKE13E-01)

The Vehicles enumerated were tested by the Car Hacking Village PH Team.

## Vehicles Fully Tested but not Vulnerable
1. 2020 Toyota Hilux Conquest (FCC ID: BM1EW)
2. 2019 Mitsubishi Montero GLS (FCC ID: GHR-M004)

## References
- https://rollingpwn.github.io/rolling-pwn/
- https://medium.com/codex/rollback-important-details-about-the-new-keyfob-vulnerability-86ea5727f3d3
- https://www.pcmag.com/news/is-your-car-key-fob-vulnerable-to-this-simple-replay-attack
- https://docs.google.com/forms/d/e/1FAIpQLSfaz6l-V6MHUk0ze9vTuM-xEwjwivsmT_Ul8otEMTrwlOlHig/viewform

## Credits

The persons below were instrumental in letting us test their vehicles and hw.

- Jayson Vallente
- Bro. Sterling Rivera

Want to collaborate in this research? Hit us up as we have the right hw :)
