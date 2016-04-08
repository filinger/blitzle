# blitzle
Small PoC for injection-less aimbot.

## Usage
`blitzle.exe {args}`

Where 'args' are:
* -dota : use Dota 2 player detector
* -paladins : use Paladins player detector
* -debug : show visual debug with trackbar controls instead of player detection
* -scale : output window scale from 0.0f to 1.0f
* -sensivity : aiming sensivity to compensate ingame sensivity settings from 0.0f to 10.0f
* -adapter: index of video adapter to capture, from 0 to n
* -output : index of output monitor to capture, from 0 to n

Run with no args defaults to:
`blitzle.exe -dota -scale 0.5f -sensivity 1 -adapter 0 -output 0`

Press `Esc` to exit.
