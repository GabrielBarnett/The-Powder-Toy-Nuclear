The Powder Toy - June 2026
==========================

Get the latest version [from the Powder Toy website](https://powdertoy.co.uk/Download.html). We're also on [Steam](https://store.steampowered.com/app/1148350/The_Powder_Toy/).

To use online features such as saving, you need to [register an account](https://powdertoy.co.uk/Register.html).
You can also visit [the official TPT forum](https://powdertoy.co.uk/Discussions/Categories/Index.html).

Have you ever wanted to blow something up? Or maybe you always dreamt of operating an atomic power plant? Do you have a will to develop your own CPU? The Powder Toy lets you to do all of these, and even more!

The Powder Toy is a free physics sandbox game, which simulates air pressure and velocity, heat, gravity and a countless number of interactions between different substances! The game provides you with various building materials, liquids, gases and electronic components which can be used to construct complex machines, guns, bombs, realistic terrains and almost anything else. You can then mine them and watch cool explosions, add intricate wirings, play with little stickmen or operate your machine. You can browse and play thousands of different saves made by the community or upload your own – we welcome your creations!

There is a Lua API – you can automate your work or even make plugins for the game. The Powder Toy is free and the source code is distributed under the GNU General Public License, so you can modify the game yourself or help with development.

Build instructions
===========================================================================

See the _Powder Toy Development Help_ section [on the main page of the wiki](https://powdertoy.co.uk/Wiki/W/Main_Page.html).

Special Thanks
===========================================================================

* Stanislaw K Skowronek - Designed the original Powder Toy
* Simon Robertshaw - Wrote the website, current server owner
* Skresanov Savely
* Pilihp64
* Catelite
* Victoria Hoyle
* Nathan Cousins
* jacksonmj
* Felix Wallin
* Lieuwe Mosch
* Anthony Boot
* Me4502
* MaksProg
* jacob1
* mniip
* LBPHacker

Libraries and other assets used
===========================================================================

* [bzip2](http://www.bzip.org/)
* [FFTW](http://fftw.org/)
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp)
* [libcurl](https://curl.se/libcurl/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* [Lua](https://www.lua.org/)
* [LuaJIT](https://luajit.org/)
* [Mallangche](https://github.com/JammPark/Mallangche)
* [mbedtls](https://www.trustedfirmware.org/projects/mbed-tls/)
* [SDL](https://libsdl.org/)

Instructions
===========================================================================

Click on the elements with the mouse and draw in the field, like in MS Paint. The rest of the game is learning what happens next.

FISS Nuclear Sandbox Mod
===========================================================================

This fork includes a fictional gameplay-only FISS system. It is not real nuclear engineering, real shielding, real neutron physics, or weapon-design guidance. All constants are arbitrary sandbox tuning values.

Stages:

* Stage 1 adds `FISS`, `NTRN`, `FPRD`, `CMTR`, `HEXP`, `SHOK`, and `DENS`.
* Stage 2 adds fictional pressure tools and controls: `BEXP`, `SEXP`, `PABS`, `PWAL`, and `PDRV`.
* Stage 3 adds fictional radiant-energy and aftermath effects: `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR`.
* Stage 4 adds fictional control materials: `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, and `PDMP`.
* Stage 5 adds `CFIS`, a fictional compressed FISS phase for coherent cluster fission, and rebalances `SEXP`.

Stage 5 compressed FISS phase:

* `CFIS` is a temporary fictional solid phase formed from compressed `FISS` clusters.
* `CFIS` is not a real material phase, isotope state, or engineering model.
* By default, supported `FISS` clusters begin forming `CFIS` around 15 in-game pressure or enough stored compression.
* `CFIS` holds together, resists pressure scatter, synchronises nearby countdowns for roughly 90 to 120 frames, releases capped XRAY warning effects after a short arming period, releases HRAY during the final late-warning window, and then uses the existing capped fictional FISS reaction.
* Weak or unsupported compression can revert `CFIS` back into `FISS` without producing fission outputs.
* `SEXP` is now tuned as a modest pressure driver broadly comparable to `BEXP`; it should not be the main high-energy event.
* `IONZ` is temporarily disabled from active gameplay and hidden from the menu, but remains defined for save compatibility.
* `RADS` now cools toward ambient/minimum temperature instead of self-cooling into freezing temperatures.

Stage 4 controls:

* `NABS` absorbs `NTRN` and slowly degrades into inert residue.
* `NSLW` slows `NTRN` for easier observation and tuning.
* `NDIFF` randomises and weakens `NTRN` paths without multiplying particles.
* `RABS` absorbs `HRAY` and `XRAY`, converting them into heat.
* `RWAL` blocks thermal flash line-of-sight and strongly absorbs `HRAY` / `XRAY`.
* `HSINK` pulls bounded heat from nearby particles and stores it as temperature.
* `PDMP` reduces positive pressure spikes without creating pressure.

Important tuning groups live in `src/simulation/elements/FISS.h`: fission thresholds, particle lifetimes, pressure and heat caps, Stage 3 radiation caps, Stage 4 control-material constants, Stage 5 `CFIS` formation/countdown/sync/reversion constants, and `BEXP` / `SEXP` pressure-driver balance. Per-frame caps limit `NTRN`, `SHOK`, `HRAY`, `XRAY`, `RADS`, thermal flash events, `RADS` emissions, and `CFIS` cluster fissions.

Stage 5 tuning constants include `CFIS_FORM_PRESSURE_THRESHOLD`, `CFIS_FORM_COMPRESSION_THRESHOLD`, `CFIS_MIN_NEIGHBOURS`, `CFIS_FORM_RADIUS`, `CFIS_ARMING_DELAY`, `CFIS_COUNTDOWN_MIN`, `CFIS_COUNTDOWN_MAX`, `CFIS_DEFAULT_COUNTDOWN`, `CFIS_MIN_LIFE_BEFORE_FISSION`, `CFIS_XRAY_START_AFTER_FORMATION`, `CFIS_HRAY_START_BEFORE_FISSION`, `CFIS_REVERT_GRACE_FRAMES`, `CFIS_SYNC_RADIUS`, `CFIS_SYNC_STRENGTH`, `CFIS_MAX_SYNC_REDUCTION_PER_FRAME`, `CFIS_SYNC_MIN_COUNTDOWN`, `CFIS_COMPRESSION_DECAY`, `CFIS_ACTIVATION_DECAY`, `CFIS_HOLD_STRENGTH`, `CFIS_REVERT_TO_FISS_THRESHOLD`, `CFIS_MAX_CLUSTER_FISSIONS_PER_FRAME`, `CFIS_PREFISSION_XRAY_CHANCE`, `CFIS_PREFISSION_HRAY_CHANCE`, `CFIS_PREFISSION_MAX_EMISSIONS_PER_FRAME`, `CFIS_FORM_COOLDOWN`, `CFIS_DAMP_VELOCITY_DURING_COUNTDOWN`, `CFIS_VELOCITY_DAMPING_FACTOR`, and `CFIS_MIN_NEIGHBOURS_TO_REMAIN`.

Manual Stage 4 smoke tests:

* Fire `NTRN` at `NABS`; expected absorption and modest absorber heat.
* Fire `NTRN` through `NSLW`; expected reduced velocity.
* Fire `NTRN` through `NDIFF`; expected randomized direction with no new `NTRN`.
* Fire `HRAY` and `XRAY` at `RABS`; expected absorption and heat.
* Put `RWAL` between a `FISS` reaction and a heat-sensitive target; expected much less thermal flash heating behind the wall.
* Place `HSINK` near hot `FPRD` or `RADS`; expected local cooling and sink warming.
* Place `PDMP` near pressure tools; expected reduced positive pressure spikes.
* Spawn many `RADS`; expected varied lifetimes, with some long-lived particles and eventual inert decay.

Manual Stage 5 smoke tests:

* Place loose `FISS` with no pressure; expected no reliable `CFIS` conversion.
* Apply about 15 in-game pressure to a supported `FISS` blob; expected local `CFIS` formation.
* Apply below-threshold pressure; expected little or no `CFIS` conversion.
* Apply pressure to a single isolated `FISS` particle; expected neighbour requirements to prevent easy `CFIS` formation.
* Watch newly formed `CFIS`; expected countdowns around 90 to 120 frames, with no fission before the arming delay.
* Compress a `FISS` blob; expected nearby `CFIS` countdowns to pull together gently, emit XRAY around 10 frames after formation, emit HRAY around 20 frames before fission, and fission close in time after that warning phase.
* Compress `FISS` with pressure tools; expected fewer stray particles fissioning far away from the cluster.
* Apply weak or brief compression; expected some `CFIS` to revert safely to `FISS`.
* Detonate `SEXP` alone; expected BEXP-like heat and pressure, no plasma, and no extreme temperatures.
* Place Stage 4 controls near a `CFIS` reaction; expected `NABS`, `RABS`, `RWAL`, `PDMP`, and related controls to retain their existing effects.

Controls
===========================================================================

| Key                     | Action                                                          |
| ----------------------- | --------------------------------------------------------------- |
| TAB                     | Switch between circle/square/triangle brush                     |
| Space                   | Pause                                                           |
| Q / Esc                 | Quit                                                            |
| Z                       | Zoom                                                            |
| S                       | Save stamp (use with Ctrl when STK2 is out)                     |
| L                       | Load last saved stamp                                           |
| K                       | Stamp library                                                   |
| 0-9                     | Set view mode                                                   |
| P / F2                  | Save screenshot as .png                                         |
| E                       | Bring up element search                                         |
| F                       | Pause and step to next frame                                    |
| G                       | Increase grid size                                              |
| Shift + G               | Decrease grid size                                              |
| H                       | Show/Hide HUD                                                   |
| Ctrl + H / F1           | Show intro text                                                 |
| D / F3                  | Debug mode (use with Ctrl when STK2 is out)                     |
| I                       | Invert Pressure and Velocity map                                |
| W                       | Cycle gravity modes (use with Ctrl when STK2 is out)            |
| Y                       | Cycle air modes                                                 |
| Ctrl + E                | Cycle edge modes                                                |
| B                       | Enter decoration editor menu                                    |
| Ctrl + B                | Toggle decorations on/off                                       |
| N                       | Toggle Newtonian Gravity on/off                                 |
| U                       | Toggle ambient heat on/off                                      |
| Ctrl + I                | Install powder toy, for loading saves/stamps by double clicking |
| Backtick                | Toggle console                                                  |
| =                       | Reset pressure and velocity map                                 |
| Ctrl + =                | Reset Electricity                                               |
| \[                      | Decrease brush size                                             |
| \]                      | Increase brush size                                             |
| Alt + \[                | Decrease brush size by 1                                        |
| Alt + \]                | Increase brush size by 1                                        |
| Ctrl + C/V/X            | Copy/Paste/Cut                                                  |
| Ctrl + Z                | Undo                                                            |
| Ctrl + Y                | Redo                                                            |
| Ctrl + Cursor drag      | Rectangle                                                       |
| Shift + Cursor drag     | Line                                                            |
| Middle click            | Sample element                                                  |
| Alt + Left click        | Sample element                                                  |
| Mouse scroll            | Change brush size                                               |
| Ctrl + Mouse scroll     | Change vertical brush size                                      |
| Shift + Mouse scroll    | Change horizontal brush size                                    |
| Shift + R               | Horizontal mirror for selected area when pasting stamps         |
| Ctrl + Shift + R        | Vertical mirror for selected area when pasting stamps           |
| R                       | Rotate selected area counterclockwise when pasting stamps       |
| F11                     | Toggle fullscreen                                               |

Command Line
---------------------------------------------------------------------------

| Command               | Description                                      | Example                                     |
| --------------------- | ------------------------------------------------ | --------------------------------------------|
| `scale:SIZE`          | Change window scale factor                       | `scale:2`                                   |
| `kiosk`               | Fullscreen mode                                  |                                             |
| `proxy:SERVER[:PORT]` | Proxy server to use                              | `proxy:wwwcache.lancs.ac.uk:8080`           |
| `open FILE`           | Opens the file as a stamp or game save           |                                             |
| `ddir DIRECTORY`      | Directory used for saving stamps and preferences |                                             |
| `ptsave:SAVEID`       | Open online save, used by ptsave: URLs           | `ptsave:2198`                               |
| `disable-network`     | Disables internet connections                    |                                             |
| `disable-bluescreen`  | Disable bluescreen handler                       |                                             |
| `redirect`            | Redirects output to stdout.txt / stderr.txt      |                                             |
| `console`             | Redirects output to a new console on Windows     |                                             |
| `cafile:CAFILE`       | Set certificate bundle path                      | `cafile:/etc/ssl/certs/ca-certificates.crt` |
| `capath:CAPATH`       | Set certificate directory path                   | `capath:/etc/ssl/certs`                     |
