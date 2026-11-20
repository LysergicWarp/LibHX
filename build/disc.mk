# Builds a disc image around a program's 1st Read file: the Initial Program, the ISO, a silent
# audio track and the CUE. Needs GNU make 4.3, the SH-2 compiler and objcopy, xorriso, and GNU
# dd, sha256sum, date and truncate.
#
# A Saturn disc has one data track and one audio track at least, each of four seconds or more.
# The data track is the ISO, filled up to that. The audio track is four seconds of silence
# behind a pause of two.
#
# Every disc carries Sega's Security Code and an Area Code block for each area it runs in. Both
# are cut from the user's BIOS on every build. The SDK holds no copy.
#
# The program sets, before the include:
#   HX_ROOT         the LibHX clone
#   HX_OUT          a folder of its build
#   HX_BIOS         the BIOS file
#   HX_DISC         the image's path without an extension, for HX_DISC.iso, HX_DISC.cue and
#                   HX_DISC.silence.bin
#   HX_DISC_FIRST   the 1st Read file: the program, as the bytes the BIOS loads
#   HX_IP_TITLE     the Game Title, 112 characters at most
#   HX_IP_DATE      the Release Date, YYYYMMDD. The volume and its files carry it too.
# No path holds a space, and no value a quote or a backslash.
#
# It may set:
#   HX_DISC_FILES       a folder whose contents go on the disc beside the 1st Read file. Its
#                       names are 8.3 and upper case already: xorriso changes any other name
#                       and says nothing. A link stops the build. The image is built on every
#                       run.
#   HX_DISC_VOLUME      the volume's name: A to Z, 0 to 9 and _, 32 characters at most. HX
#                       when it is not set.
#   HX_IP_MAKER         the Maker ID, 16 characters at most
#   HX_IP_PRODUCT       the Product Number, 10 at most
#   HX_IP_VERSION       the Version, as V1.000
#   HX_IP_DEVICE        the Device Information, as CD-1/1
#   HX_IP_AREAS         the Compatible Area Symbols, of J T U B K A E L
#   HX_IP_PERIPHERALS   the Compatible Peripherals, 16 at most
#   HX_IP_STACK_M       the first SH-2's stack pointer, 0 for the BIOS's own
#   HX_IP_STACK_S       the second SH-2's
#   HX_IP_FIRST_READ    the address the 1st Read file is loaded to and run from
#   HX_CC, HX_OBJCOPY   the SH-2 compiler and objcopy, when they are not the ones on the path
#
# The program gets:
#   HX_DISC_IMAGE   the image's three files, for a goal to depend on
#
# The BIOS loads the first entry of the disc's root, file or folder, in the order of the names.
# The 1st Read file is 0.BIN there. A folder with a name that would come ahead of it, 0 or one
# that begins "0.", stops the build.
ifeq ($(strip $(HX_ROOT)),)
  $(error Undefined HX_ROOT (the LibHX clone))
endif
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (a folder of the build, for the Initial Program))
endif
ifeq ($(strip $(HX_DISC)),)
  $(error Undefined HX_DISC (the image's path without an extension))
endif
ifeq ($(strip $(HX_DISC_FIRST)),)
  $(error Undefined HX_DISC_FIRST (the 1st Read file))
endif
ifeq ($(strip $(HX_IP_TITLE)),)
  $(error Undefined HX_IP_TITLE (the Game Title))
endif
ifeq ($(strip $(HX_IP_DATE)),)
  $(error Undefined HX_IP_DATE (the Release Date, YYYYMMDD))
endif

HX_CC?= sh2eb-elf-gcc
HX_OBJCOPY?= sh2eb-elf-objcopy

HX_IP_MAKER?= SEGA TP HX
HX_IP_PRODUCT?= HX-00001
HX_IP_VERSION?= V1.000
HX_IP_DEVICE?= CD-1/1
HX_IP_AREAS?= JTUBKAEL
HX_IP_PERIPHERALS?= J
HX_IP_STACK_M?= 0
HX_IP_STACK_S?= 0
HX_IP_FIRST_READ?= 0x06004000
HX_DISC_VOLUME?= HX

HX_DISC_IMAGE:= $(HX_DISC).iso $(HX_DISC).cue $(HX_DISC).silence.bin

# Four seconds, the least a track is: 300 sectors. The data track has 2048 bytes a sector here
# and ends in a pause of two seconds, 150 sectors, which xorriso writes as empty ones. The audio
# track has 2352 bytes a sector and its pause ahead of it, in the file.
HX_DISC_DATA_LEAST:= $$(((300 + 150) * 2048))
HX_DISC_SILENCE:= $$(((150 + 300) * 2352))

# The Security Code and the eight Area Code blocks: where a BIOS has them, and the checksum of
# each. A BIOS that has other bytes there stops the build, since its disc would not start. The
# places are those of v1.01a.
HX_BIOS_SECURITY:= skip=$$((0x6200)) count=$$((0xD00))
HX_BIOS_SECURITY_SHA256:= 96b8ea48819cfa589f24c40aa149c224c420dccf38b730f00156efe25c9bbc8f
HX_BIOS_AREAS:= skip=$$((0x6F00)) count=$$((0x100))
HX_BIOS_AREAS_SHA256:= e1a1993ad0c16a87653c5f48bc9426170a8da530943b66257e8ab85ec255f487

# The blocks' areas, in the BIOS's order
HX_BIOS_AREA_LETTERS:= J T U B K A E L

HX_IP_DIR:= $(HX_OUT)/ip

# The System ID's fields: the texts, and the numbers
HX_IP_TEXTS:= HX_IP_MAKER HX_IP_PRODUCT HX_IP_VERSION HX_IP_DATE HX_IP_DEVICE HX_IP_AREAS \
    HX_IP_PERIPHERALS HX_IP_TITLE
HX_IP_NUMBERS:= HX_IP_STACK_M HX_IP_STACK_S HX_IP_FIRST_READ

# What the image is made from, beside the files themselves
HX_DISC_SETTINGS:= $(HX_IP_TEXTS) $(HX_IP_NUMBERS) HX_DISC_FIRST HX_DISC_FILES HX_DISC_VOLUME \
    HX_CC HX_OBJCOPY

# A value reaches the shell between single quotes, and the assembler between double ones
$(foreach name,$(HX_DISC_SETTINGS) HX_ROOT HX_OUT HX_BIOS HX_DISC, \
    $(if $(findstring ',$($(name)))$(findstring ",$($(name)))$(findstring \,$($(name))), \
        $(error $(name) holds a quote or a backslash)))

HX_IP_DEFINES= \
    $(foreach name,$(HX_IP_TEXTS),-D$(name)='"$($(name))"') \
    $(foreach name,$(HX_IP_NUMBERS),-D$(name)='$($(name))')

HX_EMPTY:=
HX_SPACE:= $(HX_EMPTY) $(HX_EMPTY)

# A recipe's way to stop with a message
HX_DISC_FAIL:= fail() { echo "disc.mk: $$1" >&2; exit 1; }

# What the assembler and xorriso would take and get wrong, or refuse in words of their own
define HX_DISC_CHECKS
$(HX_DISC_FAIL); LC_ALL=C; \
$(foreach name,$(HX_IP_TEXTS),case '$($(name))' in (*[![:print:]]*) \
    fail '$(name) holds a character that is not printable ASCII.';; esac; ) \
case '$(HX_IP_VERSION)' in V[0-9].[0-9][0-9][0-9]) ;; *) \
    fail 'HX_IP_VERSION is $(HX_IP_VERSION). It is as V1.000.';; esac; \
case '$(HX_IP_DATE)' in [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]) ;; *) \
    fail 'HX_IP_DATE is $(HX_IP_DATE). It is YYYYMMDD.';; esac; \
test "$$(date -u -d '$(HX_IP_DATE)' +%Y%m%d 2> /dev/null)" = '$(HX_IP_DATE)' \
    && test '$(HX_IP_DATE)' -gt 19700101 \
    || fail 'HX_IP_DATE is $(HX_IP_DATE). It is a day of the calendar after 1970-01-01.'; \
case '$(HX_IP_AREAS)' in ''|*[!$(subst $(HX_SPACE),,$(HX_BIOS_AREA_LETTERS))]*) \
    fail 'HX_IP_AREAS is $(HX_IP_AREAS). It takes letters of $(HX_BIOS_AREA_LETTERS).';; esac; \
case '$(HX_DISC_VOLUME)' in *[!A-Z0-9_]*) \
    fail 'HX_DISC_VOLUME is $(HX_DISC_VOLUME). It takes A to Z, 0 to 9 and _.';; esac; \
$(if $(HX_DISC_FILES), \
    test -d '$(HX_DISC_FILES)' || fail 'HX_DISC_FILES names no folder ($(HX_DISC_FILES)).'; \
    if ls -A '$(HX_DISC_FILES)' | grep -q -i '^0\(\..*\)\?$$'; then \
        fail 'HX_DISC_FILES holds a name that comes ahead of 0.BIN (the 1st Read file).'; fi;)
endef

# The program's first rule stays its default goal
HX_GOAL_BEFORE:= $(.DEFAULT_GOAL)

.PHONY: hx-disc-always

$(HX_IP_DIR):
	mkdir -p $@

# Checked on every run, and written anew when a value is another than last time. So a value
# given on make's command line builds what it goes into.
$(HX_IP_DIR)/settings: hx-disc-always | $(HX_IP_DIR)
	@$(HX_DISC_CHECKS)
	@printf '%s\n' $(foreach name,$(HX_DISC_SETTINGS),'$(name)=$($(name))') > $@.new
	@cmp -s $@.new $@ && rm $@.new || mv $@.new $@

# Cut and checked on every run, so no build goes without the BIOS and no cut is taken on trust.
# The two files change when their bytes do.
$(HX_IP_DIR)/security.bin $(HX_IP_DIR)/areas_all.bin &: hx-disc-always | $(HX_IP_DIR)
	@$(HX_DISC_FAIL); \
	test -f '$(HX_BIOS)' || fail 'HX_BIOS names no file ($(HX_BIOS)). '\
	'It is the BIOS of your Saturn, which the Security Code is cut from.'; \
	dd if='$(HX_BIOS)' of=$(HX_IP_DIR)/security.new status=none \
	    iflag=skip_bytes,count_bytes $(HX_BIOS_SECURITY) \
	&& dd if='$(HX_BIOS)' of=$(HX_IP_DIR)/areas_all.new status=none \
	    iflag=skip_bytes,count_bytes $(HX_BIOS_AREAS) \
	&& printf '%s  %s\n' \
	    $(HX_BIOS_SECURITY_SHA256) $(HX_IP_DIR)/security.new \
	    $(HX_BIOS_AREAS_SHA256) $(HX_IP_DIR)/areas_all.new \
	    | sha256sum --check --status \
	|| { rm -f $(HX_IP_DIR)/security.new $(HX_IP_DIR)/areas_all.new; \
	    fail 'HX_BIOS ($(HX_BIOS)) does not hold the Security Code and the Area Code blocks '\
	'where v1.01a has them. No disc is built from it.'; }; \
	for name in security areas_all; do \
	    cmp -s $(HX_IP_DIR)/$$name.new $(HX_IP_DIR)/$$name.bin \
	        && rm $(HX_IP_DIR)/$$name.new \
	        || mv $(HX_IP_DIR)/$$name.new $(HX_IP_DIR)/$$name.bin; \
	done

# A change to a makefile builds the Initial Program anew, as a changed setting does. A
# compiler's dependency file is no makefile for this.
HX_DISC_MAKEFILES:= $(filter-out %.d,$(MAKEFILE_LIST))

# The blocks of the disc's areas, in the BIOS's order
$(HX_IP_DIR)/areas.bin: $(HX_IP_DIR)/areas_all.bin $(HX_IP_DIR)/settings $(HX_DISC_MAKEFILES)
	@index=0; for letter in $(HX_BIOS_AREA_LETTERS); do \
	    case '$(HX_IP_AREAS)' in *$$letter*) \
	        dd if=$< bs=32 skip=$$index count=1 status=none;; esac; \
	    index=$$((index + 1)); \
	done > $@

# Linked at the address the BIOS loads it to, 0x06002000, and entered at the Security Code. The
# file is the one section's bytes. The link keeps no symbols: one would name a temporary file.
$(HX_IP_DIR)/IP.BIN: $(HX_ROOT)/build/ip.sx $(HX_IP_DIR)/security.bin $(HX_IP_DIR)/areas.bin \
    $(HX_IP_DIR)/settings $(HX_DISC_MAKEFILES)
	$(HX_CC) -m2 -mb -nostdlib -Wa,--fatal-warnings -Wa,-I$(HX_IP_DIR) $(HX_IP_DEFINES) \
	    -Wl,-Ttext=0x06002000 -Wl,-e,0x06002100 -Wl,-s -o $(HX_IP_DIR)/ip.elf $<
	$(HX_OBJCOPY) -O binary -j .text $(HX_IP_DIR)/ip.elf $@

# ISO 9660 level 1 with nothing added, the Initial Program in the system area. Every date is the
# Release Date, so the same files give the same image. A file xorriso would leave out stops it.
# The 1st Read file is named last, so it is 0.BIN whatever the folder holds.
$(HX_DISC).iso: $(HX_IP_DIR)/IP.BIN $(HX_DISC_FIRST) $(HX_IP_DIR)/settings \
    $(if $(HX_DISC_FILES),hx-disc-always)
	@mkdir -p $(@D)
	xorriso -abort_on WARNING -as mkisofs -quiet -iso-level 1 --norock \
	    -sysid 'SEGA SEGASATURN' -volid '$(HX_DISC_VOLUME)' \
	    -publisher '' -preparer '' -appid '' \
	    --modification-date=$(HX_IP_DATE)00000000 \
	    --set_all_file_dates $(HX_IP_DATE)00000000 \
	    -G $(HX_IP_DIR)/IP.BIN -graft-points -o $@ \
	    $(if $(HX_DISC_FILES),/='$(HX_DISC_FILES)') 0.BIN='$(HX_DISC_FIRST)'
	truncate --size=">$(HX_DISC_DATA_LEAST)" $@

$(HX_DISC).silence.bin:
	@mkdir -p $(@D)
	truncate --size=$(HX_DISC_SILENCE) $@

# The data track, then the audio track with its pause as index 0
$(HX_DISC).cue: $(HX_DISC_MAKEFILES)
	@mkdir -p $(@D)
	printf '%s\n' \
	    'FILE "$(notdir $(HX_DISC)).iso" BINARY' \
	    '  TRACK 01 MODE1/2048' \
	    '    INDEX 01 00:00:00' \
	    'FILE "$(notdir $(HX_DISC)).silence.bin" BINARY' \
	    '  TRACK 02 AUDIO' \
	    '    INDEX 00 00:00:00' \
	    '    INDEX 01 00:02:00' > $@

.DEFAULT_GOAL:= $(HX_GOAL_BEFORE)
