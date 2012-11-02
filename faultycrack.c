/*
 
 Dumps decrypted iPhone Applications to a file - better solution than those GDB scripts for non working GDB versions
 (C) Copyright 2011 Stefan Esser
 
 iPod:~ root# DYLD_INSERT_LIBRARIES=dumpdecrypted.dylib /var/mobile/Applications/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/Scan.app/Scan
 mach-o decryption dumper
 
 DISCLAIMER: This tool is only meant for security research purposes, not for application crackers.
 
 ^ lol
 
 [+] Found encrypted data at address 00002000 of length 1826816 bytes - type 1.
 [+] Opening /private/var/mobile/Applications/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/Scan.app/Scan for reading.
 [+] Reading header
 [+] Detecting header type
 [+] Executable is a FAT image - searching for right architecture
 [+] Correct arch is at offset 2408224 in the file
 [+] Opening Scan.decrypted for writing.
 [-] Failed opening. Most probably a sandbox issue. Trying something different.
 [+] Opening /private/var/mobile/Applications/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/tmp/Scan.decrypted for writing.
 [+] Copying the not encrypted start of the file
 [+] Dumping the decrypted data into the file
 [+] Copying the not encrypted remainder of the file
 [+] Closing original file
 [+] Closing dump file
 
 */

/*
 Iljimae (c) Kim Jong-Cracks 1819-2012
 Capitalist developers prosecuted since 1000BC
 
 
 Thanks to poedCrackMod for inspiration, Clutch for swap code, pendo324, Nighthawk and Kim Jong-Un
 
 */
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <stdbool.h>

#define ARMV6 6
#define ARMV7 9
#define ARMV7S 11

struct ProgramVars {
    struct mach_header *mh;
    int *NXArgcPtr;
    const char ***NXArgvPtr;
    const char ***environPtr;
    const char **__prognamePtr;
};

void dump_binary(char *rpath, char *dumpfile, uint32_t offset, struct ProgramVars *pvars);

#define swap32(value) (((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24) )

/*
 ./app  [merge|dump|swap|fat|findoff]
 merge  
 dump  
 
 */
struct fat_header* getHeader(FILE* binary) {
    char buffer[4096];
    size_t result;
    printf("[+] Reading header\n");
    result = fread(&bufer, sizeof(buffer), 1, binary);	
    if (result != sizeof(buffer)) {
        printf("[W] Warning read only %d bytes\n", n);
    }
    struct fat_header* fh =(struct fat_header *) buffer;
    return fh;
}
bool isFat(char* rpath) {
    struct fat_header* fh = getHeader(rpath);
    /* Is this a FAT file - we assume the right endianess */
    if (fh->magic == FAT_CIGAM) {
        printf("[+] Executable is a FAT binary\n");
        return true;
    }
    else if (fh->magic == MH_MAGIC) {
        printf("[+] Executable is a thin binary\n");
        return false;
    }
    else {
        printf("[-] Executable is of unknown type\n");
        return false;
    }
}

uint32_t getOffset(struct fat_header* fh, int archtype) {
    struct fat_arch* arch = (struct fat_arch *) &fh[1];
    for (i = 0; i < swap32(fh->nfat_arch); i++) {
        printf("swag yolo %i\n", swap32(arch->cpusubtype));
        if (swap32(arch->cpusubtype) == archtype) {
            return arch.offset;
        }
        arch++;
    }
}

__attribute__ ((constructor))
void dumptofile(int argc, const char **argv, const char **envp, const char **apple, struct ProgramVars *pvars)
{
    struct load_command *lc;
    struct encryption_info_command *eic;
    struct fat_header *fh;
    struct fat_arch *arch;
    char buffer[4096];
    char rpath[4096], npath[4096];	/* should be big enough for PATH_MAX */
    char dumpfile[4096];	//yolo
    unsigned int fileoffs = 0, off_cryptid = 0, restsize;
    uint32_t offset;
    int i, fd, outfd, r, n, toread;
    char *tmp;
    if (argc > 1) {
        if (strcmp(argv[1], "dump") == 0) {
            int arch;
            
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[3], sizeof(rpath));
            }
            FILE* oldbinary = fopen(rpath, "r+");
            
            if (realpath(argv[3], dumpfile) == NULL) {
                strlcpy(dumpfile, argv[3], sizeof(rpath));
            }
            arch = atoi(argv[2]);
            printf("[+] Preparing to dump binary");
            uint32_t offset = 
            dump_binary(binary, dumpfile, offset, pvars);
            _exit(1);
        }
        else if (strcmp(argv[1], "offsets") == 0) {
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[0], sizeof(rpath));
            }
            struct fat_arch armv6, armv7, armv7s;
            bool has_armv7 = FALSE, has_armv6 = FALSE;
            struct fat_arch *arch;
            arch = (struct fat_arch *) &fh[1];
            
            struct fat_header* fh = getHeader(rpath);
            
            int archcount;
            
            for (i = 0; i < swap32(fh->nfat_arch); i++) {
                printf("swag yolo %i\n", swap32(arch->cpusubtype));
                if (swap32(arch->cpusubtype) == ARMV6) {
                    armv6 = *arch;
                    archcount++;
                    printf("### ARMV6 offset %u\n", swap32(armv6.offset));
                    printf("armv6 subtype %u\n", arch->cpusubtype);
                    fprintf(stderr, "armv6_offset=%u", swap32(armv6.offset));
                    has_armv6 = TRUE;
                }
                else if (swap32(arch->cpusubtype) == ARMV7) {
                    armv7 = *arch;
                    archcount++;
                    printf("### ARMV7 offset %u\n", swap32(armv7.offset));
                    printf("armv7 subtype %u\n", arch->cpusubtype);
                    fprintf(stderr, "armv7_offset=%u", swap32(armv7.offset));
                    has_armv7 = TRUE;
                }
                else if (swap32(arch->cpusubtype) == ARMV7S) {
                    armv7s = *arch;
                    //printf("armv7s subtype %u\n", arch->cpusubtype);
                    fprintf(stderr, "armv7s_offset=%u", swap32(armv7s.offset));
                    archcount++;
                }
                arch++;
            }
            fprintf(stderr, "archcount=%i", archcount);
            _exit(1);

        }
        else if (strcmp(argv[1], "isfat") == 0) {
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[0], sizeof(rpath));
            }
            if (isFat(rpath)) {
                fprintf(stderr, "isfat=1\n");
            }
            else {
                fprintf(stderr, "isfat=0\n");
            }
            _exit(1);

        }
    }
    printf("mach-o decryption dumpern\n");
    /* searching all load commands for an LC_ENCRYPTION_INFO load command */
    lc = (struct load_command *) ((unsigned char *) pvars->mh + sizeof(struct mach_header));
    
    for (i = 0; i < pvars->mh->ncmds; i++) {
        printf("Load Command (%d): %08xn", i, lc->cmd);
        
        if (lc->cmd == LC_ENCRYPTION_INFO) {
            eic = (struct encryption_info_command *) lc;
            
            /* If this load command is present, but data is not crypted then exit */
            if (eic->cryptid == 0) {
                break;
            }
            off_cryptid = (off_t) ((void *) &eic->cryptid - (void *) pvars->mh);
            printf("[+] Found encrypted data at address %08x of length %u bytes - type %u.n", eic->cryptoff, eic->cryptsize, eic->cryptid);
            
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[0], sizeof(rpath));
            }
            
            printf("[+] Opening %s for reading.n", rpath);
            fd = open(rpath, O_RDONLY);
            if (fd == -1) {
                printf("[-] Failed opening.n");
                _exit(1);
            }
            
            printf("[+] Reading headern");
            n = read(fd, (void *) buffer, sizeof(buffer));	//yolo cuz fat_arch is useless
            if (n != sizeof(buffer)) {
                printf("[W] Warning read only %d bytesn", n);
            }
            
            printf("[+] Detecting header typen");
            fh = (struct fat_header *) buffer;
            
            /* Is this a FAT file - we assume the right endianess */
            if (fh->magic == FAT_CIGAM) {
                
                //TODO
                //swap the architecture of the original binary
                //execute the original binary with armv6
                //dump it
                //return back
                
                printf("[+] Executable is a FAT image - searching for right architecturen");
                arch = (struct fat_arch *) &fh[1];
                for (i = 0; i < swap32(fh->nfat_arch); i++) {
                    if ((pvars->mh->cputype == swap32(arch->cputype)) && (pvars->mh->cpusubtype == swap32(arch->cpusubtype))) {
                        fileoffs = swap32(arch->offset);
                        printf("[+] Correct arch is at offset %u in the filen", fileoffs);
                        printf("cputype %u, cpusubtype %un", swap32(arch->cputype), swap32(arch->cputype));
                        break;
                    }
                    
                    arch++;
                }
                
                if (fileoffs == 0) {
                    printf("[-] Could not find correct arch in FAT imagen");
                    _exit(1);
                }
            } else if (fh->magic == MH_MAGIC) {
                printf("[+] Executable is a plain MACH-O imagen");
            } else {
                printf("[-] Executable is of unknown typen");
                _exit(1);
            }
            
            /* extract basename */
            tmp = strrchr(rpath, '/');
            if (tmp == NULL) {
                printf("[-] Unexpected error with filename.n");
                _exit(1);
            }
            strlcpy(npath, tmp + 1, sizeof(npath));
            strlcat(npath, ".decrypted", sizeof(npath));
            strlcpy(buffer, npath, sizeof(buffer));
            
            printf("[+] Opening %s for writing.n", npath);
            outfd = open(npath, O_RDWR | O_CREAT | O_TRUNC, 0644);
            if (outfd == -1) {
                if (strncmp("/private/var/mobile/Applications/", rpath, 33) == 0) {
                    printf("[-] Failed opening. Most probably a sandbox issue. Trying something different.n");
                    
                    /* create new name */
                    strlcpy(npath, "/private/var/mobile/Applications/", sizeof(npath));
                    tmp = strchr(rpath + 33, '/');
                    if (tmp == NULL) {
                        printf("[-] Unexpected error with filename.n");
                        _exit(1);
                    }
                    tmp++;
                    *tmp++ = 0;
                    strlcat(npath, rpath + 33, sizeof(npath));
                    strlcat(npath, "tmp/", sizeof(npath));
                    strlcat(npath, buffer, sizeof(npath));
                    printf("[+] Opening %s for writing.n", npath);
                    outfd = open(npath, O_RDWR | O_CREAT | O_TRUNC, 0644);
                }
                if (outfd == -1) {
                    perror("[-] Failed opening");
                    printf("n");
                    _exit(1);
                }
            }
            
            /* calculate address of beginning of crypted data */
            n = fileoffs + eic->cryptoff;
            
            restsize = lseek(fd, 0, SEEK_END) - n - eic->cryptsize;
            lseek(fd, 0, SEEK_SET);
            
            printf("[+] Copying the not encrypted start of the filen");
            /* first copy all the data before the encrypted data */
            while (n > 0) {
                toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
                r = read(fd, buffer, toread);
                if (r != toread) {
                    printf("[-] Error reading filen");
                    _exit(1);
                }
                n -= r;
                
                r = write(outfd, buffer, toread);
                if (r != toread) {
                    printf("[-] Error writing filen");
                    _exit(1);
                }
            }
            
            
            
            /* now write the previously encrypted data (header) */
            printf("[+] Dumping the decrypted data into the filen");
            r = write(outfd, (unsigned char *) pvars->mh + eic->cryptoff, eic->cryptsize);
            if (r != eic->cryptsize) {
                printf("[-] Error writing filen");
                _exit(1);
            }
            
            /* and finish with the remainder of the file */
            n = restsize;
            lseek(fd, eic->cryptsize, SEEK_CUR);
            printf("[+] Copying the not encrypted remainder of the filen");
            while (n > 0) {
                toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
                r = read(fd, buffer, toread);
                if (r != toread) {
                    printf("[-] Error reading filen");
                    _exit(1);
                }
                n -= r;
                
                r = write(outfd, buffer, toread);
                if (r != toread) {
                    printf("[-] Error writing filen");
                    _exit(1);
                }
            }
            
            printf("[+] Closing original filen");
            close(fd);
            printf("[+] Closing dump filen");
            close(outfd);
            
            _exit(1);
        }
        
        lc = (struct load_command *) ((unsigned char *) lc + lc->cmdsize);
    }
    printf("[-] This mach-o file is not encrypted. Nothing was decrypted.n");
    _exit(1);
}


void dump_binary(char *rpath, char *dumpfile, uint32_t offset, struct ProgramVars *pvars)
{
    struct encryption_info_command *eic;
    struct load_command *lc;
    uint32_t off_cryptid, text_start = 0, cryptoff;
    int i, fd;
    bool foundCrypt = FALSE, foundSegment = FALSE; //probably unncessary 
    
    //find the cryptid/offset for the particular arch 
    lc = (struct load_command *) ((unsigned char *) pvars->mh + sizeof(struct mach_header));
    
    for (i = 0; i < pvars->mh->ncmds; i++) {
        printf("Load Command (%d): %08xn", i, lc->cmd);
        if (lc->cmd == LC_ENCRYPTION_INFO) {
            eic = (struct encryption_info_command *) lc;
            
            /* If this load command is present, but data is not crypted then exit */
            if (eic->cryptid == 0) {
                break;
                _exit(5);
            }
            off_cryptid = (off_t) ((void *) &eic->cryptid - (void *) pvars->mh);
            printf("[+] offset to cryptid found: @%p(from %p) = %x\n", &eic->cryptid, pvars->mh, off_cryptid);
            printf("[+] Found encrypted data at address %08x of length %u bytes - type %u.\n", eic->cryptoff, eic->cryptsize, eic->cryptid);
            foundCrypt = TRUE;
        }
        else if (lc->cmd == LC_SEGMENT) {
            /*sc = (struct segment_commmand_do *) lc;
            uint32_t fourzeroninesix = 4096;
            if (sc->vmaddr != &fourzeroninesix) { //help!!!11
                printf("[+] Capitalist developers tried to change the vmaddr!"); //thanks Rastingac
                text_start = sc->vmaddr;
                foundSegment = TRUE;
            }*/
            foundSegment = TRUE;
        }
        if (foundCrypt && foundSegment) {
            break;
        }
        lc = (struct load_command *) ((unsigned char *) lc + lc->cmdsize);
    }
    
    printf("[+] Opening %s for reading.\n", rpath);
    fd = open(rpath, O_RDONLY);
    if (fd == -1) {
        printf("[-] Failed opening.\n");
        _exit(1);
    }
    
    int outfile = open(dumpfile, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (outfile == -1) {
        return;
    }
    lseek(outfile, offset, SEEK_SET);	//go to the offset
    
    /* we only add back the mach header and extra stuff later */
    
    if (text_start != 0) {
        cryptoff = eic->cryptoff + text_start;
    }
    else {
        cryptoff = eic->cryptoff;
    }
    
    /* now write the previously encrypted data (header) */
    printf("[+] Dumping the decrypted data into the file\n");
    int r = write(outfile, (unsigned char *) pvars->mh + cryptoff, eic->cryptsize);
    if (r != eic->cryptsize) {
        printf("[-] Error writing file\n");
        
    }
    //patch the cryptid
    if (off_cryptid) {
        uint32_t zero=0;
        off_cryptid+=offset;
        printf("[+] Setting the LC_ENCRYPTION_INFO->cryptid to 0 at offset %x\n", off_cryptid);
        if (lseek(outfile, off_cryptid, SEEK_SET) != off_cryptid || write(outfile, &zero, 4) != 4) {
            printf("[-] Error writing cryptid value\n");
            _exit(6);
        }
        printf("cryptoff=%u\n", cryptoff);
        printf("cryptsize=%u\n", eic->cryptsize);
    }
}

//void dump_header(char *rpath, char *dumpfile, uint32_t offset, uint32_t cryptoff, uint32_t cryptsize) {
//    int i, fd, r;
//    
//    printf("[+] Opening %s for reading.\n", rpath);
//    fd = open(rpath, O_RDONLY);
//    if (fd == -1) {
//        printf("[-] Failed opening.\n");
//        _exit(1);
//    }
//    
//    /* calculate address of beginning of crypted data */
//    n = offset + cryptoff;
//    
//    restsize = lseek(fd, 0, SEEK_END) - n - cryptsize;
//    lseek(fd, 0, SEEK_SET);
//    
//    printf("[+] Copying the not encrypted start of the filen");
//    /* first copy all the data before the encrypted data */
//    while (n > 0) {
//        toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
//        r = read(fd, buffer, toread);
//        if (r != toread) {
//            printf("[-] Error reading filen");
//            _exit(1);
//        }
//        n -= r;
//        
//        r = write(outfd, buffer, toread);
//        if (r != toread) {
//            printf("[-] Error writing filen");
//            _exit(1);
//        }
//    }
//    
//
//    
//}
