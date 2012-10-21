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
   export tmp="/tmp/Iljimae-$RANDOM$(date +%s)"
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
    ln -s $AppPath $tmp/Payload
   #patch iTunesMetadata
   export IPA="~/Documents/Cracked/$AppDisplayName-v$AppVer-$CrackerName.ipa"
   if [ -e "$AppPath/AppName/iTunesMetadata.plist" ];then
      WorkDir="$AppPath/AppName"
      ThankYouRastignac=$(plutil -xml "$WorkDir/iTunesMetadata.plist" 2>&1> /dev/null)
      echo -e "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > "$tmp/iTunesMetadata.plist"
      echo -e "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" >> "$tmp/iTunesMetadata.plist"
      echo -e "<plist version=\"1.0\">" >> "$tmp/iTunesMetadata.plist"
      echo -e "<dict>" >> "$tmp/iTunesMetadata.plist"
      echo -e "\t<key>appleId</key>" >> "$tmp/iTunesMetadata.plist"
      echo -e "\t<string>cracker@apple.com</string>" >> "$tmp/iTunesMetadata.plist"
      echo -e "\t<key>purchaseDate</key>" >> "$tmp/iTunesMetadata.plist"
      echo -e "\t<date>2012-12-21T23:59:59Z</date>" >> "$tmp/iTunesMetadata.plist"
      # MadHouse can last more than three seconds in bed!
      grep -A1 "<key>artistId</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>artistName</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>buy-only</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>buyParams</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>copyright</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>drmVersionNumber</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>fileExtension</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 -m1 "<key>genre</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 -m1 "<key>genreId</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>itemId</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>itemName</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>kind</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>playlistArtistName</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>playlistName</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>price</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>priceDisplay</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A99 "<key>rating</key>" "$WorkDir/iTunesMetadata.plist" | grep -m1 -B99 "</dict>" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>releaseDate</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>s</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>softwareIcon57x57URL</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>softwareIconNeedsShine</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A99 "<key>softwareSupportedDeviceIds</key>" "$WorkDir/iTunesMetadata.plist" | grep -m1 -B99 "</array>" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>softwareVersionBundleId</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>softwareVersionExternalIdentifier</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A99 "<key>softwareVersionExternalIdentifiers</key>" "$WorkDir/iTunesMetadata.plist" | grep -m1 -B99 "</array>" >> "$tmp/iTunesMetadata.plist"
      grep -A99 "<key>subgenres</key>" "$WorkDir/iTunesMetadata.plist" | grep -m1 -B99 "</array>" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>vendorId</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      grep -A1 "<key>versionRestrictions</key>" "$WorkDir/iTunesMetadata.plist" >> "$tmp/iTunesMetadata.plist"
      echo -e "</dict>" >> "$tmp/iTunesMetadata.plist"
      echo -e "</plist>" >> "$tmp/iTunesMetadata.plist"
      ThankYouRastignacAGAIN=$(plutil -binary "$tmp/iTunesMetadata.plist" 2>&1> /dev/null)
      MADHOUSEMADHOUSE=$(plutil -binary "$tmp/iTunesMetadata.plist")

#pack non-essential stuff
jongun=$(cd $"tmp"; zip -u -y -r "$IPA" Payload/* -x Payload/iTunesArtwork Payload/iTunesMetadata.plist Payload/Documents/* Payload/Library/* Payload/tmp/* "Payload/*/$AppExec" Payload/*/SC_Info/* & 2>&1> /dev/null")
   fi
   
   tmpexec="$tmp/$AppExec.decrypted"
   swag=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" findoff > /tmp/.yolo)
   include /tmp/.yolo
   if [ "$thin_binary" = "1" ]; then
      swag=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" dump "$tmpexec" 0  > /tmp/.yolo) #yay for thin binaries
      if [ "$?" != "1" ]; then
         echo "Error cracking binary"
         let removes+= 1; continue
      fi
      
   else
      echo "[+] Dumping armv7 portion of binary"
      armv7=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" dump "$tmpexec" "$armv7_offset"  > /tmp/.yolo)
      if [ "$?" != "1" ]; then
         echo "[-] Cannot dump armv7 portion"
         let removes+=1; continue
      fi
      echo "[+] Dumping armv6 portion of binary"
      armv6=$(DYLD_INSERT_LIBRARIES=$DYLB_FILE; "$AppPath/$AppName/$AppExec" dump "$tmpexec" "$armv6_offset"  > /tmp/.yolo)
      if [ "$?" != "1" ]; then
         echo "[-] Cannot dump armv6 portion"
         let removes+=1; continue
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
      clean
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

