# A prototype Makefile for H1
#
# last update: $Date: 2010/10/08 13:39:31 $
# by         : $Author: msteder $
#
# The name of this module
MODULENAME = H1Examples
#
# macro definitions
# DEFINES = 
#
# debug on or off (comment next line to disable debugger information)
DEBUG = -g 
#
# The shared library to make (if any): e.g. libH1Tracks.so
# SLIB_TARGETS = 
SLIB = libUser.so
#
# The static library to make (if any): e.g. libH1Tracks.a
# LIB_TARGETS = 
LIB = 
#
# The binaries you would like to make, you have to provide the final linking
BIN = clusters_ods dstar_mods jpsi_mods kaonfind_ods read_ods read_usertree \
rerun_finder steermanager empz_hat l1te_hat  \
lumicalc \
copyMyEvents write_eventlist create_eventlist read_eventlist \
test_binning cintsteering dbaccess \
boosted_jets

ifeq ($(STANDALONE),yes)
  # no rerun_rec
else
  BIN += rerun_rec 
endif

#
# Objects you need for the libraries
OBJS = UserEvent.o UserFiller.o

# OBJS += UserEvent.o UserFiller.o

# Dictionary objects, default is: DICTOBJS = $(OBJS:.o=Dict.o)

# Now your special libs and includes
# EXTRA_INCLUDEDIRS = ~/my_favourite_library/include
# EXTRA_LIBDIRS = ~/my_favourite_library/lib
# EXTRA_LINKFLAGS = -lmy_favourite_library
# Dummy objects
# DUMMYOBJS =

# Dummy dictionary objects
# DUMMYDICTOBJS = $(DUMMYOBJS:.o=Dict.o)

#extra files to be removed when calling 'make clean' or 'make clobber'
EXTRA_CLEAN_RULES=TLorentzVectorPtr.C
EXTRA_DISTCLEAN_RULES=TLorentzVectorPtr.h

# defines LIBS
# defines Rules for .c .C .cpp .f Dict.C .so
ifeq ($(H1BASICMK),local)
  include  ../oo_tools/H1Basic.mk
else
  include  $(H1DIST)/releases/$(H1CURRENT)/oo_tools/H1Basic.mk
endif                                                             

# include the dependencies if created via 'make depend'
-include Makefile.dep


# the executables


cintsteering: outputdirs $(BUILDDIR)/cintsteering.o
	@echo ""
	@echo Linking example for the use of the steering class H1CintSteer: $@ ...
	$(LOADER) \
		$(BUILDDIR)/cintsteering.o \
		\
		$(OO_LIBS) \
		-lH1Steering \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/cintsteering
	@echo $@ created ...
	@echo ""

clusters_ods: outputdirs $(BUILDDIR)/clusters_ods.o
	@echo ""
	@echo Linking test program for ods access: $@ ...
	$(LOADER) \
		$(BUILDDIR)/clusters_ods.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1OOBanks -lH1Skeleton -lH1Tracks -lH1Clusters \
		-lH1HadronicCalibration -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Geom -lH1Arrays -lH1Tools -lH1Pointers -lH1Steering -lH1Benchmarks \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/clusters_ods
	@echo $@ created ...
	@echo ""

create_eventlist: outputdirs $(BUILDDIR)/create_eventlist.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
	$(BUILDDIR)/create_eventlist.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters\
		-lH1OOBanks -lH1Geom -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1Arrays -lH1Tools  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/create_eventlist
	@echo $@ created ...
	@echo ""

dstar_mods: outputdirs $(BUILDDIR)/dstar_mods.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
		$(BUILDDIR)/dstar_mods.o  \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering \
                \
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/dstar_mods
	@echo $@ created ...
	@echo ""

jpsi_mods: outputdirs $(BUILDDIR)/jpsi_mods.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
	$(BUILDDIR)/jpsi_mods.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering -lH1Benchmarks \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/jpsi_mods
	@echo $@ created ...
	@echo ""

rerun_rec: outputdirs $(BUILDDIR)/rerun_rec.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
	$(BUILDDIR)/rerun_rec.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks \
		-lH1Clusters -lH1OOBanks  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1HadronicCalibration \
		\
		$(H1LIBS) $(LD_FLAGS) -L$(CERN_LIBS) $(LD_SHORT_FLAGS) \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/rerun_rec
	@echo $@ created ...
	@echo ""

kaonfind_ods: outputdirs $(BUILDDIR)/kaonfind_ods.o
	@echo ""
	@echo Linking physics test program for ods access: $@ ...
	$(LOADER) \
		$(BUILDDIR)/kaonfind_ods.o  \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters \
		-lH1OOBanks -lH1Geom -lH1Arrays -lH1Tools -lH1Pointers -lH1Steering \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1MagfieldOO \
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/kaonfind_ods
	@echo $@ created ...
	@echo ""

empz_hat: outputdirs  $(BUILDDIR)/empz_hat.o
	@echo ""
	@echo Linking physics test program for hat contents: $@ ...
	$(LOADER) \
		$(BUILDDIR)/empz_hat.o \
		\
		$(OO_LIBS) \
		-lH1Hat -lH1Skeleton -lH1Pointers -lH1Benchmarks \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Steering  \
		-lH1Clusters -lH1OOBanks \
		-lH1Mods -lH1Tracks  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/empz_hat
	@echo $@ created ...
	@echo ""

l1te_hat: outputdirs  $(BUILDDIR)/l1te_hat.o
	@echo ""
	@echo Linking physics test program for hat contents: $@ ...
	$(LOADER) \
		$(BUILDDIR)/l1te_hat.o \
		\
		$(OO_LIBS) \
		-lH1Hat -lH1Skeleton -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1Tools -lH1Geom -lH1Arrays -lH1OOBanks -lH1Clusters \
		-lH1Mods -lH1Tracks  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/l1te_hat
	@echo $@ created ...
	@echo ""

write_eventlist: outputdirs $(BUILDDIR)/write_eventlist.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
	$(BUILDDIR)/write_eventlist.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/write_eventlist
	@echo $@ created ...
	@echo ""

read_eventlist: outputdirs $(BUILDDIR)/read_eventlist.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
	$(BUILDDIR)/read_eventlist.o \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1Geom -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/read_eventlist
	@echo $@ created ...
	@echo ""


read_usertree: outputdirs slib $(BUILDDIR)/TLorentzVectorPtr.o \
	$(BUILDDIR)/TLorentzVectorPtrDict.o $(BUILDDIR)/read_usertree.o
	@echo ""
	@echo Linking physics test program for reading user tree: $@ ...
	$(LOADER) \
		$(BUILDDIR)/read_usertree.o \
		$(BUILDDIR)/TLorentzVectorPtr.o $(BUILDDIR)/TLorentzVectorPtrDict.o \
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Geom  -lH1Arrays -lH1Tools  -lH1Pointers -lH1Steering -lH1Benchmarks -lUser \
		\
		$(ROOTLIBS) -lMinuit -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/read_usertree
	@echo $@ created ...
	@echo ""

read_ods: outputdirs \
	$(BUILDDIR)/read_ods.o 
	@echo ""
	@echo Linking test program for reading ods tree: $@ ...
	$(LOADER) \
		$(BUILDDIR)/read_ods.o  \
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Skeleton  -lH1Ods  -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1Pointers -lH1Steering -lH1Geom -lH1Arrays -lH1Tools  -lH1Benchmarks \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/read_ods
	@echo $@ created ...
	@echo ""

rerun_finder: outputdirs  $(BUILDDIR)/rerun_finder.o
	@echo ""
	@echo Linking $@ ...
	$(LOADER) \
		$(BUILDDIR)/rerun_finder.o  \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters -lH1OOBanks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1MuonFinder -lH1TrkFinder -lH1Finder -lH1Selection \
		-lH1Geom -lH1SubDetInfo -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1PartEmFinder -lH1ElecCalibration -lH1Arrays -lH1HfsFinder \
		-lH1Tools -lH1SoftLeptonId \
		-lH1MagfieldOO \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
	-o $(BINDIR)/rerun_finder
	@echo $@ created ...
	@echo ""

steermanager: outputdirs $(BUILDDIR)/steermanager.o 
	@echo
	@echo Linking $@ ...
	$(LOADER) \
		$(BUILDDIR)/steermanager.o \
		\
		$(OO_LIBS) \
		-lH1Skeleton -lH1Geom -lH1Arrays -lH1Tools  -lH1Steering  \
		-lH1Clusters -lH1OOBanks -lH1Pointers \
		-lH1Mods -lH1Tracks  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		-lH1HadronicCalibration \
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
	-o $(BINDIR)/steermanager
	@echo $@ created ...
	@echo ""

copyMyEvents: outputdirs $(BUILDDIR)/copyMyEvents.o
	@echo ""
	@echo Linking test program for mods access: $@ ...
	$(LOADER) \
		$(BUILDDIR)/copyMyEvents.o  \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters\
		-lH1OOBanks -lH1Geom -lH1Arrays -lH1Tools -lH1Pointers -lH1Steering -lH1Benchmarks \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/copyMyEvents
	@echo $@ created ...
	@echo ""

dbaccess: outputdirs $(BUILDDIR)/dbaccess.o
	@echo ""
	@echo Linking test program for data base access: $@ ...
	$(LOADER) \
		$(BUILDDIR)/dbaccess.o  \
		\
		$(OO_LIBS) \
		-lH1Mods -lH1Hat -lH1Ods -lH1Skeleton -lH1Tracks -lH1Clusters\
		-lH1OOBanks -lH1Geom -lH1Arrays -lH1Tools -lH1Pointers -lH1Steering \
		-lH1HadronicCalibration  -lH1PhysUtils -lH1JetFinder -lH1Finder \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/dbaccess
	@echo $@ created ...
	@echo ""

lumicalc: outputdirs $(BUILDDIR)/lumicalc.o
	@echo ""
	@echo Linking test program for lumi treatment: $@ ...
	$(LOADER) \
		$(BUILDDIR)/lumicalc.o  \
		\
		$(OO_LIBS) \
		-lH1Arrays -lH1Tools -lH1Geom -lH1Skeleton -lH1Pointers -lH1OOBanks -lH1Steering \
		-lH1Mods -lH1Tracks  -lH1PhysUtils -lH1JetFinder -lH1Finder -lH1Clusters\
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/lumicalc
	@echo $@ created ...
	@echo ""

test_binning: outputdirs $(BUILDDIR)/test_binning.o  
	@echo ""
	@echo Linking test program for H1Binning $@ ...
	$(LOADER) \
		$(BUILDDIR)/test_binning.o \
		\
		$(OO_LIBS) \
		-lH1Geom -lH1Ods -lH1OOBanks -lH1Tracks -lH1Clusters -lH1JetFinder -lH1Finder \
		-lH1Mods -lH1Hat -lH1Skeleton -lH1Steering -lH1Calculator -lH1CalcWeights -lH1CalcPointers \
		-lH1Pointers -lH1Arrays -lH1Tools -lH1QCDFunc -lH1ElecCalibration -lH1PhysUtils \
		-lH1Benchmarks -lH1Arrays -lH1Binning \
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-L$(H1_LIBS) -l${LHAPDFLIB} \
		\
		-o $(BINDIR)/test_binning
	@echo $@ created ...
	@echo ""

boosted_jets: outputdirs $(BUILDDIR)/boosted_jets.o  
	@echo ""
	@echo Linking test program for H1Binning $@ ...
	$(LOADER) \
		$(BUILDDIR)/boosted_jets.o \
		\
		$(OO_LIBS) \
		-lH1Geom -lH1Ods -lH1OOBanks -lH1Tracks -lH1Clusters \
		-lH1Mods -lH1Hat -lH1Skeleton -lH1Steering \
		-lH1Pointers -lH1Arrays -lH1Tools  \
		-lH1Benchmarks -lH1Arrays -lH1Binning \
		-lH1PhysUtils  -lH1JetFinder -lH1Finder \
		-lH1HadronicCalibration \
		\
		$(ROOTLIBS) -lEG -l${FORTRANLIB} \
		\
		-o $(BINDIR)/boosted_jets
	@echo $@ created ...
	@echo ""


# to remove the generated *Ptr.[hC] files:
#clean::
#	rm  *Ptr.[Ch] 2>/dev/null
#	-cd $(BUILDDIR) &&\
#        rm Makefile.dep *.o *~ core *.bak 2>/dev/null

