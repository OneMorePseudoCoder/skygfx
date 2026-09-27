![](http://i.imgur.com/RXklRRg.png)

## SkyGfx Extended

SkyGfx (Sky is name of Renderware for the PS2) brings accurate PS2 graphics to the PC version of San Andreas, Vice City and III (and now Xbox graphics to III and Vice City).

[More information on gtaforums.](http://gtaforums.com/topic/750681-skygfx-ps2-and-xbox-graphics-for-pc/)

This is a Extended alternative version maintained by me (Junior_Djjr) named as "SkyGfx Extended", in short, due to this project it is an excellent base for shaders in GTA SA, so this alternative version seeks to add new shaders and modern features to the game, different from the original SkyGfx goal.

![](https://www.mixmods.com.br/wp-content/uploads/2021/06/gta-sa-mod-skygfx-extended-stochastic-procedural-texture-tiling-fix-1503666.jpg)

![](https://www.mixmods.com.br/wp-content/uploads/2022/03/gta-sa-mod-skygfx-extended-alpha-transparency-fix-bug-9878175.jpg)

[More information and download on MixMods.](https://www.mixmods.com.br/2022/03/sa-skygfx/)

----------

How to compile:
Define the RWSDK36 environmental variable or replace it with the path to RWSDK in premake5.lua.
For example: 

    RWSDK36: <some path>\RWSDK\RW36\Graphics\rwsdk\include\d3d9

Use premake to generate a solution.

----------

How to configure:
Copy [skygfx.example.ini](skygfx.example.ini) next to `skygfx.asi` in your GTA San Andreas folder, rename it to `skygfx.ini`, and adjust the values you need. Every option, with its default, is documented in that file.

Presets: put multiple configs in the same folder as `skygfx1.ini` ... `skygfx9.ini`. With a `keySwitch` scancode set in the active config, that key cycles through the presets in-game (see the sample file for the scancode table). A `keyReload` scancode reloads all configs without restarting.

New in this fork:
- Screen FX pass: bloom, exposure + filmic tone map, and PS2-style 15-bit dithering — see the `[SkyGfx]` "screen fx" section in the sample config (all off by default).
- Screen FX 2: auto exposure (night-aware, smoothed), night bloom boost, vignette, subtle chromatic aberration, PS2-style grain + 2px scanlines, and `renderScale` (internal resolution — experimental, off by default). All documented in the "screen fx 2" section of the sample config. `renderScaleDebugLog=1` appends the D3D viewport/projection call sequence during the scene pass to `skygfx_renderScale.log` (diagnostics for renderScale issues).
- Real-time shadow upgrade: vanilla renders every ped/vehicle shadow from a 128x128 raster but downsamples it to 64x64 before drawing it on the ground — the cause of blocky shadow edges. `shadowResolution` raises the raster size power-of-two for the whole shadow pipeline (default 8 = 256x256, ~5 MB VRAM), `shadowSoftness` sets the blur pass count (-1 = vanilla pipeline, 0 = crisp, 1..8), and `shadowAllEntities` gives every ped/vehicle a real-time shadow even on FX quality below Very High. See the shadow section of the sample config.
