// aowepack : AoWE.TCity.Show.c
// Ghidra 12.1.2 decompilation of AoWEPACK.dpl, function @ 0x557aca88.
// A city's draw entry. Converts its hex with HSEngine.HXtoHP and hands
// the raw HXtoHP position to the resource's ShowCity (via
// TCityResource.ShowCity, or the plain resource Show when razed/building).
// It also draws the owner flag at resource+0x68/+0x6c. So a city's anchor
// is HXtoHP (no -8), exactly like AoWE.TStructure.Show@5575f958.

void City_TCity_Show_23EDC2EF(int param_1,int param_2,int param_3,undefined4 param_4)

{
  char cVar1;
  int iVar2;
  int local_c;
  int local_8;

                    /* 0xaca88  6851  City.TCity.Show@23EDC2EF */
  HSEngine_HXtoHP_8E9B8701
            ((int)*(char *)(param_1 + 0x10),(int)*(char *)(param_1 + 0x11),&local_8,&local_c);
  param_2 = local_8 - param_2;
  param_3 = local_c - param_3;
  if ((*(byte *)(param_1 + 0x44) & 2) != 0) {
    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);
    if (cVar1 == '\0') goto LAB_557acb75;
  }
  if (((((*(byte *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x3c) & 2) != 0) ||
       (*(char *)(param_1 + 0x2c) == '\0')) ||
      (*(char *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 200) == '\0')) &&
     ((*(char *)(param_1 + 0x54) != '?' &&
      ((*(char *)(*(int *)(param_1 + 8) + 100) != '\0' || ((*(byte *)(param_1 + 0x54) & 0x20) == 0))
      )))) {
    iVar2 = *(int *)(param_1 + 8);
    if (*(byte *)(iVar2 + 0x46) < 2) {
      local_8 = *(int *)(*(int *)(*(int *)(iVar2 + 0x18) + 0x3c) + 0x40) + local_8;
      AoWE_TFlagControl_ShowFlag_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x78),
                 *(int *)(iVar2 + 0x68) + param_2,*(int *)(iVar2 + 0x6c) + param_3,param_4,
                 CONCAT31((int3)((uint)local_8 >> 8),*(undefined1 *)(param_1 + 0x54)),local_8);
    }
    else {
      local_8 = *(int *)(*(int *)(*(int *)(iVar2 + 0x18) + 0x3c) + 0x40) + local_8;
      AoWE_TFlagControl_ShowStructureFlag_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x78),
                 *(int *)(iVar2 + 0x68) + param_2,*(int *)(iVar2 + 0x6c) + param_3,param_4,
                 CONCAT31((int3)((uint)local_8 >> 8),*(undefined1 *)(param_1 + 0x54)),local_8);
    }
  }
LAB_557acb75:
  if ((*(byte *)(param_1 + 0x44) & 4) != 0) {
    iVar2 = System__RandInt_51F89FF7(5);
    param_2 = param_2 + iVar2 + -2;
    iVar2 = System__RandInt_51F89FF7(5);
    param_3 = param_3 + iVar2 + -2;
  }
  if ((*(byte *)(param_1 + 0x44) & 2) == 0) {
    City_TCityResource_ShowCity_23EDC2EF
              (*(undefined4 *)(param_1 + 8),param_2,param_3,param_1,param_4);
  }
  else {
    (**(code **)(**(int **)(param_1 + 8) + 0x78))
              (*(int **)(param_1 + 8),param_2,param_3,param_1,param_4);
  }
  return;
}
