# unreal-with-rust
unreal engine third persion demo written with rust (and c++ as middleware)

This project is based on [unreal-rust](https://github.com/MaikKlein/unreal-rust.git), so the license was both MIT and Apache.

But different with unreal-rust, this project is not data-driven(or ecs) style.

# usage 
    1.Create your own ue5.1(this moment) project.
    2.Modify setup.bat and build_win32.bat, and run setup.bat.
    3.Enter your ue editor, then refresh vs project and build.
    4.Install rust env(1.65.0+),run build_win32.bat.
    5.Setup game objects(blueprint and others), drag a monster onto map.
    6.Run in ue editor, press 1 to attack.
# ffi apis
This project use the unreal-rust ffi code gen tool on my another repository: 
[UnrealObject2RustBuilder](https://github.com/DrYaling/UnrealObject2RustBuilder.git)

If you have any questions please submit issues both on this rep or tool rep.
# other

    This project is in very early stage, many apis maybe not stable,and game has only limited functions .

    I will stick on this project(or unreal-rust framework),to learn unreal and find a player way to write(make) game.

    Welcome everyone to give me some advise or make contribution to this demo. 
