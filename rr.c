#include <Windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <PE file>\n", argv[0]);
        return 1;
    }

    FILE* dataFile = fopen(argv[1], "rb");

    if (!dataFile) return FALSE;
  
    fseek(dataFile, 0, SEEK_END);
    size_t size = ftell(dataFile);
    fseek(dataFile, 0, SEEK_SET);

    BYTE* buff = malloc(size);

    if (!fread(buff, 1, size, dataFile )) {
    printf("error\n");
    return 1;
    }

   // for (int i=0; i < size; i++) {
    //printf("\\0x%02x", buff[i]);
    //}

   // printf("Done.\n");

   // Read DOS header
    PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)buff;
    if (dh->e_magic != IMAGE_DOS_SIGNATURE) {
        printf("Invalid PE file\n");
        return FALSE;
    }

    // Read NT headers
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)dh + dh->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
       printf("Invalid NT headers\n");
        return FALSE;
    }


    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
for (int i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {

    SIZE_T rawSize = section->SizeOfRawData;

    printf("section: %s\n", section->Name);
    if (strcmp(section->Name, ".data") == 0 || strcmp(section->Name, ".text") == 0) {
    
        printf("Mapped section: %.*s\n", 8, section->Name);

        if (memcmp(section->Name, ".text", 5) == 0) {
        BYTE* textStart = (BYTE*)buff + section->VirtualAddress;
        SIZE_T textSize = section->Misc.VirtualSize;  // More accurate than SizeOfRawData
        DWORD oldProtect = 0;
        printf(".text start: %x", textStart);
        printf("size: %lu\n", section->SizeOfRawData);


        for (int i=0; i < textSize; i++) {
            if (textStart[i] == 232) {
            printf("\x1b[32m\\%02X\x1b[0m", textStart[i]);
            continue;
            }

            if (textStart[i] == 0xE8) { // CALL
            printf("\x1b[32m\\%02X\x1b[0m", textStart[i]);
            continue;
            }
            if (textStart[i] == 0xC3 || textStart[i] == 0xC2) { // RET
            printf("\x1b[36m\\%02X\x1b[0m", textStart[i]);
            continue;
            }
            if (textStart[i] == 0xCC) { // INT3
            printf("\x1b[31m\\%02X\x1b[0m", textStart[i]);
            continue;
            }
            if (textStart[i] == 0xEB || textStart[i] == 0xE9) { // JMP
            printf("\x1b[35m\\%02X\x1b[0m", textStart[i]); 
            continue;
            }
            if (textStart[i] == 0x74 || textStart[i] == 0x75) { // JE / JNE
            printf("\x1b[33m\\%02X\x1b[0m", textStart[i]); 
            continue;
            }
            if (textStart[i] == 0x55) { // PUSH EBP
            printf("\x1b[94m\\%02X\x1b[0m", textStart[i]);
            continue;
            }
            if (textStart[i] == 0x8B) { // MOV reg
            printf("\x1b[92m\\%02X\x1b[0m", textStart[i]);
            continue;
            }
            if (textStart[i] == 0xCD) { // INT n
            printf("\x1b[91m\\%02X\x1b[0m", textStart[i]);
            continue;
            }


            printf("\\%02X", textStart[i]);
           
            if (textStart[i] > 250) {

                char buff[10];
                printf("\n");
                fgets(buff, 9, stdin);
                buff[1] = '\0'; 
                if (strcmp(buff, "s") == 0) {
                    break;
                }

                // Or enter
                if (strcmp(buff, "y") == 0) {
                    continue;
                }

                if (strcmp(buff, "q") == 0) {
                    return 0;
                }

                if (strcmp(buff, "h") == 0) {
                    printf("\nRip-Ripper:\ny or Enter = Next\ns = Save\nq = Quit\n");
                }

                printf("\n");
            } 
            
        
        }
        char filename[100];
        snprintf(filename, 99, "%s.bin", argv[1]);

        FILE* file = fopen(filename, "wb");
          size_t textWritten = fwrite(textStart, 1, section->SizeOfRawData, file);
            if (textWritten == 0) {
                printf("Error\n");
                return FALSE;
            }

        break;
    }
}
}




return 0;
}
