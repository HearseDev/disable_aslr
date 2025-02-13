INCLUDE_DIR = include

PONGOTERM = ./pongoterm/pongoterm
BOOT_HELPER = ./scripts/boot

MODULE = disable_aslr.bin
$(MODULE): main.c
	xcrun -sdk iphoneos clang main.c -o $@ -I$(INCLUDE_DIR) -arch arm64 -mabi=aapcs -Xlinker -kext -nostdlib -Xlinker -fatal_warnings -D_SECURE__STRING_H_ -O3 -flto -ffreestanding -U__nonnull -DTARGET_OS_OSX=0 -DTARGET_OS_MACCATALYST=0 

clean:
	rm -f $(MODULE)

# Spawn pongoterm (you need to have a pongoterm binary)
term:
	$(PONGOTERM)

# Boot into PongoOS using the boot helper script
boot:
	$(BOOT_HELPER)

# Run the module using pongoterm and the cmd.txt file.
# You may need to modify the cmd.txt file to suit your needs. For example, sep auto might not be needed for your device.
run: $(MODULE)
	$(PONGOTERM) <cmd.txt

# Boot into PongoOS and run the module for ease of use to help us debug.
boot_run: $(MODULE)
	$(BOOT_HELPER) && $(PONGOTERM) <cmd.txt

# Boot into PongoOS and spawn pongoterm for ease of use to help us debug.
boot_term:
	$(BOOT_HELPER) && $(PONGOTERM)