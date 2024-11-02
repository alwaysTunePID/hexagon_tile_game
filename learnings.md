##

## Todo

## Coordinate Systems

Input coordinates - should immediatelly be translated to world coordinates
World coordinates - x,y,z (cartesian) used to store position in game. You need to implement 3d coordinates so that objects can be in the air and fall to the ground.
Tile coordinate - Currently used, but should be removed?
Screen coordinates - When drawing the world coordinates needs to be translated to screen coordinates. Includes camera translation and zoom.

## Sprite base
Sprites should have a base, meaning where it touches the ground. This will determine if the object should be drawn in front of or to the back of another object.
It will also help when calculating a pixel height if that pixel will become independent from the rest of the object. 

### Fire simulation
Fire is one pixel that after a second spreads to adjacent pixels of the same sprite.
After two seconds it dies, alternatively replaced by the sprite underneeth representing the burnt version of what it once was.
It needs to not spread to pixels that are already burnt, nor pixels that are transparent.
So add information to the object which pixels that are burnt. Probably an 2d array or something, with enum if it's burnt, currently burning or neither.
And have a common timer for when all currently burning pixels should spread and die.


After this is implemented the animation of it should be begun with

### Slime
How do we get to slime?... Reddit says byproduct of missused magick... but which magick then. Blood from another player dying maybe??? Or even better idea, a meteor from space with this goo on.