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

#define swap32(value) (((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24) )

/*
 ./app  [merge|dump|swap|fat|findoff]
 merge  
 dump  
 
 */


void swap(int fd, int outfd, struct ProgramVars *pvars, int swaparch) {
    char buffer[4096];
    size_t result;
    printf("[+] Reading header\n");
    lseek(fd, 0, SEEK_SET);
    result = read(fd, &buffer, sizeof(buffer));
    if (result != sizeof(buffer)) {
        printf("[W] Warning read only %zu bytes\n", result);
    }
    struct fat_header* fh =(struct fat_header *) buffer;
    
    uint32_t arch_subtype;
    int i;
    
    switch (swaparch) {
        case 6:
            arch_subtype = 0x6000000;
        case 9:
            arch_subtype = 0x9000000;
        case 11:
            arch_subtype = 0xb000000; //bu
            
    }
    bool swap1 = FALSE, swap2 = FALSE;
    struct fat_arch* arch = (struct fat_arch *) &fh[1];
    for (i = 0; i < swap32(fh->nfat_arch); i++) {
        printf("swag yolo %i\n", swap32(arch->cpusubtype));
        if (swap32(pvars->mh->cpusubtype) == swap32(arch->cpusubtype)) {
            arch->cpusubtype = arch_subtype;
            printf("[+] Found local arch %i", swap32(pvars->mh->cpusubtype));
            swap1 = TRUE;
        }
        else if (swap32(arch->cpusubtype) == swaparch) {
            printf("[+] Found arch to swap %i", swaparch);
            arch->cpusubtype = pvars->mh->cpusubtype;
            swap2 = TRUE;
            
        }
        if (swap1 && swap2) {
            break;
        }
        arch++;
    }
    //write it back
    lseek(outfd, 0, SEEK_SET);
    result = write(outfd, buffer, sizeof(buffer));
    if (result != sizeof(buffer)) {
        printf("[-] Could not write mach header");
    }
}

void dump_header(int fd, int outfd, uint32_t offset, uint32_t cryptoff, uint32_t cryptsize) {
    int r, n, restsize, toread;
    
    char buffer[1024];
    
    /* calculate address of beginning of crypted data */
    n = offset + cryptoff;
    
    restsize = lseek(fd, 0, SEEK_END) - n - cryptsize;
    lseek(fd, 0, SEEK_SET);
    lseek(outfd, 0, SEEK_SET);
    
    printf("[+] Copying the not encrypted start of the file\n");
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
            printf("[-] Error writing file\n");
            _exit(1);
        }
    }
}

void dump_binary(int rfd, int fd, uint32_t offset, struct ProgramVars *pvars, struct fat_header* mh)
{
    int r, n, restsize, toread;
    
    char* buffer[1024];
    struct encryption_info_command *eic;
    struct load_command *lc;
    uint32_t off_cryptid, cryptoff;
    int i;
    bool foundCrypt = FALSE, foundSegment = FALSE; //probably unncessary 
    
    //find the cryptid/offset for the particular arch 
    lc = (struct load_command *) ((unsigned char *) pvars->mh + sizeof(struct mach_header));
    
    for (i = 0; i < pvars->mh->ncmds; i++) {
        printf("Load Command (%d): %08xn", i, lc->cmd);
        if (lc->cmd == LC_ENCRYPTION_INFO) {
            eic = (struct encryption_info_command *) lc;
            
            /* If this load command is present, but data is not crypted then exit */
            if (eic->cryptid == 0) {
                printf("[-] Application is already cracked?!//3321222 yolo");
                break;
                _exit(5);
            }
            off_cryptid = (off_t) ((void *) &eic->cryptid - (void *) pvars->mh);
            printf("[+] offset to cryptid found: @%p(from %p) = %x\n", &eic->cryptid, pvars->mh, off_cryptid);
            printf("[+] Found encrypted data at address %08x of length %u bytes - type %u.\n", eic->cryptoff, eic->cryptsize, eic->cryptid);
            foundCrypt = TRUE;
        }
        else if (lc->cmd == LC_SEGMENT) {
            //            struct segment_command* sc = (struct segment_commmand *) lc;
            /* if (swap32(sc->vmaddr) != 4096 { 
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

    n = offset + eic->cryptoff; /* we assume header is normally at 0x1000 */
    
     
    restsize = lseek(rfd, 0, SEEK_END) - n - eic->cryptsize;			
    lseek(rfd, 0, SEEK_SET);
    
    printf("[+] Copying the not encrypted start of the file\n");
    /* first copy all the data before the encrypted data */
    while (n > 0) {
        toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
        r = read(rfd, buffer, toread);
        if (r != toread) {
            printf("[-] Error reading file\n");
            _exit(1);
        }
        n -= r;
        
        r = write(fd, buffer, toread);
        if (r != toread) {
            printf("[-] Error writing file\n");
            _exit(1);
        }
    }
    
    /* now write the previously encrypted data */
    printf("[+] Dumping the decrypted data into the file\n");
    r = write(fd, (unsigned char *)pvars->mh + eic->cryptoff, eic->cryptsize);
    if (r != eic->cryptsize) {
        printf("[-] Error writing file\n");
        _exit(1);
    }
    
    /* and finish with the remainder of the file */
    n = restsize;
    lseek(rfd, eic->cryptsize, SEEK_CUR);
    printf("[+] Copying the not encrypted remainder of the file\n");
    while (n > 0) {
        toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
        r = read(rfd, buffer, toread);
        if (r != toread) {
            printf("[-] Error reading file\n");
            _exit(1);
        }
        n -= r;
        
        r = write(fd, buffer, toread);
        if (r != toread) {
            printf("[-] Error writing file\n");
            _exit(1);
        }
    }

    //patch the cryptid
    if (off_cryptid) {
        uint32_t zero=0;
        off_cryptid+=offset;
        printf("[+] Setting the LC_ENCRYPTION_INFO->cryptid to 0 at offset %x\n", off_cryptid);
        if (lseek(fd, off_cryptid, SEEK_SET) != off_cryptid || write(fd, &zero, 4) != 4) {
            printf("[-] Error writing cryptid value\n");
            _exit(6);
        }
        fprintf(stderr, "cryptoff=%u\n", cryptoff);
        fprintf(stderr, "cryptsize=%u\n", eic->cryptsize);
    }
}


struct fat_header* getHeader(int fd) {
    char buffer[1024];
    size_t result;
    printf("[+] Reading header\n");
    lseek(fd, 0, SEEK_SET);
    result = read(fd, &buffer, sizeof(buffer));
    if (result != sizeof(buffer)) {
        printf("[W] Warning read only %zu bytes\n", result);
    }
    struct fat_header* fh =(struct fat_header *) buffer;
    return fh;
}
bool isFat(int fd) {
    struct fat_header* fh = getHeader(fd);
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
    int i;
    struct fat_arch* arch = (struct fat_arch *) &fh[1];
    for (i = 0; i < swap32(fh->nfat_arch); i++) {
        printf("swag yolo %i\n", swap32(arch->cpusubtype));
        if (swap32(arch->cpusubtype) == archtype) {
            printf("[+] Found offset %u", arch->offset);
            return arch->offset;
        }
        arch++;
    }
    return 0;
}

__attribute__ ((constructor))
void dumptofile(int argc, const char **argv, const char **envp, const char **apple, struct ProgramVars *pvars)
{
    char rpath[4096], dump_path[4096];/* should be big enough for PATH_MAX */
    int fd, outfd;
    bool dumprest;
    if (argc > 0) {
        printf("dud yolo\n");
        if (strcmp(argv[1], "dump") == 0) {
            //dump <arch> <dumpfile> <dumprest>
            int arch;
            
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[0], sizeof(rpath));
            }
            printf("path %s\n", rpath);
            fd = open(rpath, O_RDONLY);
            if (realpath(argv[3], dump_path) == NULL) {
                strlcpy(dump_path, argv[3], sizeof(rpath));
            }
            if (strcmp(argv[4], "1")) {
                dumprest = TRUE;
            }
            outfd = open(dump_path, O_RDWR|O_CREAT|O_TRUNC, 0644);
            arch = atoi(argv[2]);
            printf("[+] Preparing to dump binary\n");
            printf("path %s, arch %i\n", dump_path, arch);
            struct fat_header* fh = getHeader(fd);
            
            uint32_t offset;
            if (fh->magic == FAT_CIGAM) {
                offset = getOffset(fh, arch);
            }
            else if (fh->magic == MH_MAGIC) {
                offset = 0;
            }
            dump_binary(fd, outfd, offset, pvars, fh);
            _exit(1);
        }
    
        else if (strcmp(argv[1], "offsets") == 0) {
            if (realpath(argv[0], rpath) == NULL) {
                strlcpy(rpath, argv[0], sizeof(rpath));
            }
            printf("yo yo yo lo %s\n", rpath);
            struct fat_arch armv6, armv7, armv7s;
            bool has_armv7 = FALSE, has_armv6 = FALSE;
            
            fd = open(rpath, O_RDONLY);
            if (fd == -1) {
                printf("[-] Binary could not be opened\n");
                _exit(6);
            }
            struct fat_header* fh = getHeader(fd);
            int archcount = 0;
            if (fh->magic == FAT_CIGAM) {
                struct fat_arch* arch = (struct fat_arch *) &fh[1];
                int z = 0;
                printf("swag yolo yoyo %i\n", swap32(fh->nfat_arch));
                for (z = 0; z < swap32(fh->nfat_arch); z++) {
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
            }
            else if (fh->magic == MH_MAGIC) {
				printf("[+] Executable is a plain MACH-O image\n");
                archcount = 1;
                
            }
            else {
                printf("[-] Unknown executable type\n");
                _exit(5);
            }
            fprintf(stderr, "archcount=%i", archcount);
            _exit(1);
        }
    }
}



    
//}
