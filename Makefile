SCRIPTS_DIR = scripts
TOOLS_DIR = tools
INCLUDE_DIR = include

PONGOTERM = $(TOOLS_DIR)/pongoterm

MODULE = disable_aslr.bin

$(MODULE): main.c
	xcrun -sdk iphoneos clang main.c -o $@ -I$(INCLUDE_DIR) -arch arm64 -mabi=aapcs -Xlinker -kext -nostdlib -Xlinker -fatal_warnings -D_SECURE__STRING_H_ -O3 -flto -ffreestanding -U__nonnull -DTARGET_OS_OSX=0 -DTARGET_OS_MACCATALYST=0 

clean:
	rm -f $(MODULE)

term:
	./$(PONGOTERM)
boot:
	CHECKRA1N_EARLY_EXIT=1 /Applications/checkra1n.app/Contents/MacOS/checkra1n -p

test: $(MODULE)
	$(PONGOTERM) <cmd.txt
