
/* WARNING: Removing unreachable block (ram,0x00435834) */

void FUN_00435798(int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int local_1c;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if ((*(byte *)(*(int *)(param_1 + 0x144) + 4) & 1) != 0) {
    iVar1 = *(int *)(*(int *)(param_1 + 0x144) + 8);
    local_c = param_3;
    local_8 = param_2;
    local_1c = AoWE_TCombatData_GetCount_23EDC2EF(*(undefined4 *)(iVar1 + 0xc));
    if (-1 < local_1c + -1) {
      iVar6 = 0;
      do {
        iVar2 = AoWE_TCombatData_GetObjects_23EDC2EF(*(undefined4 *)(iVar1 + 0xc),iVar6);
        if (*(byte *)(iVar2 + 0x46) != 0x80) {
          FUN_004356a0(param_1,*(byte *)(iVar2 + 0x46) >> 4,&local_10,&local_14);
          iVar3 = local_14 + local_c;
          local_14 = iVar3 + -0x20;
          uVar4 = (uint)(*(byte *)(iVar2 + 0x46) & 0xf);
          iVar2 = local_10 + local_8 + -0xf + (uVar4 & 0x80000003) * 0x28;
          if (3 < uVar4) {
            local_14 = iVar3 + 6;
          }
          uVar8 = 3;
          uVar7 = param_4;
          iVar3 = local_14;
          local_10 = iVar2;
          uVar5 = AoWE_TCombatData_GetObjects_23EDC2EF(*(undefined4 *)(iVar1 + 0xc),iVar6);
          AoWE_TCombatViewer_ShowFieldObject_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x144),uVar5,*(undefined4 *)(param_1 + 300),uVar7,
                     iVar3,iVar2,uVar8);
        }
        iVar6 = iVar6 + 1;
        local_1c = local_1c + -1;
      } while (local_1c != 0);
    }
  }
  return;
}

