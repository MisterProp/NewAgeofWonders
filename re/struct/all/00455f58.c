
void FUN_00455f58(undefined4 param_1)

{
  int *piVar1;
  code *pcVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 auStack_1c [4];
  
  pcVar2 = AoWE_AoWEngine_7E705837_exref;
  bVar6 = 0;
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),6,&local_2c,param_1);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a1c0 + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x20) = 0;
    *(undefined4 *)(iVar3 + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x20) = local_28;
    *(undefined4 *)(iVar3 + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))();
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),0,&local_2c,param_1);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a4fc + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x24) = 0;
    *(undefined4 *)(iVar3 + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x24) = local_28;
    *(undefined4 *)(iVar3 + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))();
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),2,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a53c + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 4;
    *(undefined1 *)(iVar3 + 0x29) = 4;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 1;
    *(undefined1 *)(iVar3 + 0x29) = 1;
    *(undefined4 *)(iVar3 + 0x20) = local_28;
    *(undefined4 *)(iVar3 + 0x18) = local_2c;
    (**(code **)(*piVar1 + 0x60))();
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),3,&local_2c);
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),4,&local_2c);
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),5,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a0b0 + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x24) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
    *(undefined4 *)(iVar3 + 0x18) = 0;
    *(undefined4 *)(iVar3 + 0x1c) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x7c);
    *(undefined1 *)(iVar3 + 0x28) = 3;
    *(undefined1 *)(iVar3 + 0x29) = 2;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 1;
    *(undefined1 *)(iVar3 + 0x29) = 1;
    *(undefined4 *)(iVar3 + 0x20) = local_28;
    *(undefined4 *)(iVar3 + 0x18) = local_2c;
    (**(code **)(*piVar1 + 0x60))();
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),0xd,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a394 + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined4 *)(iVar3 + 0x24) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
    *(undefined4 *)(iVar3 + 0x18) = 0;
    *(undefined4 *)(iVar3 + 0x1c) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x7c);
    *(undefined1 *)(iVar3 + 0x28) = 3;
    *(undefined1 *)(iVar3 + 0x29) = 2;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 1;
    *(undefined1 *)(iVar3 + 0x29) = 1;
    *(undefined4 *)(iVar3 + 0x20) = local_28;
    *(undefined4 *)(iVar3 + 0x18) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),1,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a084 + 0x44);
  if (local_24 == 0) {
    *(undefined4 *)(piVar1[0x35] + 0x20) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x80);
    *(undefined4 *)(piVar1[0x35] + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    *(undefined4 *)(piVar1[0x35] + 0x20) = local_28;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),7,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a5c4 + 0x44);
  if (local_24 == 0) {
    *(undefined4 *)(piVar1[0x35] + 0x20) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x80);
    *(undefined4 *)(piVar1[0x35] + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    *(undefined4 *)(piVar1[0x35] + 0x20) = local_28;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),8,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a490 + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 3;
    *(undefined4 *)(iVar3 + 0x18) = 0;
    *(undefined4 *)(iVar3 + 0x1c) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x154);
    *(undefined4 *)(iVar3 + 0x24) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x28) = 1;
    *(undefined4 *)(iVar3 + 0x24) = local_28;
    *(undefined4 *)(iVar3 + 0x18) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),0xc,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a098 + 0x44);
  if (local_24 == 0) {
    *(undefined4 *)(piVar1[0x35] + 0x20) = 0;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    *(undefined4 *)(piVar1[0x35] + 0x20) = local_28;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),10,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a358 + 0x44);
  if (local_24 == 0) {
    *(undefined4 *)(piVar1[0x35] + 0x24) = 0;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = 0;
    (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
    (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  }
  else {
    *(undefined4 *)(piVar1[0x35] + 0x24) = local_28;
    *(undefined4 *)(piVar1[0x35] + 0x1c) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),0xb,&local_2c);
  if (local_24 == 0) {
    *(undefined1 *)(*(int *)PTR_DAT_0045a5bc + 0x6f) = 3;
    *(undefined1 *)(*(int *)PTR_DAT_0045a5bc + 0x6d) = 3;
    Classes_Rect_9923E97E
              (0,0,*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x154),auStack_1c,
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80));
    puVar4 = auStack_1c;
    puVar5 = (undefined4 *)(*(int *)PTR_DAT_0045a5bc + 0x5c);
    for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
    }
  }
  else {
    puVar4 = &local_2c;
    puVar5 = (undefined4 *)(*(int *)PTR_DAT_0045a5bc + 0x5c);
    for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
    }
    *(undefined1 *)(*(int *)PTR_DAT_0045a5bc + 0x6f) = 1;
    *(undefined1 *)(*(int *)PTR_DAT_0045a5bc + 0x6d) = 1;
  }
  AoWReg_TAoWRegistry_GetRectangle_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),9,&local_2c);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a5bc + 0x44);
  if (local_24 == 0) {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x29) = 3;
    *(undefined1 *)(iVar3 + 0x28) = 3;
    *(undefined4 *)(iVar3 + 0x20) = 0;
    *(undefined4 *)(iVar3 + 0x18) = 0;
    *(undefined4 *)(iVar3 + 0x1c) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x154);
    *(undefined4 *)(iVar3 + 0x24) =
         *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
  }
  else {
    iVar3 = piVar1[0x35];
    *(undefined1 *)(iVar3 + 0x29) = 1;
    *(undefined1 *)(iVar3 + 0x28) = 1;
    *(undefined4 *)(iVar3 + 0x20) = local_28;
    *(undefined4 *)(iVar3 + 0x18) = local_2c;
    (**(code **)(*piVar1 + 0x60))(piVar1,local_24);
    (**(code **)(*piVar1 + 100))(piVar1,local_20);
  }
  return;
}

