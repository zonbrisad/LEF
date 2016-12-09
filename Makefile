# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# 
# General Atmel AVR  Makefile template.
#
# author: Peter Malmberg <peter.malmberg@gmail.com>
#
#  
#----------------------------------------------------------------------------
# On command line:
# >make help 
#----------------------------------------------------------------------------

# Target platform
# linux win32 avr arm osx
TARGET_PLATFORM = avr

# Project Licence (GPL, GPLv2, MIT, BSD, Apache, etc.) 
LICENCE = GPLv2

# Target file name (without extension).
TARGET = avrtest

# List C source files here. (C dependencies are automatically generated.)
<<<<<<< HEAD
SRC = src/main.c     \
      src/avrsimul.c \
      src/LEF.c      \
			src/LEF_Led.c  \
			src/LEF_Timer.c \

=======
SRC = src/main.c      \
      src/avrsimul.c  \
      src/LEF.c       \
      src/LEF_Queue.c \
      src/LEF_Led.c   \
      src/LEF_Timer.c \
      src/LEF_Buzzer.c \
      src/LEF_Cli.c    
>>>>>>> 6250a679c206c54b7e6af8ef241fbedde3e10ddb

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC = 

# List Assembler source files here.
#     Make them always end in a capital .S.  
ASRC =

# Include directories
INCLUDE =  src 

LIB  = 

# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
OBJDIR = .

# Output directory
OUTDIR = output

# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
OPT = s

# Compiler flag to set the C Standard level.
# [ gnu99 gnu11 c++98 c++03 c++11 c++14 ] 
CSTANDARD = gnu99

# C Macro definitions
CDEFS = ARDUINO 

# ASM  Macro definitions
ADEFS = 

# C++ Macro definitions
CPPDEFS = 

# Debug information 
# 0=no debug information 
# 1=minimal debug information
#  = normal debug information 
# 3=maximal debug information
DEBUG=0

#
# Compiler and Linker options
#============================================================================

# Compiler Options C --------------------------------------------------------
CFLAGS = -g$(DEBUG)                            # Debugging information
CFLAGS += -O$(OPT)                             # Optimisation level
CFLAGS += -std=$(CSTANDARD)                    # C standard
CFLAGS += $(patsubst %,-I%,$(INCLUDE))         # Include directories 
CFLAGS += $(patsubst %,-D%,$(CDEFS))           # Macro definitions
CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst) # Generate assembler listing

# Compiler Tuning C ---------------------------------------------------------
CFLAGS += -funsigned-char
#CFLAGS += -funsigned-bitfields
#CFLAGS += -fpack-struct
#CFLAGS += -fshort-enums
#CFLAGS += -fno-unit-at-a-time
#CFLAGS += -mshort-calls

# Compiler Warnings C -------------------------------------------------------
CFLAGS += -Wall                  # Standard warnings
CFLAGS += -Wextra                # Some extra warnings
CFLAGS += -Wmissing-braces 
CFLAGS += -Wmissing-declarations # Warn if global function is not declared
CFLAGS += -Wmissing-prototypes   # if a function is missing its prototype
CFLAGS += -Wstrict-prototypes    # non correct prototypes i.e. void fun() => void fun(void) 
CFLAGS += -Wredundant-decls      # Warn if something is declared more than ones
CFLAGS += -Wunreachable-code     # if code is not used
CFLAGS += -Wshadow               # if local variable has same name as global
CFLAGS += -Wformat=2             # check printf and scanf for problems
#CFLAGS += -Wno-format-nonliteral # 
CFLAGS += -Wpointer-arith        # warn if trying to do aritmethics on a void pointer
#CFLAGS += -Wsign-compare
#CFLAGS += -Wundef
#CFLAGS += -Werror               # All warnings will be treated as errors



# Compiler Options C++ ------------------------------------------------------
CPPFLAGS = -g$(DEBUG)                              # Debugging information
CPPFLAGS += -O$(OPT)                               # Optimisation level
CPPFLAGS += -std=$(CSTANDARD)                      # C standard
CPPFLAGS += $(patsubst %,-I%,$(INCLUDE))           # Include directories 
CPPFLAGS += $(patsubst %,-D%,$(CPPDEFS))           # Macro definitions
CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst) # Generate assembler listing

# Compiler Tuning C++ -------------------------------------------------------
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time

# Compiler Warnings C++ -----------------------------------------------------
CPPFLAGS += -Wall                  # Standard warnings
CPPFLAGS += -Wextra                # Some extra warnings
CPPFLAGS += -Wmissing-braces 
CPPFLAGS += -Wmissing-declarations # Warn if global function is not declared
CPPFLAGS += -Wmissing-prototypes   # if a function is missing its prototype
CPPFLAGS += -Wstrict-prototypes    # non correct prototypes i.e. void fun() => void fun(void) 
CPPFLAGS += -Wredundant-decls      # Warn if something is declared more than ones
CPPFLAGS += -Wunreachable-code     # if code is not used
CPPFLAGS += -Wshadow               # if local variable has same name as global
CPPFLAGS += -Wformat=2             # check printf and scanf for problems
#CPPFLAGS += -Wno-format-nonliteral # 
CPPFLAGS += -Wpointer-arith        # warn if trying to do aritmethics on a void pointer
#CPPFLAGS += -Wsign-compare
#CPPFLAGS += -Wundef
#CPPFLAGS += -Werror              # All warnings will be treated as errors



# Assembler Options ---------------------------------------------------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS =  $(patsubst %,-D%,$(ADEFS))
ASFLAGS += -Wa,-adhlns=$(<:%.S=$(OBJDIR)/%.lst),-gstabs,--listing-cont-lines=100

# Linker Options ------------------------------------------------------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(INCLUDE))

#
# Platform specific options
#============================================================================

# Linux options -------------------------------------------------------------
ifeq ($(TARGET_PLATFORM), linux)

# Target filename
TRGFILE=$(OUTDIR)/$(TARGET)

# Toolchain base directory
TCHAIN_BASE=/usr/bin

# Toolchain prefix 
TCHAIN_PREFIX=

endif
# Windows options -----------------------------------------------------------
ifeq ($(TARGET_PLATFORM), win32)

# Target filename
TRGFILE=$(OUTDIR)/$(TARGET).exe

# Toolchain base directory
TCHAIN_BASE=/usr/bin

# Toolchain prefix 
TCHAIN_PREFIX=

endif
# ARM Cortex options --------------------------------------------------------

ifeq ($(TARGET_PLATFORM), arm)

CFLAGS += -mcpu=$(MCU)
CFLAGS += -mthumb
#CFLAGS += -mfpu=fpv4-sp-d16 
#CFLAGS += -mfloat-abi=hard 

LDSCRIPT = stm32_flash.ld
#LDSCRIPT = arm-gcc-link.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=$(MCU) -nostdlib 

# Toolchain base directory
TCHAIN_BASE=/usr/bin
# Toolchain prefix 
TCHAIN_PREFIX=arm-none-eabi-

TRGFILE=$(OUTDIR)/$(TARGET).elf

endif

# Atmel AVR options ---------------------------------------------------------

ifeq ($(TARGET_PLATFORM), avr)
# MCU name
MCU = atmega328

# Processor frequency.
# Some common values:
#  1000000  1843200  2000000 3686400 4000000 7372800 8000000 11059200 14745600
F_CPU = 16000000

CFLAGS   += -mmcu=$(MCU)
CPPFLAGS += -mmcu=$(MCU)
ASFLAGS  += -mmcu=$(MCU)

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2

CDEFS   += F_CPU=$(F_CPU)UL
ADEFS   += F_CPU=$(F_CPU)UL
CPPDEFS += F_CPU=$(F_CPU)UL

# Toolchain base directory
TCHAIN_BASE=/usr/bin
# Toolchain prefix 
TCHAIN_PREFIX=avr-

TRGFILE=$(OUTDIR)/$(TARGET).elf

# Library Options ------------------------------------------------------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS = 

LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

endif

#
# Tool settings
#============================================================================

# Define programs and commands ----------------------------------------------
SHELL     = bash
REMOVE    = rm -f
REMOVEDIR = rm -rf
COPY      = cp -f 
MOVE      = mv -f
WINSHELL  = cmd
MKDIR     = mkdir -p
SED       = sed
MOC       = moc
QMAKE     = qmake
CTEMPLATE = python3 tools/ctemplate.py
BIN2ARRAY = python3 tools/bin2array.py
MPTOOL    = tools/mkptools
CPPCHECK  = cppcheck
INSTALL   = install

TCHAIN = $(TCHAIN_BASE)/$(TCHAIN_PREFIX)

CC        = ${TCHAIN}gcc
OBJCOPY   = ${TCHAIN}objcopy
OBJDUMP   = ${TCHAIN}objdump
SIZE      = ${TCHAIN}size
AR        = ${TCHAIN}ar rcs
NM        = ${TCHAIN}nm
AS        = ${TCHAIN}as

#
# Message/Filter settings
#============================================================================

# Color definitions ---------------------------------------------------------
E_BLACK        = \033[0;300m
E_RED          = \033[0;31m
E_GREEN        = \033[0;32m
E_YELLOW       = \033[0;33m
E_BLUE         = \033[0;34m
E_MAGENTA      = \033[0;35m
E_CYAN         = \033[0;36m
E_GRAY         = \033[0;37m
E_DARKGRAY     = \033[1;30m
E_BR_RED       = \033[1;31m
E_BR_GREEN     = \033[1;32m
E_BR_YELLOW    = \033[1;33m
E_BR_BLUE      = \033[1;34m
E_BR_MAGENTA   = \033[1;35m
E_BR_CYAN      = \033[1;36m
E_WHITE        = \033[1;37m
E_END          = \033[0m
E_ON_BLACK     = \033[40m
E_ON_RED       = \033[41m
E_ON_GREEN     = \033[42m
E_ON_YELLOW    = \033[43m
E_ON_BLUE      = \033[44m
E_ON_MAGENTA   = \033[45m
E_ON_CYAN      = \033[46m
E_ON_WHITE     = \033[47m

C_OK=$(E_BR_GREEN)
C_ERROR=$(E_BR_RED)
C_WARNING=$(E_BR_YELLOW)
C_FILE=$(E_BR_CYAN)
C_NOTE=$(E_BR_GREEN)
C_MSG=$(E_BR_GREEN)
C_ACTION=$(E_BR_MAGENTA)

# Messages ------------------------------------------------------------------
MSG_LINE             = "$(E_WHITE)------------------------------------------------------------------$(E_END)"
MSG_BEGIN            = "${E_WHITE}-------------------------------- Begin ---------------------------${E_END}"
MSG_END              = "${E_WHITE}-------------------------------- End -----------------------------${E_END}"
MSG_ERRORS_NONE      = "${C_OK}Errors: none ${E_END}"
MSG_LINKING          = "${C_ACTION}Linking:${E_END}"
MSG_COMPILING        = "${C_ACTION}Compiling C:${E_END}"
MSG_COMPILING_CPP    = "${C_ACTION}Compiling C++:${E_END}"
MSG_ASSEMBLING       = "${C_ACTION}Assembling:${E_END}"
MSG_CLEANING         = "$(C_ACTION)Cleaning project:$(E_END)"
MSG_EXTENDED_LISTING = "${C_ACTION}Creating Extended Listing/Disassembly:$(E_END)"
MSG_SYMBOL_TABLE     = "${C_ACTION}Creating Symbol Table:$(E_END)"
MSG_HEX_FILE         = "${C_ACTION}Creating Hex file:$(E_END)"
MSG_FORMATERROR      = "${C_ERROR}Can not handle output-format${E_END}"
MSG_ASMFROMC         = "${C_ACTION}Creating asm-File from C-Source:$(E_END)"
MSG_SIZE_BEFORE      = "${C_ACTION}Size before:${E_END}"
MSG_SIZE_AFTER       = "${C_ACTION}Size after build:${E_END}"
MSG_LOAD_FILE        = "${C_ACTION}Creating load file:${E_END}"
MSG_ARCHIVING        = "${C_ACTION}Creating tar archive:${E_END}"
MSG_CREATING_LIBRARY = "${C_ACTION}Creating library:${E_END}"
MSG_FLASH            = "${C_ACTION}Creating load file for Flash:${E_END}"
MSG_EEPROM           = "${C_ACTION}Creating load file for EEPROM:${E_END}"
MSG_COFF             = "${C_ACTION}Converting to AVR COFF:${E_END}"
MSG_EXTENDED_COFF    = "${C_ACTION}Converting to AVR Extended COFF:${E_END}"
MSG_BACKUP           = "${E_BR_GREEN}Making incremental backup of project:${E_END}"
MSG_SRC              = "${C_MSG}Source files $(E_GREEN)-----------------------------------------------------${E_END}"
MSG_FLAGS            = "${C_MSG}Compiler Flags $(E_GREEN)---------------------------------------------------${E_END}"
MSG_LINKER           = "${C_MSG}Linker Flags $(E_GREEN)-----------------------------------------------------${E_END}"
MSG_PROJECT          = "${C_MSG}Project info $(E_GREEN)-----------------------------------------------------${E_END}"
MSG_INCLUDES         = "${C_MSG}Include directories $(E_GREEN)----------------------------------------------${E_END}"
MSG_DEFS             = "${C_MSG}Macro definitions $(E_GREEN)------------------------------------------------${E_END}"
MSG_INSTALL_INFO     = "${C_MSG}Install settings $(E_GREEN)-------------------------------------------------${E_END}"
MSG_INSTALLING       = "${C_ACTION}Installing:   ${E_END}"
MSG_BUILDING         = "$(C_ACTION)Building:     "
	
# Compiler output colorizer filter ------------------------------------------

F_INF="s/In function/$$(printf "$(E_BR_GREEN)")&$$(printf "$(E_END)")/i"
FF_INF="s/^.*In function/$$(printf "$(C_FILE)")&$$(printf "$(E_END)")/i"
F_ERROR="s/error:/$$(printf "$(C_ERROR)")&$$(printf "$(E_END)")/i"
F_FATAL_ERROR="s/fatal error:/$$(printf "$(C_ERROR)")&$$(printf "$(E_END)")/i"
F_WARNING="s/warning:/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_NOTE="s/note:/$$(printf "$(C_NOTE)")&$$(printf "$(E_END)")/i"
FF_ERROR="s/^.*error:/$$(printf "$(C_FILE)")&$$(printf "$(E_END)")/i"
FF_WARNING="s/^.*warning:/$$(printf "$(C_FILE)")&$$(printf "$(E_END)")/i"
FF_NOTE="s/^.*note:/$$(printf "$(C_FILE)")&$$(printf "$(E_END)")/i"
F_WARNING1="s/defined but not used/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_WARNING2="s/unused variable/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_WARNING3="s/may be used uninitialized in this function/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_WARNING4="s/implicit declaration of function/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_WARNING5="s/value computed is not used/$$(printf "$(C_WARNING)")&$$(printf "$(E_END)")/i"
F_BRACKET="s/\[-W.*\]/$$(printf "$(E_GREEN)")&$$(printf "$(E_END)")/g"
#XX="s/\[[^][]*\]/$$(printf "$(E_BR_BLUE)")&$$(printf "$(E_END)")/i"
#YY="s/\'[^][]*\'/$$(printf "$(E_BR_GREEN)")&$$(printf "$(E_END)")/i"
C_FILTER   = | sed -ru -e $(F_FATAL_ERROR) -e $(F_ERROR) -e $(FF_ERROR)     \
                       -e $(F_WARNING) -e $(FF_WARNING)                     \
                       -e $(F_WARNING1) -e $(F_WARNING2) -e $(F_WARNING3)   \
                       -e $(F_WARNING4) -e $(F_WARNING5)                    \
                       -e $(F_NOTE) -e $(FF_NOTE)                           \
                       -e $(F_INF) -e $(FF_INF) -e $(F_BRACKET)
CPP_FILTER = $(C_FILTER)

LD_ERROR1="s/undefined reference/$$(printf "$(C_ERROR)")&$$(printf "$(E_END)")/i"
LD_ERROR2="s/No such file or directory/$$(printf "$(C_ERROR)")&$$(printf "$(E_END)")/i"
LD_FILTER = | sed -ru -e $(LD_ERROR1) -e $(LD_ERROR2)
	
#
# Build rules	
#============================================================================

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS   =  -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS =  -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS  =  -I. -x assembler-with-cpp $(ASFLAGS)

# Define all object files.
OBJS = $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o)

# Define all listing files.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst) 

# Default target.
all:	begin build finished end

# Remove colorfilter
nocolor:
	$(eval C_FILTER:= )
	
#
# Build with no color filter on compiler output
# Useful for Netbeans, Eclipse builds etc. 
#	
nc: nocolor all   ## Build with no color on compiler output

build: elf hex eep lss sym sizeafter


elf: $(TRGFILE)
lss: $(OUTDIR)/$(TARGET).lss
sym: $(OUTDIR)/$(TARGET).sym
hex: $(OUTDIR)/$(TARGET).hex
bin: $(OUTDIR)/$(TARGET).bin
eep: $(OUTDIR)/$(TARGET).eep

# Eye candy.
begin:
	@echo -e $(MSG_BEGIN)
	@echo -e ${MSG_BUILDING}"$(E_BR_GREEN)$(TARGET) $(E_END)"
 
end:
	@echo
	@echo -e $(MSG_END)
	
finished:
	@echo
#	@echo -e $(MSG_ERRORS_NONE)


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(OUTDIR)/$(TARGET).hex
ELFSIZE = $(SIZE) --mcu=$(MCU) --format=avr $(TRGFILE)

sizebefore:
	@if [ -f $(OUTDIR)/$(TARGET).elf ]; then echo; echo -e $(MSG_SIZE_BEFORE); $(ELFSIZE); \
 	2>/dev/null; echo; fi

sizeafter: $(TRGFILE)
	@echo
	@echo -e $(MSG_SIZE_AFTER) 
	@$(ELFSIZE); 



# Display compiler version information.
gccversion :  ## Display compiler version
	@$(CC) --version

# Link target
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJS)
%.elf: $(OBJS) $(OUTDIR)
	@echo 
	@echo -e $(MSG_LINKING)"\n             $(C_FILE)" $@ "$(E_END)"
	@$(CC) $(ALL_CFLAGS) $(OBJS) --output $@ $(LDFLAGS) $(LIB) 2>&1 $(LD_FILTER)
	
# Create extended listing file/disassambly from ELF output file.
# using objdump testing: option -C
%.lss:	$(TRGFILE)
	@echo
	@echo -e $(MSG_EXTENDED_LISTING) "\n             $(C_FILE)" $@ "$(E_END)"
	@$(OBJDUMP) -h -S -z $< > $@
	
# Create a symbol table from ELF output file.
%.sym: $(TRGFILE)
	@echo
	@echo -e $(MSG_SYMBOL_TABLE) "\n             $(C_FILE)" $@ "$(E_END)"
	@$(NM) -n $< > $@

# Create hex file from ELF output file.
%.hex: %.elf
	@echo
	@echo -e $(MSG_HEX_FILE) "\n             $(C_FILE)" $@ "$(E_END)"
	@$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature $< $@

coff: $(TARGET).elf
	@echo
	@echo -e $(MSG_COFF)"\n             $(E_BR_CYAN)" $(TARGET).cof "$(E_END)"
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof

extcoff: $(TARGET).elf
	@echo
	@echo -e $(MSG_EXTENDED_COFF)"\n             $(E_BR_CYAN)" $(TARGET).cof "$(E_END)"
	@$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

%.eep: %.elf
	@echo
	@echo -e $(MSG_EEPROM)"\n             $(E_BR_CYAN)" $@ "$(E_END)"
	@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0




# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo -e $(MSG_COMPILING) "$(C_FILE)" $< "$(E_END)"
	@$(CC) -c $(ALL_CFLAGS) $< -o $@ 2>&1  $(C_FILTER)

# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo
	@echo -e $(MSG_COMPILING_CPP) "$(C_FILE)" $< "$(E_END)"
	@$(CC) -c $(ALL_CPPFLAGS) $< -o $@ 2>&1 | $(CPP_FILTER)
	
# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S
	@echo
	@echo -e $(MSG_ASSEMBLING) "$(C_FILE)" $< "$(E_END)"
	@$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Compile: create assembler files from C source files.
$(OBJDIR)/%.s : %.c
	@$(CC) -S $(ALL_CFLAGS) $< -o $@

# Compile: create assembler files from C++ source files.
$(OBJDIR)/%.s : %.cpp
	@$(CC) -S $(ALL_CPPFLAGS) $< -o $@

# Create output dir
$(OUTDIR):
	@$(MKDIR) $@

size: $(TRGFILE)
	@echo
	@echo -e $(MSG_SIZE_AFTER)
	@$(SIZE) $(TRGFILE)
	
#
# Various utility rules	
#============================================================================


clean:  ## Remove all build files
	@echo
	@echo -e $(MSG_CLEANING)
	@$(REMOVE) $(OUTDIR)/$(TARGET)
	@$(REMOVE) $(OUTDIR)/$(TARGET).elf
	@$(REMOVE) $(OUTDIR)/$(TARGET).hex
	@$(REMOVE) $(OUTDIR)/$(TARGET).lss
	@$(REMOVE) $(OUTDIR)/$(TARGET).map
	@$(REMOVE) $(OUTDIR)/$(TARGET).sym
	@$(REMOVE) $(OUTDIR)/$(TARGET).bin
	@$(REMOVE) $(OUTDIR)/$(TARGET).eep
	@$(REMOVE) $(OUTDIR)/$(TARGET).cof
	@$(REMOVEDIR) .dep
	@$(REMOVE) $(OBJS)
	@find . -name *~ -delete


archive: ## Make a tar archive of the source code
	@echo
	@echo -e $(MSG_ARCHIVING)
	@$(eval DT=$(shell date +"%Y%m%d-%H%M%S"))
	@$(MKDIR) archive
	@tar -cvzf archive/$(TARGET)_${DT}.tar.gz *  \
		--exclude='archive'  \
		--exclude='backup'   \
		--exclude='output'   \
		--exclude='*.a'      \
		--exclude='*.o'      \
		--exclude='*.ko'     \
		--exclude='*.obj'    \
		--exclude='*.a'      \
		--exclude='*.la'     \
		--exclude='*.lo'     \
		--exclude='*.slo'    \
		--exclude='*.lib'    \
		--exclude='*.so'     \
		--exclude='*.so*'    \
		--exclude='.dep'     \
		--exclude='.svn'     \
		--exclude='.git'     \
		--exclude='*.elf'    \
		--exclude='*.hex'    \
		--exclude='*.bin'    \
		--exclude='*.exe'    \
		--exclude='*.sym'    \
		--exclude='*.lss'    \
		--exclude='*.map'    \
		--exclude='*.app'    \
		--exclude='*.i*86'   \
		--exclude='*.x86_64' \
		--exclude='*~'       \
		--exclude="*.old"    \
		--exclude="*.tmp"    \

# Backup directory
BACKUP_DIR=backup

# Nr of backups
BACKUPS=100

backup: ## Make an incremental backup
	@echo
	@echo -e $(MSG_BACKUP)
	@$(MKDIR) $(BACKUP_DIR)
  # remove oldest backup
	@$(RM) -rf $(BACKUP_DIR)/backup_$(BACKUPS) 
  # rotate backups 
	@for ((x=$(BACKUPS);x>0;x--)); do                 \
	  bdir=$(BACKUP_DIR)/backup_`expr $${x} - 1` ;    \
	  # check if directory exist before renameing  it \
	  if [ -d $${bdir} ]; then                        \
	    mv -f $${bdir}  $(BACKUP_DIR)/backup_$${x} ;  \
	  fi ;                                            \
	done 
	@rsync --archive             \
	      --delete               \
				--relative             \
				--exclude="backup"     \
				--exclude="archive"    \
				--exclude="output"     \
				--exclude="*.o"        \
				--exclude=".git"       \
				--exclude=".svn"       \
				--exclude="*~"         \
				--exclude="*.old"      \
				--exclude="*.tmp"      \	
				--link-dest=$(CURDIR)/$(BACKUP_DIR)/backup_1 \
				.                      \
				$(BACKUP_DIR)/backup_0 


edit:   ## Open source and makefile in editor
	@$(EDITOR) Makefile $(SRC)


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

newfile:  ## Create a new C file
	@${CTEMPLATE} --newc --dir src --author "$(AUTHOR)" --licence "$(LICENCE)"
	
newproj:  ## Create a new project
	@${PROJECT} newproj

# Install options -----------------------------------------------------------

# Install directory
INSTALL_DIR      = ~/bin

# Owner of binary
INSTALL_OWNER    = $${USER}

# Group owner of binary
# #INSTALL_GROUP    = $${USER}
INSTALL_GROUP    = users

# Install options
INSTALL_OPTIONS =  --owner ${INSTALL_OWNER}
INSTALL_OPTIONS += --group ${INSTALL_GROUP}
INSTALL_OPTIONS += -D
INSTALL_OPTIONS += --preserve-timestamps
#INSTALL_OPTIONS += --verbose

install: $(TRGFILE) ## Install program
	@echo -e $(MSG_INSTALLING) "$(E_BR_GREEN)$(TARGET) $(E_END)"
	@${INSTALL} ${INSTALL_OPTIONS} $(TRGFILE) ${INSTALL_DIR}

#
# Help information
#============================================================================

help: ## This help information
	@printf "%-30s %s\n" "target" "help"
	@echo -e $(MSG_LINE)
	@IFS=$$'\n' ;                                    \
	help_lines=(`fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##/:/'`); \
	for help_line in $${help_lines[@]}; do                                     \
	  IFS=$$':' ;                                                              \
		help_split=($$help_line) ;                                               \
		help_command=`echo $${help_split[0]} | sed -e 's/^ *//' -e 's/ *$$//'` ; \
		help_info=`echo $${help_split[2]} | sed -e 's/^ *//' -e 's/ *$$//'` ;    \
		printf "$(E_BR_CYAN)%-15s %s$(E_END)" $$help_command ;                 \
		printf "$(E_BR_GREEN)%s$(E_END)\n" $$help_info ;                       \
	done ; 
	@echo -e $(MSG_LINE)

list-src: ## List all source files
	@echo -e $(MSG_SRC)
	@export IFS=" "
	@for f in $(SRC); do      \
	  echo $${f} ;            \
	done                      \
	
list-flags: ## List compiler flags
	@echo -e $(MSG_FLAGS)
	@export IFS=" "
	@for f in $(CFLAGS); do   \
	  echo $${f} ;            \
	done                      \

list-ldflags: ## List linker flags
	@echo -e $(MSG_LINKER)
	@export IFS=" "
	@for f in $(LDFLAGS); do   \
	  echo $${f} ;             \
	done                       \

check: ## Check if tools and libraries are present 
	@$(MPTOOL) ce $(CC)
	@$(MPTOOL) ce $(OBJCOPY)
	@$(MPTOOL) ce $(OBJDUMP)
	@$(MPTOOL) ce $(SIZE)
	@$(MPTOOL) ce $(AR)
	@$(MPTOOL) ce $(NM)
	@$(MPTOOL) ce $(AS)
	@$(MPTOOL) ce $(QMAKE)
	@$(MPTOOL) ce $(MOC)
	@$(MPTOOL) ce python3
	@$(MPTOOL) ce $(CPPCHECK)
	@$(MPTOOL) ce $(INSTALL)
	@for f in $(LIB); do               \
		${MPTOOL} cl ${CC} $${f};      \
	done                               \
	
list-info: 
	@echo -e $(MSG_PROJECT)
	@echo "Target:     $(TARGET)"
	@echo "Platform:   $(TARGET_PLATFORM)"
	@echo "Licence:    $(LICENCE)"
	@echo "Outdir:     $(OUTDIR)"
	@echo "C standard: $(CSTANDARD)"
	@echo "MCU:        $(MCU)"
	@echo "F_CPU:      $(F_CPU)"

	
list-includes: 
	@echo -e $(MSG_INCLUDES)
	@export IFS=" "
	@for f in $(INCLUDE); do   \
	  echo $${f} ;             \
	done        

list-defs: 
	@echo -e $(MSG_DEFS)
	@export IFS=" "
	@for f in $(CDEFS); do     \
	  echo $${f} ;             \
	done        

	@for f in $(CPPDEFS); do   \
	  echo $${f} ;             \
	done        

	@for f in $(ASDEFS); do    \
	  echo $${f} ;             \
	done        


list-installinfo:
	@echo -e $(MSG_INSTALL_INFO)
	@echo "Install dir:   $(INSTALL_DIR)"
	@echo "Install user:  $(INSTALL_USER)"
	@echo "Install group: $(INSTALL_GROUP)"




info: list-info list-includes list-defs list-flags list-ldflags list-installinfo ## Print information about project

files: list-src ## List source files
	
#
# Personal settings
#============================================================================

# Only for default settings. Change value in settings.mk
AUTHOR=Peter Malmberg <peter.malmberg@gmail.com>
EDITOR=jed
US:=$$USER
#PERSONAL=personal_$${USER}.mk
PERSONAL=personal.mk

# Include some external settings
# If file does not exist it will be generated.
include  ${PERSONAL}

$(PERSONAL):	# Create a settings file
	@echo "#" > ${PERSONAL}
	@echo "# This file is for personal settings only." >> ${PERSONAL}
	@echo "#" >> ${PERSONAL}
	@echo "EDITOR=${EDITOR}" >> ${PERSONAL}
	@echo "AUTHOR=${AUTHOR}" >> ${PERSONAL}



#
# CppCheck static code analysis
#============================================================================
.PHONY: ccheck acheck

ccheck: ## Static code analysis using cppcheck(errors only)
	@$(CPPCHECK) --inline-suppr $(SRC) 

acheck: ## Static code analysis using cppcheck(all warnings)
	@$(CPPCHECK) --inline-suppr --enable=all $(SRC) 


#
# Avrdude settings 
# ----------------------------------------------------------
.PHONY: program
	
AVRDUDE = avrdude	
# Programming hardware
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = stk500v2
 
# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com1    # programmer connected to serial device

AVRDUDE_BITRATE = 57600 
 
AVRDUDE_WRITE_FLASH = -U flash:w:$(OUTDIR)/$(TARGET).hex:i
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
  
# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
AVRDUDE_VERBOSE = -v -v
 
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b ${AVRDUDE_BITRATE} -D
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)


# Program the device.
program: $(TARGET).hex # $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) # $(AVRDUDE_WRITE_EEPROM)
 
#
# AVR simulator
#============================================================================
.PHONY: sim

sim:  ## Run AVR simulator 
	simulavr -d $(MCU) -F $(F_CPU) --file $(TRGFILE) -W 0x20,- -R 0x22,- -T exit --irqstatistic
	
# Listing of phony targets.
.PHONY : all clean gccversion build begin finished end elf lss sym archive edit help backup list-src list-flags newproj run install
