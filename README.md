# NVAN X-Plane Command Aliases Plugin
This is a simple plugin that makes aliases for commands that are "push and hold" so you can use them via the native X-Plane UDP API.

## Installation
Download the [latest version here](https://github.com/nvan/x-plane-command-aliases-plugin/releases).

Just extract and drop the plugin into `X-Plane/Resources/plugins` folder.

Then in the `commands.txt` file, write the commands you want aliases to be made for one per line.
You can place comments with `#`.

```txt
# Sample commands.txt file

# A couple commands for Fire Panel of Toliss A321
AirbusFBW/FireTestENG1
AirbusFBW/FireTestENG2
AirbusFBW/FireTestAPU

# Another sample of native planes
sim/flight_controls/pitch_trim_up
sim/flight_controls/pitch_trim_down
sim/flight_controls/rudder_trim_left
sim/flight_controls/rudder_trim_right
```

Three aliases will be created for each command:
- A dataref `COMMAND_press` if set to 1 will begin the command / if set to 0 will stop it.
- A command `COMMAND_begin` that will begin the command.
- A command `COMMAND_end` that will stop it.

Examples for `AirbusFBW/FireTestENG1`:
- `AirbusFBW/FireTestENG1_press`
- `AirbusFBW/FireTestENG1_begin`
- `AirbusFBW/FireTestENG1_end`

## Donations
If you use and like this program, consider buying me a 🍺 beer. You can donate via **PayPal**:

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/maduranma)

## Build
You can build & install using cmake (CMakeLists.txt file present). Remember to use MSVC as a compiler for Windows.

## Issues and contributions
Please, feel free to ask for questions or features, or report bugs in the **Issues** section.

Also, feel free to **Contribute** by making a **Pull Request**.

## Tested in our own A320 Simulator
We are building an A320 Flight Simulator in Mallorca, check it on [Instagram](https://www.instagram.com/a320flightsimulator/)!

## License
MIT

This program is free, but credit is appreciated if you use it at your setup!