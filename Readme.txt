# EZFlash Parallel Recovery Tool


This is a save tool based on NDS Backup Tool geared towards dumping/restoring the internal firmware on EZ Flash Parallel Carts. The main modification to this program is the removal of the rom dumping code and allowing user to ignore invalid card error.
If save chip would otherwise detect as unknown, this program instead defaults to the 4MB size/type the EZP is known to use.


Unlike most other flashcarts EZFlash...for some reason...exposes the main firmware flash chip that contains the main rom and the ntrboot section to standard SPI commands.

Thus any ol save tool can interact with this chip and read/write to it. Because of this this card can end up bricked if even a single DS game is run that has a missing save patch.

There have already been a few reports of bricks. I have succefully unbricked a card with this tool.

I recommend not using fw 1.06 after you unbrick because any future bricks require physical mod to disconnect WP# pin. EZ Flash added code that sets write protection on first block or two of the flash chip.


Like NDS Backup Tool, I've made this tool compatible with both DSi/3DS and DS. To use on DS/DS Lite you will need a slot2 device. 3DS/DSi console must be softmodded before this tool can be used due to requiring both slot-1 and SD access. (not possible with older exploits on DSi or on stock 3DS)

# If you have a bricked card that had fw1.06 booted on it at least once

If you've managed to run kernel 1.06, the first 0x8000 bytes of the chip is locked.

The only way to undo the lock is to disconnect the WP# pin on the flash chip, then run this program. I added code that resets the write config registers that clears any block protection.
This only works however if the WP pin is lifted because they intentionally tried to lock the chip. (though not very well. The data after 0x10000 can still be written to. :P )

When the WP pin is lifted the card will no longer show up in menu. This is normal. This is also why I've allowed this tool to ignore invalid cards. That way the config register write code still runs.

After running this tool with the pin lifted (I don't recommend you attempt to flash anything to it while it's in this state), reconnect the WP# pin to return the card to normal operation.


Hopefully once EZFlash becomes aware of this tool they may push out a fw update that doesn't lock the chip. Perhaps they will use this program as official method of restoring the card.