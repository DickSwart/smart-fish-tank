# smart-fish-tank

<p align="center">
    <img src="https://swart.ninja/assets/images/SwartNinjaLogoV2.svg" alt="SwartNinja logo" height="150">
</p>

## Parts Used
---
- 1 x N-Channel MOSFET (IRF530N)
- 1 x NodeMCU V3.0
- 2 x Bi-Directional Logic Level Converter (JY-MCU)
- 1 x 12v to 5v Step Down
- 1 x 12V 6amp Power Supply
- 1 x Digital RGB Leds
- 1 x Project Box
- 1 x Power Jacks
- 1 x Toggle Switch
- Header Wires

## Fritzing Sketch
---
<p align="center">
  <img src="/assits/images/fritzing-breadboard.jpg">
</p>

## Wiring
---

### Logic Shifter 1 - 5v Side
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Green | VCC | Step down converter 5v | positive |
| Purple | GND | Step down converter 5v | negative |
| Blue | 1<sup>*</sup> | Relay Module | IN1 |
| Green | 2<sup>*</sup> | Relay Module | IN2 |
| Brown | 3<sup>*</sup> | Relay Module | IN3 |
| Purple | 4<sup>*</sup> | Relay Module | IN4 |

### Logic Shifter 1 - 3.3v Side
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Orange | VCC | NodeMCU | 3v |
| Black | GND | NodeMCU | G |
| Blue | 1<sup>*</sup> | NodeMCU | D6 |
| Purple | 2<sup>*</sup> | NodeMCU | D5 |
| Gray | 3<sup>*</sup> | NodeMCU | D0 |
| Green | 4<sup>*</sup> | - | - |

### Logic Shifter 2 - 5v Side
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Green | VCC | Step down converter 5v | positive |
| Purple | GND | Step down converter 5v | negative |
| Brown | 1<sup>*</sup> | Terminal Block | 4 |
| - | 2<sup>*</sup> | - | - |
| - | 3<sup>*</sup> | - | - |
| - | 4<sup>*</sup> | - | - |

### Logic Shifter 2 - 3.3v Side
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Orange | VCC | NodeMCU | 3v |
| White | GND | NodeMCU | G |
| Orange | 1<sup>*</sup> | NodeMCU | D2 |
| - | 2<sup>*</sup> | - | - |
| - | 3<sup>*</sup> | - | - |
| - | 4<sup>*</sup> | - | - |

_* numbering starts from next to the GND incrementing from left to right pin as there is no physical numbering on the board_

### Terminal Block
| Pin | Function |
|--|--|
| 1 | White & Color LED VCC (Step down converter 12v positive) |
| 2 | White LED GND |
| 3 | Color LED GND (Step down converter 12v negative) |
| 4 | Color LED Signal |
| 5 | Switch Signal (connected to D7 on NodeMCU) |
| 6 | Switch Ground (connected to G on NodeMCU) |

### Fish Tank Light Wire
| Color | Function |
|--|--|
| brown | White & Color LED VCC  |
| Black | White LED - GND |
| Green | Color LED - Signal |
| White | Color LED - GND |
| Orange | Switch |
| Blue | Switch |

## Photo
---
More photos and other assets available in the assits folder.
<p align="center">
  <img src="https://raw.githubusercontent.com/DickSwart/smart-fish-tank/master/assits/images/project/IMG_0656.jpg">
</p>
