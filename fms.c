#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void getPassword(char pass[])
{
    char ch;
    int i = 0;

    while (1)
    {
        ch = getch();

        if (ch == 13)    
        {
            pass[i] = '\0';
            break;
        }
        else if (ch == 8) 
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else
        {
            pass[i] = ch;
            i++;
            printf("*");
        }
    }
}

int calculateKey(char pass[])
{
    int i;
    int hash = 0;

    for (i = 0; pass[i] != '\0'; i++)
    {
        hash = hash * 31 + pass[i];
    }

    if (hash < 0)
        hash = -hash;

    return hash;
}
void procFile(char InputFile[], char OutputFile[], char password[])
{
    FILE *fptr1, *fptr2;
    int ch;
    int i = 0;
    int keyLen;

    keyLen = strlen(password);

    fptr1 = fopen(InputFile, "rb");

    if (fptr1 == NULL)
    {
        printf("\nError: Cannot Open Input File!");
        return;
    }

    fptr2 = fopen(OutputFile, "wb");

    if (fptr2 == NULL)
    {
        printf("\nError: Cannot Create Output File!");
        fclose(fptr1);
        return;
    }

    while ((ch = fgetc(fptr1)) != EOF)
    {
        ch = ch ^ password[i];

        fputc(ch, fptr2);

        i++;

        if (i == keyLen)
        {
            i = 0;
        }
    }

    fclose(fptr1);
    fclose(fptr2);
}
int isEncrypted(char filename[])
{
    FILE *fp;
    char check[5];

    fp = fopen(filename, "rb");

    if (fp == NULL)
        return 0;

    fgets(check, 5, fp);
    fclose(fp);

    if (strcmp(check, "KEY:") == 0)
        return 1;
    else
        return 0;
}

void createFile()
{
    FILE *fp;
    char filename[50];

    printf("\nEnter file name: ");
    scanf("%s", filename);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        printf("\nFile cannot be created.");
        return;
    }

    printf("\nFile created successfully.");
    fclose(fp);
}

void writeFile()
{
    FILE *fp;
    char filename[50];
    char text[200];

    printf("\nEnter file name: ");
    scanf("%s", filename);

    if (isEncrypted(filename))
    {
        printf("\nFile is encrypted. Decrypt it before writing data.");
        return;
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    printf("\nEnter text to write in file.");
    printf("\nType END to stop writing.\n\n");

    fflush(stdin);

    while (1)
    {
        gets(text);

        if (strcmp(text, "END") == 0)
            break;

        fprintf(fp, "%s\n", text);
    }

    printf("\nData written successfully.");
    fclose(fp);
}

void viewFile()
{
    FILE *fp;
    char filename[50];
    int ch;

    printf("\nEnter file name: ");
    scanf("%s", filename);

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    if (isEncrypted(filename))
    {
        printf("\n\n----- Encrypted File Content -----\n\n");
        printf("NOTE: This is encrypted unreadable text.\n\n");
    }
    else
    {
        printf("\n\n----- File Content -----\n\n");
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        putchar(ch);
    }

    fclose(fp);
}
void editFile()
{
    FILE *fp;
    char filename[50];
    char text[200];

    printf("\nEnter file name to edit: ");
    scanf("%s", filename);

    if (isEncrypted(filename))
    {
        printf("\nFile is encrypted. Decrypt it before editing.");
        return;
    }

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    printf("\nOld content will be replaced.");
    printf("\nEnter new text.");
    printf("\nType END to stop writing.\n\n");

    fflush(stdin);

    while (1)
    {
        gets(text);

        if (strcmp(text, "END") == 0)
            break;

        fprintf(fp, "%s\n", text);
    }

    printf("\nFile edited successfully.");
    fclose(fp);
}

void encryptFile()
{
    FILE *fp, *temp;
    char filename[50];
    char pass[50], confirmPass[50];
    int keySum;
    int passLen;

    printf("\nEnter file name: ");
    scanf("%s", filename);

    if (isEncrypted(filename))
    {
        printf("\nThis file is already encrypted.");
        return;
    }

    printf("\nEnter passkey: ");
    getPassword(pass);

    printf("\nConfirm passkey: ");
    getPassword(confirmPass);

    if (strcmp(pass, confirmPass) != 0)
    {
        printf("\nPasskey does not match.");
        return;
    }

    passLen = strlen(pass);

    if (passLen == 0)
    {
        printf("\nPasskey cannot be empty.");
        return;
    }

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    fclose(fp);

    keySum = calculateKey(pass);

    temp = fopen("tempfile.tmp", "wb");

    if (temp == NULL)
    {
        printf("\nTemporary file cannot be created.");
        return;
    }

    fprintf(temp, "KEY:%d\n", keySum);
    fclose(temp);

    procFile(filename, "encrypted.tmp", pass);

    fp = fopen("encrypted.tmp", "rb");
    temp = fopen("tempfile.tmp", "ab");

    if (fp == NULL || temp == NULL)
    {
        printf("\nError while encrypting file.");
        return;
    }

    while (!feof(fp))
    {
        int ch = fgetc(fp);

        if (ch != EOF)
            fputc(ch, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("encrypted.tmp");
    remove(filename);
    rename("tempfile.tmp", filename);

    printf("\nFile encrypted successfully.");
}

void decryptFile()
{
    FILE *fp, *temp;
    char filename[50];
    char pass[50];
    int savedKey;
    int keySum;
    int passLen;
    int ch;

    printf("\nEnter file name: ");
    scanf("%s", filename);

    if (!isEncrypted(filename))
    {
        printf("\nThis file is not encrypted.");
        return;
    }

    printf("\nEnter passkey: ");
    getPassword(pass);

    passLen = strlen(pass);

    if (passLen == 0)
    {
        printf("\nPasskey cannot be empty.");
        return;
    }

    keySum = calculateKey(pass);

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    if (fscanf(fp, "KEY:%d\n", &savedKey) != 1)
    {
        printf("\nInvalid encrypted file format.");
        fclose(fp);
        return;
    }

    if (savedKey != keySum)
    {
        printf("\nWrong passkey. Decryption failed.");
        fclose(fp);
        return;
    }

    temp = fopen("encrypteddata.tmp", "wb");

    if (temp == NULL)
    {
        printf("\nTemporary file cannot be created.");
        fclose(fp);
        return;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        fputc(ch, temp);
    }

    fclose(fp);
    fclose(temp);

    procFile("encrypteddata.tmp", "decrypted.tmp", pass);

    remove(filename);
    rename("decrypted.tmp", filename);
    remove("encrypteddata.tmp");

    printf("\nFile decrypted successfully.");
}
void searchInFile()
{
    FILE *fp;
    char filename[50];
    char word[50];
    char temp[50];
    int count = 0;

    printf("\nEnter file name: ");
    scanf("%s", filename);

    if (isEncrypted(filename))
    {
        printf("\nFile is encrypted. Decrypt it before searching.");
        return;
    }

    printf("\nEnter word to search: ");
    scanf("%s", word);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    while (fscanf(fp, "%s", temp) != EOF)
    {
        if (strcmp(temp, word) == 0)
        {
            count++;
        }
    }

    fclose(fp);

    if (count > 0)
        printf("\nWord found %d time(s).", count);
    else
        printf("\nWord not found.");
}

void fileStatistics()
{
    FILE *fp;
    char filename[50];
    int ch;
    int characters = 0;
    int words = 0;
    int lines = 0;
    int inWord = 0;

    printf("\nEnter file name: ");
    scanf("%s", filename);

    if (isEncrypted(filename))
    {
        printf("\nFile is encrypted. Decrypt it before checking statistics.");
        return;
    }

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("\nFile not found.");
        return;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        characters++;

        if (ch == '\n')
            lines++;

        if (ch == ' ' || ch == '\n' || ch == '\t')
        {
            inWord = 0;
        }
        else if (inWord == 0)
        {
            inWord = 1;
            words++;
        }
    }

    fclose(fp);

    printf("\n----- File Statistics -----");
    printf("\nCharacters: %d", characters);
    printf("\nWords     : %d", words);
    printf("\nLines     : %d", lines);
}

void renameFile()
{
    char oldName[50];
    char newName[50];

    printf("\nEnter old file name: ");
    scanf("%s", oldName);

    printf("Enter new file name: ");
    scanf("%s", newName);

    if (rename(oldName, newName) == 0)
        printf("\nFile renamed successfully.");
    else
        printf("\nUnable to rename file.");
}

void deleteFile()
{
    char filename[50];
    char confirm;

    printf("\nEnter file name to delete: ");
    scanf("%s", filename);

    printf("\nAre you sure you want to delete this file? (y/n): ");
    fflush(stdin);
    scanf("%c", &confirm);

    if (confirm == 'y' || confirm == 'Y')
    {
        if (remove(filename) == 0)
            printf("\nFile deleted successfully.");
        else
            printf("\nUnable to delete file.");
    }
    else
    {
        printf("\nDelete operation cancelled.");
    }
}

void showMenu()
{
    printf("\n========================================");
    printf("\n        SECURE FILE MANAGEMENT SYSTEM");
    printf("\n========================================");
    printf("\n1. Create File");
    printf("\n2. Write Data");
    printf("\n3. View File");
    printf("\n4. Edit File");
    printf("\n5. Encrypt File");
    printf("\n6. Decrypt File");
    printf("\n7. Search in File");
    printf("\n8. File Statistics");
    printf("\n9. Rename File");
    printf("\n10. Delete File");
    printf("\n11. Exit");
    printf("\n========================================");
}

void main()
{
    int choice;

    clrscr();

    while (1)
    {
        showMenu();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                createFile();
                break;

            case 2:
                writeFile();
                break;

            case 3:
                viewFile();
                break;

            case 4:
                editFile();
                break;

            case 5:
                encryptFile();
                break;

            case 6:
                decryptFile();
                break;

            case 7:
                searchInFile();
                break;

            case 8:
                fileStatistics();
                break;

            case 9:
                renameFile();
                break;

            case 10:
                deleteFile();
                break;

            case 11:
                printf("\nExiting program...");
                getch();
                return;

            default:
                printf("\nInvalid choice.");
        }

        printf("\n\nPress any key to continue...");
        getch();
        clrscr();
    }
}