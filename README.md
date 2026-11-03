# Rose.exe

## What is it?
This is the source code of a crackme which was developed in 2024 and uploaded to crackmes.one

Rose.exe is a 13 stage crackme, where each stage aims to introduce a new class of obfuscation technique, or an advancement of a previous technique, or the combination of multiple techniques. It starts from simple runtime patches to keygens to SEH hooks to virtual machines, and a whole lot more tricks bundled together.

Progress is saved through a text file, and each stage's solution doubles as the key to decrypt the next stage. This is to force the user to beat the game the intended way rather than jmping across stages, otherwise the next stage will be encrypted and unplayable.

## Notes about the repository
The code has been published un-refactored on purpose, with a few comments of my thoughts on the code looking back at it. If you want to learn more about how this project works, the best way is to read the code and comments to see the inner workings of this. Yes, the code has a lot of brute-force, magic numbers, and non-scaling components to it, but I have left the code base as it was. My code from this project onwards has been cleaner.

Two .sln files are included:
- **Rose**, the main part of this project which generates the base .exe file
- **rosencryptor**, a post-processing .exe that allowed me to inject inline x64 assembly (since MSVC does not support it) and applies static encryption to strings and code sections.

Build Rose first, then target the built .exe with rosencryptor (by editing the source code to the correct path of Rose.exe) to apply the necessary effects


## Credits
This project didn't come from nowhere. I thank:
- **Bitburner** (this showed me that games based on coding can be fun and engaging)
- **Entry Point (Roblox)** (one developer who made a game with a great storyline, Rose.exe is an unofficial fan-made prequel: the training Rose, the hacker from entry point, went through before becoming one)
- **A Hat in Time** (whimsical and lighthearted, made by a small team, which showed me that small developers can make a great experience)
- **Bloons TD X** (a simple but fun game made by a small developer)
- **CarlSagan42** (for the love of puzzles)
- **Press Continue** (a person who does brutally difficult challenges in games, he showed me that with determination, the seemingly impossible might just be possible, and I wanted to make a game which reflected just that to the person playing with the difficulty scaling)
- and especially **crackmes.one**, for flavouring this crackme in their own unique ways.

