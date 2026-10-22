
void FUN_0045660c(void)

{
  int iVar1;
  int iVar2;
  undefined *puVar3;
  code *pcVar4;
  undefined1 local_20 [16];
  
  pcVar4 = AoWE_AoWEngine_7E705837_exref;
  puVar3 = PTR_DAT_0045a5bc;
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a4fc + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x24),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a4fc + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a4fc + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0,local_20);
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a084 + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x20),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a084 + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a084 + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),1,local_20);
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a53c + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  if (*(char *)(iVar2 + 0x28) == '\x04') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),2,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x20),
               *(undefined4 *)(iVar1 + 0x7c),local_20,*(undefined4 *)(iVar1 + 0x80));
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),2,local_20);
  }
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a0b0 + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  if (*(char *)(iVar2 + 0x28) == '\x03') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),5,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x20),
               *(undefined4 *)(iVar1 + 0x7c),local_20,*(undefined4 *)(iVar1 + 0x80));
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),5,local_20);
  }
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a394 + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  if (*(char *)(iVar2 + 0x28) == '\x03') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0xd,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x20),
               *(undefined4 *)(iVar1 + 0x7c),local_20,*(undefined4 *)(iVar1 + 0x80));
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0xd,local_20);
  }
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x20),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),6,local_20);
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5c4 + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x20),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a5c4 + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a5c4 + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),7,local_20);
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a490 + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  if (*(char *)(iVar2 + 0x28) == '\x03') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),8,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x24),
               *(undefined4 *)(iVar1 + 0x7c),local_20,*(undefined4 *)(iVar1 + 0x80));
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),8,local_20);
  }
  iVar1 = *(int *)(*(int *)puVar3 + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  if (*(char *)(iVar2 + 0x28) == '\x03') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),9,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x20),
               *(undefined4 *)(iVar1 + 0x7c),local_20,*(undefined4 *)(iVar1 + 0x80));
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),9,local_20);
  }
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x20),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0xc,local_20);
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a358 + 0x44) + 0xd4);
  Classes_Rect_9923E97E
            (*(undefined4 *)(iVar1 + 0x1c),*(undefined4 *)(iVar1 + 0x24),
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a358 + 0x44) + 0x7c),local_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a358 + 0x44) + 0x80));
  AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),10,local_20);
  if (*(char *)(*(int *)puVar3 + 0x6d) == '\x03') {
    Classes_Rect_9923E97E(0,0,0,local_20,0);
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0xb,local_20);
  }
  else {
    Classes_Rect_9923E97E
              (*(undefined4 *)(*(int *)puVar3 + 0x5c),*(undefined4 *)(*(int *)puVar3 + 0x60),
               *(undefined4 *)(*(int *)puVar3 + 100),local_20,*(undefined4 *)(*(int *)puVar3 + 0x68)
              );
    AoWReg_TAoWRegistry_SetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar4 + 0x5c),0xb,local_20);
  }
  return;
}

