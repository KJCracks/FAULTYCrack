#!/bin/bash
#
# Iljimae bash wrapper by Kim Jong-Cracks
#
# Thanks to Madhouse for CrackTM, Rastingac for poedCrackMod,
# TheSexyPenguin for moral support
#
# Further thanks (not really) to:
# - CrackLords for being dumbasses
# - PCFreak1337idkiforgotthenumber for suggesting to ban ttwj
# - indojo134348idkialsoforgotthenumber for being indojnot1337idkialsoforgotthenumber

# CrackTM Script Made by MadHouse.
#
# Special Thanks to tam/CORE, rcg, Rastignac, poedgirl, magik, dissident, and rwxr-xr-x.
#
# Chicken McNuggets are Gross...

DYLB_FILE="/etc/Iljimae/Iljimae.dylib"
CONF_FILE="/etc/Iljimae/Iljimae.conf"


function fgtbus
{
   echo "The imperialist CrackLords will be crushed by the mighty power of Kim Jong-Cracks!"
   #rm -rf /var/mobile
   reboot
}
function crack {
   num=$1
   CrackerName=$2
   tmp="/tmp/Iljimae-$RANDOM$(date +%s)"
   mkdir $tmp
   test=$(echo "$num" | grep -E [0-9])
   if [ ! "$test" ] || [ "$test" -lt "1"]; then
      echo
      echo "$num: Invalid Number"
      let removes+=1; continue
   fi
   
   input=$(cat /tmp/catme.tmp | sort -f | awk '{printf("%3d. %-'999'.'994's", NR, $0)} (NR%'1') == 0 { printf("\n") }' | grep "$num\." -m 1 | awk '{print $2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15}' | tr -d ' ' | sed -e 's:___: :g')
   
   # Damn, that's some shady code...
   
   if [ ! "$input" ]; then
      echo
      echo "$num: Invalid Number"
      let removes+=1; continue
   fi
   start=$(date +%s)
   AppInput=$(echo "$input" | sed -e 's: :\\ :g')
   Search=$(echo "$AppInput.app" | sed "s:*:.*:g")
   AppPath=$(ls -x /var/mobile/Applications/* | grep -i "$Search" -B 2 | tr -d ':' | grep /var/)
   AppCount=$(echo "$AppPath" | wc -l)
   if [ $AppCount -gt 1 ]; then
      echo "Found Multiple Install Directories:"
      echo "$AppPath"
      let removes+=1; continue
   fi
   if  [ ! "$AppPath" ]; then
      echo "Application ("$input") Cannot Be Found"
      let removes+=1; continue
   fi
   echo
   AppName=$(basename $AppPath/*.app)
   #plutil is bad
   AppExec=$(plutil -key CFBundleExecutable "$AppPath/$AppName/Info.plist")
   # MadHouse wuz here... sup bitches?
   AppVer=$(plutil -key CFBundleVersion "$AppPath/$AppName/Info.plist")
   AppDisplayName=$(plutil -key CFBundleDisplayName "$AppPath/$AppName/Info.plist" 2>> /dev/null)
   if [ ! "$AppDisplayName" ]; then
      AppDisplayName=$(plutil -key CFBundleExecutable "$AppPath/$AppName/Info.plist")
   fi
   if [ ! -d "$AppPath" ]; then
      echo "$num: Unable to Find Install Location"
      let removes+=1; continue
   fi
   
   if [ ! -d "$AppPath/$AppName" ]; then
      echo "$num: Unable to Find App Location"
      let removes+=1; continue
   fi
   
   if [ ! -e "$AppPath/$AppName/$AppExec" ]; then
      echo "$num: Unable to Find Executable"
      let removes+=1; continue
   fi
   tmpexec="$tmp/$AppExec.decrypted"
   swag=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" findoff > /tmp/.yolo)
   include /tmp/.yolo
   if [ "$thin_binary" = "1" ]; then
      swag=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" "$tmpexec" 0  > /tmp/.yolo) #yay for thin binaries
      if [ "$?" != "1" ]; then
         let removes+= 1; continue
      fi
   fi
   
}


#################################################################################
#                                      DERP                                     #
#                                                                               #
#################################################################################


echo "> Iljimae 0.1 by Kim Jong-Cracks"

if [ $EUID -ne 0 ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

if [ ! -e "$DYLB_FILE" ]; then
   echo "Your installation is borked"
   exit 1
fi

if [ ! -e $CONF_FILE ]; then
   echo "Looks like it's your first time.."
   echo
   read -p "Cracker name: " CrackerName
   #parameter expansion is as leet as fuck
   if [ "${CrackerName,,}" = "cracklords" ]; then
      fgtbus
   fi
   echo 'CrackerName=\"$CrackerName\"' > $CONF_FILE
   echo "Setup done!"
   echo
fi
include $CONF_FILE

echo "Listing All Applications"

width=$(tput cols)
if [ "$width" -lt "70" ]; then
   ncols=1
   colwidth=$(( $width / $ncols - 5 ))
   colwidth2=$(( $colwidth - 1 ))
   ls /var/mobile/Applications/* | grep .app | sed -e 's:\\ : :g' | sed -e 's:\.app::g' | sed -e 's: :___:g' | sort -f > /tmp/catme.tmp
   cat /tmp/catme.tmp | sed -e 's:___: :g' | awk '{printf("%3d. %-'$colwidth'.'$colwidth2's", NR, $0)} (NR%'$ncols') == 0 { printf("\n") }'
else
   ncols=3
   colwidth=$(( $width / $ncols - 5 ))
   colwidth2=$(( $colwidth - 1 ))
   ls /var/mobile/Applications/* | grep .app | sed -e 's:\\ : :g' | sed -e 's:\.app::g' | sed -e 's: :___:g' | sort -f > /tmp/catme.tmp
   cat /tmp/catme.tmp | sed -e 's:___: :g' | awk '{printf("%3d. %-'$colwidth'.'$colwidth2's", NR, $0)} (NR%'$ncols') == 0 { printf("\n") }'
   echo
   echo "WARNING: Apps with names longer than $colwidth2 characters have been truncated!"
fi
echo
read -p "Please Enter an App Number (Use a Space to Separate More Than One): " numbah
removes=0
totalremoves=0
overallstart=$(date +%s)
for num in $numbah; do
   if [ $removes -gt 0 ]; then
      echo "Skipping previous selection"
      totalremoves=$((removes + totalremoves))
      removes=0
      crack $num "$CrackerName"
   fi
done
# WANG SMASH
rm /tmp/catme.tmp
overallend=$(date +%s)
overall=$(( $overallend - $overallstart ))
AppNumber=$(echo "$numbah" | wc -w)
final=$(( $AppNumber - $removes ))
if [ "$final" -gt "1" ]; then
   echo
   echo "Finished $final Apps in $overall Seconds!"
fi
exit 1
# I don't have ADHD, okay?

