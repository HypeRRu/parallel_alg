# project settings
SRCDIR					:= src
INCDIR					:= include
BUILDDIR				:= build
TARGETDIR				:= target
RESDIR					:= assets
SRCEXT					:= cpp
OBJEXT					:= o
DEPEXT					:= d

# compiler settings
CC						:= LANG=en_US g++
OPTIMIZATION			:= -O0 -g3
CXXFLAGS				:= -Wall -Wextra -Wpedantic
STANDART				:= -std=c++11
LIB						:=
INC						:= -I$(INCDIR) -I/usr/local/include
INCDEP					:= -I$(INCDIR)
CXX						:= $(CC) $(OPTIMIZATION) $(CXXFLAGS) $(STANDART)

# common files
COMMON_SRC				:= $(SRCDIR)/common/matrix.cpp
IPC_SRC					:= $(SRCDIR)/ipc/socket.cpp $(SRCDIR)/ipc/unix_socket.cpp $(SRCDIR)/ipc/matrix_algorithms.cpp
THREADS_SRC				:= $(SRCDIR)/threads/tasks.cpp $(SRCDIR)/threads/matrix_algorithms.cpp

# read process files
READ_PROCESS_SRC		:= $(SRCDIR)/ipc/read_proc.cpp $(COMMON_SRC) $(IPC_SRC)
READ_PROCESS_OBJ		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(READ_PROCESS_SRC:.$(SRCEXT)=.$(OBJEXT)))
READ_PROCESS_TARGET		:= $(TARGETDIR)/read_process

#calc process files
CALC_PROCESS_SRC		:= $(SRCDIR)/ipc/calc_proc.cpp $(COMMON_SRC) $(IPC_SRC)
CALC_PROCESS_OBJ		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(CALC_PROCESS_SRC:.$(SRCEXT)=.$(OBJEXT)))
CALC_PROCESS_TARGET		:= $(TARGETDIR)/calc_process

# out process files
WRITE_PROCESS_SRC		:= $(SRCDIR)/ipc/write_proc.cpp $(COMMON_SRC) $(IPC_SRC)
WRITE_PROCESS_OBJ		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(WRITE_PROCESS_SRC:.$(SRCEXT)=.$(OBJEXT)))
WRITE_PROCESS_TARGET	:= $(TARGETDIR)/write_process

# threading files
THREADING_SRC			:= $(SRCDIR)/threads/threads.cpp $(COMMON_SRC) $(THREADS_SRC)
THREADING_OBJ			:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(THREADING_SRC:.$(SRCEXT)=.$(OBJEXT)))
THREADING_TARGET		:= $(TARGETDIR)/threading

# threading extended files
EXTENDED_SRC			:= $(SRCDIR)/threads/threads_extended.cpp $(COMMON_SRC) $(THREADS_SRC)
EXTENDED_OBJ			:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(EXTENDED_SRC:.$(SRCEXT)=.$(OBJEXT)))
EXTENDED_TARGET			:= $(TARGETDIR)/threading_extended

# sources and objects list
SOURCES					:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS					:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))


all: processing threading threading_extended

processing: resources $(READ_PROCESS_TARGET) $(CALC_PROCESS_TARGET) $(WRITE_PROCESS_TARGET)
threading: resources $(THREADING_TARGET)
threading_extended: resources $(EXTENDED_TARGET)

# link objects
$(READ_PROCESS_TARGET): $(READ_PROCESS_OBJ)
	$(CC) -o $(READ_PROCESS_TARGET) $^ $(LIB)

$(CALC_PROCESS_TARGET): $(CALC_PROCESS_OBJ)
	$(CC) -o $(CALC_PROCESS_TARGET) $^ $(LIB)

$(WRITE_PROCESS_TARGET): $(WRITE_PROCESS_OBJ)
	$(CC) -o $(WRITE_PROCESS_TARGET) $^ $(LIB)

$(THREADING_TARGET): $(THREADING_OBJ)
	$(CC) -o $(THREADING_TARGET) $^ $(LIB)

$(EXTENDED_TARGET): $(EXTENDED_OBJ)
	$(CC) -o $(EXTENDED_TARGET) $^ $(LIB)

# remake
remake: cleaner all

# copy resources
resources: directories
	@cp -r $(RESDIR)/* $(TARGETDIR)/$(RESDIR)/

# make directories
directories:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(TARGETDIR)/$(RESDIR)/

# clean only objects
clean:
	@rm -fr $(BUILDDIR)
	@rm -fr $(TARGETDIR)
	@rm -fr $(TARGETDIR)/$(RESDIR)/

# full clean: objects and binaries
cleaner: clean
	@rm -fr $(READ_PROCESS_TARGET)
	@rm -fr $(CALC_PROCESS_TARGET)
	@rm -fr $(WRITE_PROCESS_TARGET)

# set dependency info for .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# compile sources
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@echo $@
	@mkdir -p $(dir $@)
	$(CXX) $(INC) -c $< -o $@
	@$(CXX) $(INCDEP) -MM $< > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$@:|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp