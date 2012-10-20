/*
 
 Dumps decrypted iPhone Applications to a file - better solution than those GDB scripts for non working GDB versions
 (C) Copyright 2011 Stefan Esser
 
 iPod:~ root# DYLD_INSERT_LIBRARIES=dumpdecrypted.dylib /var/mobile/Applications/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/Scan.app/Scan
 mach-o decryption dumper
 
 DISCLAIMER: This tool is only meant for security research purposes, not for application crackers.
 
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
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>

#define ARMV6 6
#define ARMV7 9

struct ProgramVars {
    struct mach_header*	mh;
    int*		NXArgcPtr;
    const char***	NXArgvPtr;
    const char***	environPtr;
    const char**	__prognamePtr;
};

void dump_binary(char *dumpfile, uint32_t cryptsize, uint32_t cryptoff, struct ProgramVars *pvars);

#define swap32(value) (((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24) )

/*
 ./app <dump file> [merge|dump|swap|fat|findoff]
 merge <cryptoff> <crytsize>
 dump <cryptsize> <cryptoff>
 
 */

__attribute__((constructor))
void dumptofile(int argc, const char **argv, const char **envp, const char **apple, struct ProgramVars *pvars)
{
	struct load_command *lc;
	struct encryption_info_command *eic;
	struct fat_header *fh;
	struct fat_arch *arch;
	struct mach_header *mh;
	char buffer[1024];
	char rpath[4096],npath[4096]; /* should be big enough for PATH_MAX */
    char dumpfile[4096]; //yolo
	unsigned int fileoffs = 0, off_cryptid = 0, restsize;
    uint32_t cryptsize, cryptoff;
	int i,fd,outfd,r,n,toread;
	char *tmp;
	
    if (strcmp(argv[1], "dump") != 0) {
        strlcpy(dumpfile, argv[2], sizeof(dumpfile));
        printf("SWAG YOLO");
        sscanf(argv[3], "%"SCNu32, &cryptsize);
        sscanf(argv[4], "%"SCNu32, &cryptoff);
        printf("SWAG YOLO 123");
        dump_binary(dumpfile, cryptsize, cryptoff, pvars);
        _exit(1);
    }
    else if (strcmp(argv[1], "findoff") != 0) {
        lc = (struct load_command *)((unsigned char *)pvars->mh + sizeof(struct mach_header));
        
        for (i=0; i<pvars->mh->ncmds; i++) {
            fprintf(stderr, "Load Command (%d): %08x\n", i, lc->cmd);
            
            if (lc->cmd == LC_ENCRYPTION_INFO) {
                eic = (struct encryption_info_command *)lc;
                
                /* If this load command is present, but data is not crypted then exit */
                if (eic->cryptid == 0) {
                    _exit(33);
                }
                off_cryptid=(off_t)((void*)&eic->cryptid - (void*)pvars->mh);
                fprintf(stderr, "[+] Found encrypted data at address %08x of length %u bytes - type %u.\n", eic->cryptoff, eic->cryptsize, eic->cryptid);
                printf("cryptoff
                if (realpath(argv[0], rpath) == NULL) {
                    strlcpy(rpath, argv[0], sizeof(rpath));
                }
            }
        }
                

    }
	fprintf(stderr, "mach-o decryption dumper\n\n");
	/* searching all load commands for an LC_ENCRYPTION_INFO load command */
	lc = (struct load_command *)((unsigned char *)pvars->mh + sizeof(struct mach_header));
    
	for (i=0; i<pvars->mh->ncmds; i++) {
		fprintf(stderr, "Load Command (%d): %08x\n", i, lc->cmd);
		
		if (lc->cmd == LC_ENCRYPTION_INFO) {
			eic = (struct encryption_info_command *)lc;
			
			/* If this load command is present, but data is not crypted then exit */
			if (eic->cryptid == 0) {
				break;
			}
			off_cryptid=(off_t)((void*)&eic->cryptid - (void*)pvars->mh);
			fprintf(stderr, "[+] Found encrypted data at address %08x of length %u bytes - type %u.\n", eic->cryptoff, eic->cryptsize, eic->cryptid);
			
			if (realpath(argv[0], rpath) == NULL) {
				strlcpy(rpath, argv[0], sizeof(rpath));
			}
			
			fprintf(stderr, "[+] Opening %s for reading.\n", rpath);
			fd = open(rpath, O_RDONLY);
			if (fd == -1) {
				fprintf(stderr, "[-] Failed opening.\n");
				_exit(1);
			}
			
			fprintf(stderr, "[+] Reading header\n");
			n = read(fd, (void *)buffer, sizeof(buffer)); //yolo cuz fat_arch is useless
			if (n != sizeof(buffer)) {
				fprintf(stderr, "[W] Warning read only %d bytes\n", n);
			}
			
			fprintf(stderr, "[+] Detecting header type\n");
			fh = (struct fat_header *)buffer;
			
			/* Is this a FAT file - we assume the right endianess */
			if (fh->magic == FAT_CIGAM) {

                //TODO
                //swap the architecture of the original binary
                //execute the original binary with armv6
                //dump it
                //return back
                
				fprintf(stderr, "[+] Executable is a FAT image - searching for right architecture\n");
				arch = (struct fat_arch *)&fh[1];
				for (i=0; i<swap32(fh->nfat_arch); i++) {
					if ((pvars->mh->cputype == swap32(arch->cputype)) && (pvars->mh->cpusubtype == swap32(arch->cpusubtype))) {
						fileoffs = swap32(arch->offset);
						fprintf(stderr, "[+] Correct arch is at offset %u in the file\n", fileoffs);
                        fprintf(stderr, "cputype %u, cpusubtype %u\n", swap32(arch->cputype), swap32(arch->cputype));
						break;
					}
                    
					arch++;
				}
                
				if (fileoffs == 0) {
					fprintf(stderr, "[-] Could not find correct arch in FAT image\n");
					_exit(1);
				}
			} else if (fh->magic == MH_MAGIC) {
				fprintf(stderr, "[+] Executable is a plain MACH-O image\n");
			} else {
				fprintf(stderr, "[-] Executable is of unknown type\n");
				_exit(1);
			}
            
			/* extract basename */
			tmp = strrchr(rpath, '/');
			if (tmp == NULL) {
				fprintf(stderr, "[-] Unexpected error with filename.\n");
				_exit(1);
			}
			strlcpy(npath, tmp+1, sizeof(npath));
			strlcat(npath, ".decrypted", sizeof(npath));
			strlcpy(buffer, npath, sizeof(buffer));
            
			fprintf(stderr, "[+] Opening %s for writing.\n", npath);
			outfd = open(npath, O_RDWR|O_CREAT|O_TRUNC, 0644);
			if (outfd == -1) {
				if (strncmp("/private/var/mobile/Applications/", rpath, 33) == 0) {
					fprintf(stderr, "[-] Failed opening. Most probably a sandbox issue. Trying something different.\n");
					
					/* create new name */
					strlcpy(npath, "/private/var/mobile/Applications/", sizeof(npath));
					tmp = strchr(rpath+33, '/');
					if (tmp == NULL) {
						fprintf(stderr, "[-] Unexpected error with filename.\n");
						_exit(1);
					}
					tmp++;
					*tmp++ = 0;
					strlcat(npath, rpath+33, sizeof(npath));
					strlcat(npath, "tmp/", sizeof(npath));
					strlcat(npath, buffer, sizeof(npath));
					fprintf(stderr, "[+] Opening %s for writing.\n", npath);
					outfd = open(npath, O_RDWR|O_CREAT|O_TRUNC, 0644);
				}
				if (outfd == -1) {
					perror("[-] Failed opening");
					fprintf(stderr, "\n");
					_exit(1);
				}
			}
			
			/* calculate address of beginning of crypted data */
			n = fileoffs + eic->cryptoff;
			
			restsize = lseek(fd, 0, SEEK_END) - n - eic->cryptsize;			
			lseek(fd, 0, SEEK_SET);
			
			fprintf(stderr, "[+] Copying the not encrypted start of the file\n");
			/* first copy all the data before the encrypted data */
			while (n > 0) {
				toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
				r = read(fd, buffer, toread);
				if (r != toread) {
					fprintf(stderr, "[-] Error reading file\n");
					_exit(1);
				}
				n -= r;
				
				r = write(outfd, buffer, toread);
				if (r != toread) {
					fprintf(stderr, "[-] Error writing file\n");
					_exit(1);
				}
			}
			
            
            
			/* now write the previously encrypted data (header) */
			fprintf(stderr, "[+] Dumping the decrypted data into the file\n");
			r = write(outfd, (unsigned char *)pvars->mh + eic->cryptoff, eic->cryptsize);
			if (r != eic->cryptsize) {
				fprintf(stderr, "[-] Error writing file\n");
				_exit(1);
			}
			
			/* and finish with the remainder of the file */
			n = restsize;
			lseek(fd, eic->cryptsize, SEEK_CUR);
			fprintf(stderr, "[+] Copying the not encrypted remainder of the file\n");
			while (n > 0) {
				toread = (n > sizeof(buffer)) ? sizeof(buffer) : n;
				r = read(fd, buffer, toread);
				if (r != toread) {
					fprintf(stderr, "[-] Error reading file\n");
					_exit(1);
				}
				n -= r;
				
				r = write(outfd, buffer, toread);
				if (r != toread) {
					fprintf(stderr, "[-] Error writing file\n");
					_exit(1);
				}
			}
			
			fprintf(stderr, "[+] Closing original file\n");
			close(fd);
			fprintf(stderr, "[+] Closing dump file\n");
			close(outfd);
			
			_exit(1);
		}
		
		lc = (struct load_command *)((unsigned char *)lc+lc->cmdsize);		
	}
	fprintf(stderr, "[-] This mach-o file is not encrypted. Nothing was decrypted.\n");
	_exit(1);
}
/* TODO: set the LC_ENCRYPTION_INFO->cryptid to 0 */

void dump_binary(char *dumpfile, uint32_t cryptsize, uint32_t cryptoff, struct ProgramVars *pvars) {
    int outfile = open(dumpfile, O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (outfile == -1) {
        return;
    }
    lseek(outfile, cryptoff, SEEK_SET); //go to the top
    
    /* now write the previously encrypted data (header) */
    fprintf(stderr, "[+] Dumping the decrypted data into the file\n");
    int r = write(outfile, (unsigned char *)pvars->mh + cryptoff, cryptsize);
    if (r != cryptsize) {
        fprintf(stderr, "[-] Error writing file\n");
    }
}
