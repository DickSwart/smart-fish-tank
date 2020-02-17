# smart-fish-tank

<p align="center">
    <img src="https://swart.ninja/assets/images/SwartNinjaLogoV2.svg" alt="SwartNinja logo" height="150">
</p>

## Parts Used
- N-Channel MOSFET (IRF530N)
- NodeMCU V3.0
- Bi-Directional Logic Level Converter (JY-MCU)
- 12v to 5v Step Down
- 12V 6amp Power Supply
- Digital RGB Leds
- Project Box
- Header Wires
- Power Jacks

## Fritzing Sketch
To Add


## Fritzing Sketch
_TODO!!_

## Wiring

### Logic Shifter 1
**5v Side**
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Green | VCC | Step down converter 5v | positive |
| Purple | GND | Step down converter 5v | negative |
| Blue | 1 | Relay Module | IN1 |
| Green | 2 | Relay Module | IN2 |
| Brown | 3 | Relay Module | IN3 |
| Purple | 4 | Relay Module | IN4 |

**3.3v Side**
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Orange | VCC | NodeMCU | 3v |
| Black | GND | NodeMCU | G |
| Blue | 1 | NodeMCU | D5 |
| Purple | 2 | NodeMCU | D6 |
| Gray | 3 | NodeMCU | D0 |
| Green | 4 | - | - |

### Logic Shifter 2
**5v Side**
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Green | VCC | Step down converter 5v | positive |
| Purple | GND | Step down converter 5v | negative |
| Brown | 1 | Terminal Block | 4 |
| - | 2 | - | - |
| - | 3 | - | - |
| - | 4 | - | - |

**3.3v Side**
| Wire |  Pin | Destination | Destination Pin |
|---|---|---|---|
| Orange | VCC | NodeMCU | 3v |
| White | GND | NodeMCU | G |
| Orange | 1 | NodeMCU | D2 |
| - | 2 | - | - |
| - | 3 | - | - |
| - | 4 | - | - |

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
| Red | White & Color LED VCC  |
| Black | White LED - GND |
| Green | Color LED - Signal |
| White | Color LED - GND |
| Orange | Switch |
| Blue | Switch |