## C++ Coding Style Conventions

Here it is a quick list with some of the conventions used in this project template:

Code element | Convention | Example
--- | :---: | ---
Defines | ALL_CAPS | `#define PLATFORM_DESKTOP`
Macros | ALL_CAPS | `#define MIN(a,b) (((a)<(b))?(a):(b))`
Variables | camelCase | `int screenWidth = 0;`
Local variables | camelCase | `Vector2 playerPosition;`
Global variables | camelCase | `bool fullscreen = false;`
Constants | camelCase | `const int maxValue = 8`
Pointers | MyType* pointer | `Texture2D* array;`
float values | always x.xf | `float value = 10.0f`
Operators | value1 * value2 | `int product = value * 6;`
Operators | value1 / value2 | `int division = value / 4;`
Operators | value1 + value2 | `int sum = value + 10;`
Operators | value1 - value2 | `int res = value - 5;`
Enum | TitleCase | `enum TextureFormat`
Enum members | ALL_CAPS | `UNCOMPRESSED_R8G8B8`
Struct | TitleCase | `struct Texture2D`
Struct members |camelCase | `texture.id`
Functions | TitleCase | `InitWindow()`
Class | TitleCase | `class Player`
Class Fields | camelCase | `Vector2 position`
Class Methods | TitleCase | `GetPosition()`
Ternary Operator | (condition)? result1 : result2 | `printf("Value is 0: %s", (value == 0)? "yes" : "no");`

### Branching policy
Everyone should work on their own branch (develop branch), and every time we finish a functionality and QA tested, then push towards the release branch (main).

### The #define guard
+ All defines and macros should be in all caps separated by underscores if there are multiple words.
+ All header files should have #define guards to prevent multiple inclusion.
```c++
> #ifndef FOO_H_
>
> #define FOO_H_
>
> ...
>
> #endif FOO_H_
```
### Variable naming
+ Variables should be in camelCase.
+ Variable names should specify what they do:
    * If a variable is used instantly and never used again in an obvious usage (e.g iterator in a while), it's fine with it being a single letter (e.g.: i).
    * If a variable is used in multiple parts of the code, be as long as necessary (e.g.: tilesFromPlayerToEnemy).
+ Do not deny boolean variables (e.g.: "notDead"), use: "isFalling", "isAlive" instead. 

### Enumerations
+ Always use enum class.
+ Enums should be in TitleCase.
+ Members should be in all caps with underscores if multiple words.

### Class and Structs
+ Always in TitleCase.
+ Do not mix public and private members.

### Miscellaneous
+ Brackets after functions should go on a new line.
```c
void SomeFunction()
{
   // TODO: Do something here!
}
```
+ Use **TABS** instead of 4 spaces.
+ Usage of `constexpr` is highly advised instead of the C-style macros.
+ Use nullptr instead of NULL.

## File Organization

File formats encode information so that it can be accessed by the same or similar software programmes in the future and accessed from any computer and some mobile devices.  Selection of file format is often determined from the software and hardware that we initially use to encode the information. 
With that being said, here we have the format files we will be using for the project:

### Art
+ Art assets should be in *.png.
+ Sprites with few (2-) or no animations:
    + Should be encapsulated in a folder.
    + Should be named as: Identifier of item + image variation number + "_" + name of animation + frame number. Example:
    > * Items
    >     * arrow0_shine000.png
    >     * arrow0_shine001.png
    >     * arrow0_static000.png
    >     * arrow1_shine000.png
    >     * coin0_rotation000.png
    >     * ...
+ Sprites with a large (3+) animations 
    + Should have their own folder per animation.
    + If the images are part of a longer animation, they should be named: name of the animation followed by the frame number. 
    + Example:
    > * Characters
    >     * Player
    >         * Idle
    >              * idle1.png
    >              * idle2.png
    >         * Attack
    >              * ...
    >         * Run
    >              * ...
    >     * Enemies

### Audio
+ File format has to be either *.ogg or *.wav.
+ It has to be stored in the corresponding SFX or music folder.
+ In case there are multiple sounds related to each other (e.g.: enemy sounds on battle) they should be inside a folder. Example:
+ Example:

    > * Audio
    >     * SFX
    >         * Battle
    >              * Enemies
    >                  * Slime
    >                      * slime0_hit.ogg
    >                      * slime0_death.ogg
    >                      * slime0_attack.ogg
    >              * Player
    >                  * ...
    >         * Overworld
    >              * ...
    >         * Example3
    >              * ...
    >     * Music
    >         * ...

### Map assets
+ Tiled format file should be *.tmx.
+ Maps in tiled should **NOT** have tileset data embedded on them. It should be in their own *.tsx file.
+ The *.tsx file should be stored in the same folder as the *.png.

### Data saving
+ Configuration files and databases should be on .xml format.