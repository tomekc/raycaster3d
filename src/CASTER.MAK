# This file is automaticly generated by RHIDE
CC=gcc
AS=gcc
LD=gcc
AR=ar
ARFLAGS=rcs
ALL_CFLAGS=$(INCLUDE_DIRS) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS) \
	$(C_WARN_FLAGS) $(C_EXTRA_FLAGS) $(CPPFLAGS) \
	$(C_C_LANG_FLAGS) $(CFLAGS)
ALL_CXXFLAGS=$(INCLUDE_DIRS) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS) \
	$(C_WARN_FLAGS) $(CXX_EXTRA_FLAGS) $(CPPFLAGS) $(CXXFLAGS) \
	$(C_CXX_LANG_FLAGS)
ALL_LD_FLAGS=$(LD_EXTRA_FLAGS) $(LD_FLAGS)
ALL_AS_FLAGS=$(AS_EXTRA_FLAGS) $(AS_FLAGS)
INCLUDE_DIRS=
LIB_DIRS=
C_DEBUG_FLAGS=
C_OPT_FLAGS=
C_WARN_FLAGS=
C_C_LANG_FLAGS=
C_CXX_LANG_FLAGS=
LIBS= \
	-lalleg \
	-llzo \
	-liostr
LD_EXTRA_FLAGS=
C_EXTRA_FLAGS=
CXX_EXTRA_FLAGS=
AS_EXTRA_FLAGS=
OBJ_DIR=.
OUTFILE=$(addprefix $(OBJ_DIR)/,$@)
LOCAL_OPT=$(subst ___~~~___, ,$(subst $@___,,$(filter $@___%,$(LOCAL_OPTIONS))))

%.o: %.cpp
	$(CC) $(ALL_CXXFLAGS) -c $< $(LOCAL_OPT) -o $(OUTFILE)


%.s: %.cpp
	$(CC) $(ALL_CXXFLAGS) -S $< $(LOCAL_OPT) -o $(OUTFILE)


%.o: %.cc
	$(CC) $(ALL_CXXFLAGS) -c $< $(LOCAL_OPT) -o $(OUTFILE)


%.s: %.cc
	$(CC) $(ALL_CXXFLAGS) -S $< $(LOCAL_OPT) -o $(OUTFILE)


%.o: %.c
	$(CC) $(ALL_CFLAGS) -c $< $(LOCAL_OPT) -o $(OUTFILE)


%.s: %.c
	$(CC) $(ALL_CFLAGS) -S $< $(LOCAL_OPT) -o $(OUTFILE)

%.o: %.s
	$(AS) $(ALL_AS_FLAGS) -c $< $(LOCAL_OPT) -o $(OUTFILE) $<

all: caster
DEPS_0=\
		ack32.o\
		bitfont.o\
		draw.o\
		game.o\
		objects.o\
		pakfile.o\
		tables.o\
		textures.o
caster: $(DEPS_0)
	$(LD) $(ALL_LD_FLAGS) -o $@  $(addprefix $(OBJ_DIR)/,$(filter %.o,$(DEPS_0))) $(filter-out %.o,$(DEPS_0))\
	$(LIB_DIRS) $(LIBS)
DEPS_1=ack32.obj
ack32.o: $(DEPS_1)
DEPS_2=bitfont.cc
bitfont.o: $(DEPS_2)
DEPS_3=draw.cc
draw.o: $(DEPS_3)
DEPS_4=game.cc defs.h
game.o: $(DEPS_4)
DEPS_5=objects.cc
objects.o: $(DEPS_5)
DEPS_6=pakfile.cc
pakfile.o: $(DEPS_6)
DEPS_7=tables.cc
tables.o: $(DEPS_7)
DEPS_8=textures.cc
textures.o: $(DEPS_8)

force:

