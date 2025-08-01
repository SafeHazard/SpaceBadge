# Space Badge
The official repo of SPACE BADGE.
- [Intro](#intro)
- [Usage](#usage)
- [Multiplayer](#multiplayer)
- [Games](#games)
- [Single Player](#single-player)
- [Hardware Bill of Materials (BOM)](#hardware-bill-of-materials-bom)
- [Enclosure](#enclosure)
- [Credits](#credits)
- [AI Disclosure](#ai--ip-disclosure)
- [IP Disclaimer](#intellectual-property-disclaimer)
- [Known Issues](#known-issues)

# Intro
I created the Space Badge for DEF CON 33 in 2024-2025. It's meant to be a fun, multiplayer electronic badge for a conference and a community that I love.

It's a celebration of the creativity, range of interests and depth of talent that makes this scene so incredible.

This repo is here to share the code that makes the badge go, share the BOM, enclosure model and documentation.

This project was a huge effort for a high school sophomore. I genuinely hope you love it.
# Usage
## Buttons
From the front of the enclosure (farthest from the text on the side and USB port):
- **Bootloader** - hold this down while powering on/resetting the board to enter bootloader mode, then release
- **Power off** - press this to immediately shut down your badge. If connected to USB power, this reboots your badge
- **Power on** - press and hold for ~2 seconds to power the badge on
## LEDs & Charging
- **Red LED:** Badge is powered on
- **Green LED:** Badge is charging (over USB-C)
  - The green LED turns off once charging is complete
## Main Screen
- The main screens are accessible along the left of the screen
- The icons across the bottom tell you:
	- Battery charge (estimated; also: don't trust it when plugged in)
	- Badge mode (enabled/disabled)
	- Wifi status (enabled/disabled)
## Setup
- **Username**
  - Main > Profile > [type a username] > Save
  - This will be advertised to all other users, so please be respectful
- **Avatar**
  - Main > [Tap the image in the top right] > select an image from the roller
  - This is the image that others will see. It can be changed whenever you want
  - More avatars are unlocked by progressing in games
- **Badge Mode**
    - This mode shows off your game scores and rank after a specified delay
	- Enabled by default
	- Lowers power consumption of the badge 
- **Privacy**
	- The SPACE BADGE uses a private wifi mesh network to communicate and **does not** connect to the internet
	- Settings > Cloak (Airplane Mode)
		- This powers down the wifi radio and cloaks you from other users (saves power)
		- As you'd expect, multiplayer is not available when cloaked
- **Filtering Users & Usernames**
  - Settings > Show Usernames: Everyone, Not Blocked, Crew, None
    - This is used to cycle through options of which usernames you'll see in the 'Contacts' screen
    - Used mainly to hide usernames (you'll see their unique ID instead) of other users that you don't want to see. For instance, some users will likely choose some... questionable... usernames that you might not want to see. 
    - **Everyone:** You'll see everyone's usernames
    - **Not Blocked:** As long as you haven't blocked them, you'll see their username
    - **Crew:** You'll only see usernames for users whom you've marked as 'Crew'
    - **None:** You'll only see unique IDs
  - Settings > Show Game Hosts: Everyone, Not Blocked, Crew, None
    - This is used to cycle through which Hosting usernames you'll see when attempting to join a game
    - Like 'Show Usernames' this is used primarily to hide certain types of 'creativity' that you might not want to see
    - **Everyone:** You'll see every Host's username
    - **Not Blocked:** As long as you haven't blocked them, you'll see a Host's username
    - **Crew:** You'll only see usernames for Hosts whom you've marked as 'Crew'
    - **None:** You'll only see unique IDs in the 'Join' screen
# Multiplayer
SPACE BADGE is meant to be a multiplayer experience. 
Communications are run over a private wifi mesh network. 
## Scoring
- Multiplayer gives you the opportunity to earn more XP than you would solo for a given game 
- Each player has a target score for each session that's shown by the 'Score' bar in the top right of a game's screen during play
- If all players reach their target score, the whole crew gets an XP multiplier
	- If not... then they don't
- Each game has its own score which unlocks avatars and contributes to your rank
- Game score and rank are shown off in badge mode
## Hosting
- Mission > Host
	- Select the game you want (this is only for you -  
	- Wait for players to join (red empty icon by their name) and ready up (green check by their name)
	- Click 'Engage'
- Tap on a player and then 'Kick' to remove them from the game (they'll be able to re-join); no, there's no 'ban' function
## Joining
- Mission > Join
	- Select the game you want to play
	- Tap 'Ready' when ready (the host can't start until all players are ready)
	- The host can kick you from the game
# Games
Each game has 'helpful' tutorials within the badge.
There are several levels of difficulty for each game and the higher your score, the more challenging things get.
Each level of difficulty has a new spin on it to keep things interesting.
Games last 2 minutes each - score as much as you can within that time.
There is no pause function.
## Game 1: Assimilation
Land on the platform - but don't come in too hot!
Tap on the left or right side (don't hold or spam - won't work!) to add some upward velocity and a little angle.
## Game 2: LineCon
DEFCON's favorite game! Keep random objects in the air.
Each tap moves the highlighted set of hands (note that they alternate).
## Game 3: Packet Filter
Bad things are incoming. 
Tap the bad ones - but leave the good ones alone.
## Game 4: Wiring
Connect each symbol with its twin. But don't cross the lines between each.
Use the selector at the bottom to choose which type of wire to use.
## Game 5: Defender
Ever work a help desk? It's like that - stuff getting thrown at you constantly.
Tap in one of 4 directions to shield yourself from whatever gets thrown your way...
## Game 6: Speeder
Use the force (of tapping) to avoid obstacles as you speed toward the finish.
Tap in the direction you want to move; there are 3 'lanes' to choose from.
# Single Player
- The process is the same as 'Hosting' a game, but start the game before anyone joins (or enable your cloak).
# Hardware Bill of Materials (BOM)
This is where I got my materials from. No endorsement of any particular vendor expressed/implied (though these particular vendors treated me VERY well).
- Waveshare ESP32-S3 (SKU 27690) [link](https://www.waveshare.com/esp32-s3-touch-lcd-2.8.htm?sku=27690)
- MakerHawk 2200mAh 3.7V Li-Ion rechargeable battery [link](https://www.amazon.com/dp/B0D3LQYX49)
- GS-JJ Dye-Submilmated 1" Lanyards with Lobster Claw Clips [link](https://www.gs-jj.com/lanyards/Custom-Lanyards)
- Frienda Cell Phone Charm Straps [link](https://www.amazon.com/dp/B094MQD3DV)
- BambuLab A1 with AMS Lite [link](https://us.store.bambulab.com/products/a1?id=579550514255634440)
- BambuLab PLA Matte Filament (Ivory White, Charcoal, Dark Red) [link](https://us.store.bambulab.com/products/pla-matte)
- SUNLU Glow in The Dark PLA Filament [Orange](https://www.amazon.com/dp/B0C4LT6SFH) | [Blue](https://www.amazon.com/dp/B0C4LSVW6N)
# Enclosure
- The enclosure was designed for printing on BamuLab A1 printers with an AMS Lite.
- See /enclosure/ for the 3mf file
# Credits
Projects like this are almost never done solo. Here are some of the many who made SPACE BADGE possible:
- Project supporters (including all the Kickstarter backers who believed in this!)
- My family for sticking with me for this year-long project
- Every IP owner/creator who inspired a part of this project
- Our suppliers: Waveshare, MakerHawk, GS-JJ & Amazon
- Michael Okuda: the original LCARS design
- Jim @ theLCARS.com
- @grandkhan221b on tumblr for sharing their amazing art
- trekcore.com for collecting and sharing so much audio
- Everyone who contributed to open source projects we used, including ESP32, LVGL, audio-tools, painlessMesh, Async TCP, TaskScheduler, Arduino, EEZ Studio, Invoke AI and all the talented contributors on HuggingFace
- Special thanks to both the Visual Studio 2022 and Visual Micro development teams
- Anyone whose posts helped me write better code (there were a lot)
- The range of AIs (and all the training material they learned from) that helped me - Claude and ChatGPT in particular (and the adults that paid for Pro on each for me)
- Anyone else I forgot (sorry!)
# AI Disclosure
All images in this were either created by Artificial Intelligence (AI) at my direction or created by me. 
Reference images were used in many cases, but to the best of my knowledge & efforts, no commercial content was used in this badge and I used the work of others with permission and within requested scope. Any misuse was unintentional.
If you plan to use any portion of this project, please check applicable laws to ensure you are respecting the intellectual property of others. I've done my best to do that in this project and am grateful for the patience and understanding of IP owners if I made any mistakes. Thank you!
# Intellectual Property Disclaimer
- This project is an independent fan creation not affiliated with, endorsed by, or sponsored by any intellectual property holders referenced herein. All original artwork was created by the project team or generated using AI tools under our direction.
- Any resemblance to existing intellectual properties is intended as parody, homage, or fair use commentary in the spirit of fan appreciation. Reference materials may have been used for inspiration, but all final implementations represent transformative works.
- This product is sold based on its technical functionality and craftsmanship, not on any intellectual property references. Purchasers are advised that they are buying an electronic badge/device, and any IP-related content is incidental to the core product.
- No trademark or copyright infringement is intended. All trademarks and copyrights remain the property of their respective owners. If any intellectual property holder objects to references made herein, please contact us for immediate resolution.
- This disclaimer does not constitute legal advice. Users assume all risks associated with the use of this product.
# Known Issues
- Booting is a tad slow, but I did it that way to make the UI much more responsive (frames/second vs seconds/frame)
- It can take 30-ish seconds for game hosts and clients to "see" each other
- Audio may cut out; a reboot tends to fix that
- There may be occasional issues that cause a board to reboot
- If you find yourself in a 'boot loop' situation:
  - If you can mount the LittleFS partition and edit default.json you can fix whatever setting is causing issues
  - If not, you can flash your badge with the precompiled .bin from this repo (or build one yourself - it's fun-ish)

