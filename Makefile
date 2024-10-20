SCRIPTS_DIR = scripts
INCLUDE_DIR = include

PONGOTERM = ./$(SCRIPTS_DIR)/pongoterm

MODULE = disable_aslr.bin

BOOT_HELPER = ./$(SCRIPTS_DIR)/boot_helper

$(MODULE): main.c
	xcrun -sdk iphoneos clang main.c -o $@ -I$(INCLUDE_DIR) -arch arm64 -mabi=aapcs -Xlinker -kext -nostdlib -Xlinker -fatal_warnings -D_SECURE__STRING_H_ -O3 -flto -ffreestanding -U__nonnull -DTARGET_OS_OSX=0 -DTARGET_OS_MACCATALYST=0 

clean:
	rm -f $(MODULE)
term:
	$(BOOT_HELPER) && $(PONGOTERM)
boot:
	$(BOOT_HELPER)
test: $(MODULE)
	$(BOOT_HELPER) && $(PONGOTERM) <cmd.txt
