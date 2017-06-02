os = $(shell uname -s)

INCFLAGS      =  -I$(shell root-config --incdir) $(shell fastjet-config --cxxflags)  -I$(shell pythia8-config --includedir) -I$(BOOSTDIR)/include -I$(STARPICODIR) # -I$(BOOSTDIR)/include

ifeq ($(os),Linux)
CXXFLAGS      = -std=c++11
else
CXXFLAGS      = -O -std=c++11 -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
## for debugging:
# CXXFLAGS      = -g -O0 -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
endif

ifeq ($(os),Linux)
LDFLAGS       = -g
LDFLAGSS      = -g --shared
else
LDFLAGS       = -O -Xlinker -bind_at_load -flat_namespace
LDFLAGSS      = -flat_namespace -undefined suppress
LDFLAGSSS     = -bundle
endif

ifeq ($(os),Linux)
CXX          = g++
else
CXX          = clang
endif


ROOTLIBS      = $(shell root-config --libs)
FJLIBS        = $(shell fastjet-config --plugins=yes --libs)
PYTHIALIBS    = $(shell pythia8-config --ldflags)
LIBPATH       = -L$(FASTJETDIR)/lib -L$(STARPICODIR) $(shell root-config --libs)
LIBS          =  $(ROOTLIBS) $(FJLIBS) $(PYTHIALIBS) -lfastjet -lfastjettools -lTStarJetPico

# for cleanup
SDIR          = src
ODIR          = src/obj
BDIR          = bin


###############################################################################
################### Remake when these headers are touched #####################
###############################################################################


###############################################################################
# standard rules
$(ODIR)/%.o : $(SDIR)/%.cxx $(INCS)
	@echo
	@echo COMPILING
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

$(BDIR)/%  : $(ODIR)/%.o
	@echo
	@echo LINKING
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBPATH) $(LIBS)

###############################################################################

###############################################################################
############################# Main Targets ####################################
###############################################################################
all : $(BDIR)/pythia8TrackEfficiency $(BDIR)/pythia6TrackEfficiency $(BDIR)/p6GeantTrackEfficiency

#$(ODIR)/qa_v1.o 		: $(SDIR)/qa_v1.cxx
$(ODIR)/vTrackEfficiency.o : $(SDIR)/vTrackEfficiency.cxx
$(ODIR)/pythia8TrackEfficiency.o : $(SDIR)/pythia8TrackEfficiency.cxx
$(ODIR)/pythia6TrackEfficiency.o : $(SDIR)/pythia6TrackEfficiency.cxx
$(ODIR)/p6GeantTrackEfficiency.o : $(SDIR)/p6GeantTrackEfficiency.cxx

#data analysis
#$(BDIR)/qa_v1		: $(ODIR)/qa_v1.o
$(BDIR)/vTrackEfficiency  : $(ODIR)/vTrackEfficiency.o
$(BDIR)/pythia8TrackEfficiency  : $(ODIR)/pythia8TrackEfficiency.o
$(BDIR)/pythia6TrackEfficiency  : $(ODIR)/pythia6TrackEfficiency.o
$(BDIR)/p6GeantTrackEfficiency  : $(ODIR)/p6GeantTrackEfficiency.o

###############################################################################
##################################### MISC ####################################
###############################################################################

clean :
	@echo
	@echo CLEANING
	rm -vf $(ODIR)/*.o
	rm -vf $(BDIR)/*
	rm -vf lib/*

