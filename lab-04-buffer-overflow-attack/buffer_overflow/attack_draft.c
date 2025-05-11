#define adminMode *(char *)0x4080d4e8
register unsigned int ra __asm__("ra");

void main()
{
    adminMode = 1;
    ra = 0x4200b788;
}