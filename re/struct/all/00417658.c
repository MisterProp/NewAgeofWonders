
void FUN_00417658(int param_1)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  int *piVar6;
  undefined4 *in_FS_OFFSET;
  undefined1 *puVar7;
  undefined *puVar8;
  undefined4 uVar9;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_28 = &LAB_00417b5f;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  iVar5 = 0;
  do {
    switch(iVar5) {
    case 0:
      piVar6 = *(int **)(param_1 + 0x200);
      break;
    case 1:
      piVar6 = *(int **)(param_1 + 0x1fc);
      break;
    case 2:
      piVar6 = *(int **)(param_1 + 0x1f8);
      break;
    case 3:
      piVar6 = *(int **)(param_1 + 0x1f0);
      break;
    case 4:
      piVar6 = *(int **)(param_1 + 0x204);
      break;
    case 5:
      piVar6 = *(int **)(param_1 + 0x208);
      break;
    case 6:
      piVar6 = *(int **)(param_1 + 0x20c);
      break;
    default:
      piVar6 = (int *)0x0;
    }
    iVar2 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
    if (iVar5 < iVar2) {
      (**(code **)(*piVar6 + 0x6c))(piVar6,1);
      uVar3 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),iVar5);
      switch(uVar3 & 0x7f) {
      case 1:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x1c);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x1d);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x1b);
        break;
      case 2:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x1f);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x20);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x1e);
        break;
      case 3:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x22);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x23);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x21);
        break;
      case 4:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x25);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x26);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x24);
        break;
      case 5:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x2b);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x2c);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x2a);
        break;
      case 6:
        (**(code **)(*piVar6 + 0xa4))(piVar6,0x28);
        (**(code **)(*piVar6 + 0xa8))(piVar6,0x29);
        (**(code **)(*piVar6 + 0xa0))(piVar6,0x27);
      }
    }
    else {
      (**(code **)(*piVar6 + 0x6c))(piVar6,0);
    }
    if (iVar5 < *(int *)(param_1 + 0x2e0)) {
      *(undefined1 *)(piVar6 + 0x30) = 0;
      (**(code **)(*piVar6 + 0xa0))(piVar6,piVar6[0x49] + 1);
    }
    else {
      *(undefined1 *)(piVar6 + 0x30) = 1;
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 != 7);
  System_LoadResString_02614E1B(AoWE_SelectedSpheresRStr_04ECFCC5_exref,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  uVar4 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
  SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);
  puVar8 = &DAT_00417b84;
  uVar9 = local_10;
  SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(*(int *)(param_1 + 0x2e4) + 0x9c),&local_14);
  puVar7 = &LAB_00417b90;
  uVar4 = local_14;
  System__LStrCatN_51F89FF7(&local_8,6);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x224),local_8);
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),1);
  if (cVar1 == '\0') {
    iVar5 = **(int **)(param_1 + 0x1d8);
    (**(code **)(iVar5 + 0xa0))(*(int **)(param_1 + 0x1d8),0x1c,iVar5,puVar7,uVar4,puVar8,uVar9);
    *(undefined1 *)(*(int *)(param_1 + 0x1d8) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1d8) + 0xa0))(*(int **)(param_1 + 0x1d8),0x1b);
    *(undefined1 *)(*(int *)(param_1 + 0x1d8) + 0xc0) = 1;
  }
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),2);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1d0) + 0xa0))(*(int **)(param_1 + 0x1d0),0x1f);
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1d0) + 0xa0))(*(int **)(param_1 + 0x1d0),0x1e);
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xc0) = 1;
  }
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),5);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1dc) + 0xa0))(*(int **)(param_1 + 0x1dc),0x2b);
    *(undefined1 *)(*(int *)(param_1 + 0x1dc) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1dc) + 0xa0))(*(int **)(param_1 + 0x1dc),0x2a);
    *(undefined1 *)(*(int *)(param_1 + 0x1dc) + 0xc0) = 1;
  }
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),6);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1e8) + 0xa0))(*(int **)(param_1 + 0x1e8),0x28);
    *(undefined1 *)(*(int *)(param_1 + 0x1e8) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1e8) + 0xa0))(*(int **)(param_1 + 0x1e8),0x27);
    *(undefined1 *)(*(int *)(param_1 + 0x1e8) + 0xc0) = 1;
  }
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),3);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1e0) + 0xa0))(*(int **)(param_1 + 0x1e0),0x22);
    *(undefined1 *)(*(int *)(param_1 + 0x1e0) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1e0) + 0xa0))(*(int **)(param_1 + 0x1e0),0x21);
    *(undefined1 *)(*(int *)(param_1 + 0x1e0) + 0xc0) = 1;
  }
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),4);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1e4) + 0xa0))(*(int **)(param_1 + 0x1e4),0x25);
    *(undefined1 *)(*(int *)(param_1 + 0x1e4) + 0xc0) = 0;
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1e4) + 0xa0))(*(int **)(param_1 + 0x1e4),0x24);
    *(undefined1 *)(*(int *)(param_1 + 0x1e4) + 0xc0) = 1;
  }
  *(undefined1 *)(*(int *)(param_1 + 0x1d8) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x1dc) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x1e8) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x1e0) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x1e4) + 0xa4) = 0;
  *in_FS_OFFSET = puVar7;
  System__LStrArrayClr_51F89FF7(&local_14,4,puVar8,&LAB_00417b66);
  return;
}

