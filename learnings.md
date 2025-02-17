##

## Todo
- Test on different devices and see the lag. Test the suggestion of updating the position of the player localy and then smooth in the response from the server.
- When moving keep track which tile idx the object is in. Make WorldPosToTileIdx better. Implement a tryMove(Player& player, double dt) to worldObjects
- Optimize collision detection
- Implement shooting of fireballs again (simple animation to begin with)
- Fire and smoke as particle simulations on the GPU

## Compilation
https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170 
https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022 

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

### Water simulation
Ali had an idea of having the water wave information in a large texture. maybe it could have r and g as direction and b as height. Then it would be possible to have
interaction with the water, so if a ball fall in the water waves will be created and spreading. I don't understand yet how edges of water tiles can be handled and how normals can be
calculated to get the light reflection.

After this is implemented the animation of it should be begun with

### Slime
How do we get to slime?... Reddit says byproduct of missused magick... but which magick then. Blood from another player dying maybe??? Or even better idea, a meteor from space with this goo on.

## Shaders
https://learnwebgl.brown37.net/12_shader_language/glsl_builtin_functions.html 

## Graphics
 If you need a depth buffer for 3D rendering, don't forget to request it when calling RenderTexture::create

## Particle Simulation
The current idea is to learn how to use a Compute Shader to simulate a lot of particles.
This is one of the youtube videos you have seen on the matter. Slime:
https://github.com/SebLague/Slime-Simulation/blob/main/Assets/Scripts/Slime/Simulation.cs

## Measurements
Sending whole game:
21244 bytes
 1565 bytes after adding delta to worldObjects
  650 bytes after adding delta to tiles as well

Clients active time period: 0.02
Server  active time period: 0.0003
So server is 67 times as fast
60 fps => 0.016 period time

## CMake info
https://en.sfml-dev.org/forums/index.php?topic=24070.0 
https://github.com/SFML/SFML/blob/master/cmake/SFMLConfig.cmake.in