# Snolf Mania: Reqiuem Ultimax
*At last.*
Take control of Snolf - the "coolest golf ball" - in his biggest adventure yet! All the Sonic Mania action you love, through the horrendous lens of Golf Controls! Take aim, set up your shot, and hope in vain that you don't tumble off of a cliff! 

# Installing
* Install the **RSDKv5U decompilation**, either by compiling it yourself, or from its [Releases page here.](https://github.com/Rubberduckycooly/RSDKv5-Decompilation/releases)
* Install the **Sonic Mania decompilation**, either by compiling it yourself, or from its [Releases page here,](https://github.com/Rubberduckycooly/Sonic-Mania-Decompilation/releases) putting it in the same directory that you put the RSDKv5U decompilation. Rename the **SonicManiav5U-x64.dll** to **Game.dll**. (On Linux, the file extension will be **.so**).
    * (Note that enabling Sonic Mania Plus content requires you to compile Sonic Mania yourself.)
* Install **Sonic Mania** from Steam or your storefront of choice. Go to its directory, and copy the **Data.rsdk** file and place it in the same directory that you put the RSDKv5U decompilation.
* Download **Snolf Mania** from the [Releases page here!](https://github.com/DrMelon/SnolfMania/releases)
* Create a **"mods"** directory in the same directory that you put the RSDKv5U decompilation, and then extract **Snolf Mania** into it, so that the directory structure looks something like:  

```
RSDKv5U/
----Game.dll
----RSDKv5U_x64.exe
----mods/
--------SnolfMania/
------------Data
------------SnolfMania.dll (or SnolfMania.so)
------------
```

* Create a file called **modConfig.ini**, and put it in the **mods** folder. Copy and paste this into it:  

```
[Mods]
SnolfMania=true
```

* Now, you can **launch RSDKv5U_x64.exe** and, if everything is working, you can now enjoy Snolf Mania!
* Alternatively, you can use the **RSDK Mod Manager** via [GameBanana.](https://gamebanana.com/tools/10457)



# Playing and Controls
* Press **Jump** when you're on the ground and not moving to open the **Snolf Shot Meter**. Set your **horizontal shot strength** by waiting until the meter is in the desired place, and press **Jump** again.
* Set your **vertical shot strength**, and lock it in with the **Jump** button. Snolf will **shoot into the air**! Or along the ground!
* Are you **rolling too fast?** Press **Left** or **Right** while rolling against your direction of travel to **Slow Your Roll**.
* Is your shot sending you careening into the unknown? **Are you stuck** between two aggressive springs, bouncing forever? **Hold the Jump button** while Snolf is moving to **mulligan, and reset yourself** back to the last place you took a shot.
* **NEW, IN SNOLF MANIA: REQIUEM ULTIMAX**: When playing as Sonic, you will not have access to the Drop-Dash. Instead, press **Down+Jump** when you're on the ground and not moving to open the **Topspin/Backspin Meter**. Tap **Left** or **Right** to set your backspin or topspin speed, up to **3 levels (slow, medium, and fast)**. Lock it in with **Jump** and perform a **Snolf Shot** as usual; Snolf will be launched into the air with spin! If the power level wasn't zero, **Snolf will dash along the ground upon landing**, in the direction and speed that you set! Use this for complex maneuvers!


# Building
* I'm going to assume you know what you're doing if you're reading these instructions.
* Check out the repository with git: `git clone --recursive git@github.com:DrMelon/SnolfMania.git`  
* Navigate into the directory: `cd SnolfMania`
* **Linux/MacOS**: `cmake .`, then `cmake --build .`
* **Windows**: Install vscode, and the CMake Tools extension from Microsoft. Open the folder in vscode, and when prompted to configure with CMake in vscode, select "Yes". Build release using the CMake Tools extension for Win x64. Note: I have only tested this with LLVM/Clang (16.0.5 x86_64-pc-windows-msvc) so far on Windows.
* **Other Platforms**: You probably know better than I do - and it's likely going to involve running cmake with your preferred target configuration.

[![CMake](https://github.com/DrMelon/SnolfMania/actions/workflows/cmake.yml/badge.svg)](https://github.com/DrMelon/SnolfMania/actions/workflows/cmake.yml)

# Troubleshooting
* If there's a significant, game-breaking bug, such as a total soft-lock **unresolvable by mulligans**, please raise an [Issue in the GitHub issues page of this project.](https://github.com/DrMelon/SnolfMania/issues).
* Otherwise, good luck.

# Credits & Special Thanks
* Using **Rubberduckycooly**'s GameAPI, Sonic Mania Decompilation, & RSDKv5 Decompilation. 
* Some configuration and files based on **MegAmi24**'s mods. Thank you for making them public!
* **Dowolff, Bartogian, videochess, Brossentia, Lizstar** - for your continued inspiration for me to make these horrible things.

