# disable_aslr

## _Disable Userspace ASLR for iOS via PongoOS_


## IMPORTANT
- **The current kpf matches/masks are only tested on iOS 14.8 iPhone X, if it does not work for your device/iOS please feel free to contribute to this project.**

## Explanation

- By patching slide to 0 in function [parse_machfile](https://github.com/apple-oss-distributions/xnu/blob/8d741a5de7ff4191bf97d57b9f54c2f6d4a15585/bsd/kern/mach_loader.c#L892) found in the xnu kernel.
<img width="271" alt="image" src="https://github.com/user-attachments/assets/f954cd1d-7cb4-4660-902f-1aa7a2c42092">
<img width="565" alt="image" src="https://github.com/user-attachments/assets/4acf8d15-76b1-43db-98d7-79161196662b">




## Usage (See Makefile for test command)


## Credits

- Special thanks to the Checkra1n team for [PongoOS](https://github.com/checkra1n/PongoOS).
- [Billy Ellis](https://github.com/Billy-Ellis) for his [research](https://bellis1000.medium.com/aslr-the-ios-kernel-how-virtual-address-spaces-are-randomised-d76d14dc7ebb).
- [Cryptiiiic](https://github.com/cryptiiiic) for his advice.
- [blacktop](https://github.com/blacktop) for his awesome [ipsw](https://github.com/blacktop/ipsw) tool.
