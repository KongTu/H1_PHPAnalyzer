#! /bin/zsh -f

# Author     : M. Goettlich
# last update: $Date: 2010/06/17 12:57:25 $
# by         : $Author: kogler $
# This scripts helps to adapt the steering files for the examples 
# to a given version of the H1OO software.
#
# I want to have one steering for each single example because then
# one just has to enter the program name and everything works.

# In case of new examples you have to modify this script and add them.

# You may have to adapt the following two lines:
DIRMODSHAT=/acs/data/oo-4.0/04
OOVERSION=4.0.6
DSTTYPE=dst7

MODS=$DIRMODSHAT/mods.$OOVERSION.10$DSTTYPE.c040040.root
HAT=$DIRMODSHAT/hat.$OOVERSION.10$DSTTYPE.c040040.root
LIST=$H1DIST/eventlists/list.$OOVERSION.$DSTTYPE.04.acs.d03-04.root;

echo "Will create steerings for the examples with the following settings:"
echo "Directory where HAT and MODS files are located: $DIRMODSHAT"
echo "H1OO version of files that are read: $OOVERSION"
echo "The DST type used is: $DSTTYPE"

cat >! dstar_mods.steer <<EOF
H1SteerTree() {
  //fEventListFiles =  "$LIST";
  fHatFiles  = "$HAT";
  fModsFiles  = "$MODS"; 
}
EOF

cat >! clusters_ods.steer <<EOF
H1SteerTree() {
  fHatFiles  = "$HAT";
  fModsFiles  = "$MODS";
}
EOF

cp clusters_ods.steer rerun_rec.steer
cat >> rerun_rec.steer <<EOF
H1SteerBankEvent()
{
  fBankNames = "HEAD,DTRY,DCLU,DCEL,DMIS,FRME,DTRA,WARN,IRWE,IWCR";
  fUseBranches = 0;
}
EOF

cat >! empz_hat.steer <<EOF
H1SteerTree() {
  fHatFiles  = "$HAT";
}
EOF

cat >! write_usertree.steer <<EOF
// This is an example steering file to produce a usertree using the program
// 'oosubset'.
// Note that it is possible to make a HAT selection: Then the subset of MODS
// and HAT as well as the produced USER tree contain only events passing the
// selection.
// The filling class (here "UserFiller") may access ODS which is loaded
// on-the-fly then, as usual.
//
// Usage:
// oosubset -f write_usertree.steer -n 100
// The option '-n 100' is of course optional and stops after 100 events.
// 

H1SteerTree() 
{
  fHatFiles  = "$HAT";
  fModsFiles  = "$MODS";
  fHatSelection = "Q2e > 2";
}

H1SteerOosubset()
{
  fOutputHat  = "hat.usertreesubset.root";   // HAT  file to write
  fOutputMods = "mods.usertreesubset.root";  // MODS file to write

  fOutputUserFill = "user.usertreesubset.root"; // USER file to create

  fUserLibs = "libUser.so"; // shared library containing classes UserEvent and UserFiller

  fUserTreeName   = "USER";       // name given to usertree
  fUserEventClass = "UserEvent";  // name of class defining user tree data structure
  fUserFillerClass= "UserFiller"; // name of class to fill user tree
}
EOF

cp dstar_mods.steer jpsi_mods.steer
cp clusters_ods.steer kaonfind_ods.steer
cp clusters_ods.steer copyMyEvents.steer
cp clusters_ods.steer create_eventlist.steer
cp clusters_ods.steer write_eventlist.steer
cp clusters_ods.steer dbaccess.steer
cp clusters_ods.steer read_ods.steer
cp clusters_ods.steer rerun_finder.steer
cp clusters_ods.steer boosted_jets.steer
cp empz_hat.steer l1te_hat.steer

echo 'Steering files created ...'
echo 'dstar_mods.steer looks like this:'
cat dstar_mods.steer
echo 'clusters_ods.steer looks like this:'
cat clusters_ods.steer
echo 'empz_hat.steer looks like this:'
cat empz_hat.steer



