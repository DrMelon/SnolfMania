# Snolf Mania Implementation Plan
*"Ah, Snolf. Here we go again."* ~ DrMelon, 2023 

1. Add basic Snolf things here, like the shot-meter variables (timer, current meter stage, horiz/vert shot power magnitudes) and "last-known good mulligan position" etc.
2. Add some helper functions to perform the shot meter logic and the launch logic.
3. Modify Global/Player.c to call into the helper functions and force rolling on player characters. Look for entry points for other objects that might take control of the player. Wall-bounces, infinite lives, etc. Shot-types is something that's a bit up in the air right now.
4. Add new objects to represent shot-meter. Might be able to make some nice custom UI for it instead of just the floating rings in this one? Maybe a toggle for Snolf purists.
5. Do cosmetic changes: title screen (possibly even title screen animations), character names, UI, sound effects. 
6. Figure out mod distribution. Do other mods need binary mods like this? How does the mod launcher work? How do I distribute this in a way that's natural for Sonic Mania mod users (e.g, is GameBanana the primary method?). Figure out RSDK stuff. Might need to join the RGE modding discord and ask around, but it would be nice to figure it out myself and just go "ta-da!" out of the blue. We'll see how far I get :P
7. Need some Mania-specific control gimmicks. Maybe repurpose the drop-dash as a topspin/backspin shot option? Hmm...
