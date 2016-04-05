# blitzle
Small PoC for injection-less aimbot.

## Usage
`blitzle.exe {args}`
Where 'args' are:
* -dota : use Dota 2 player detector
* -paladins : use Paladins player detector
* -debug : show visual debug with trackbar controls instead of player detection
* -scale : output window scale from 0.0f to 1.0f 
* -output : index of output adapter (monitor) to capture, from 0 to n

If '-paladins' detector is chosen, then you must also specify:
* -template : path to the HP bar template to search

Run with no args defaults to:
`blitzle.exe -dota -scale 0.5f -output 0`
