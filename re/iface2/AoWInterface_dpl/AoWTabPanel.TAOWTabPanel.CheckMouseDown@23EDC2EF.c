// /AoWInterface.dpl : AoWTabPanel.TAOWTabPanel.CheckMouseDown@23EDC2EF

void AoWTabPanel_TAOWTabPanel_CheckMouseDown_23EDC2EF
               (int param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int local_14;
  int local_c;
  
                    /* 0x14d58  313  AoWTabPanel.TAOWTabPanel.CheckMouseDown@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseDown_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  *(undefined4 *)(param_1 + 0x6c) = 0;
  iVar3 = *(int *)(param_1 + 0x108);
  iVar6 = *(int *)(iVar3 + 0x80) + *(int *)(param_1 + 0x80);
  if ((((iVar6 + -1 < param_5) && (param_5 < iVar6 + *(int *)(param_1 + 0x78))) &&
      (*(int *)(iVar3 + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_4)) &&
     (param_4 < *(int *)(iVar3 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c))) {
    *(undefined4 *)(param_1 + 0x6c) = 0xffffffff;
    if ((char)param_2 == '\0') {
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
      iVar3 = *(int *)(iVar3 + 0x20);
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
      iVar6 = *(int *)(iVar6 + 0x20);
      bVar1 = *(byte *)(param_1 + 0x9c);
      iVar2 = *(int *)(param_1 + 0xa0);
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
      iVar3 = ((param_5 - *(int *)(*(int *)(param_1 + 0x108) + 0x80)) - *(int *)(param_1 + 0x80)) /
              (int)(iVar3 + iVar6 * (uint)bVar1 + iVar2 + *(int *)(iVar4 + 0x20));
      if (iVar3 != *(int *)(param_1 + 0x114)) {
        *(int *)(param_1 + 0x114) = iVar3;
        local_14 = *(int *)(*(int *)(param_1 + 0x120) + 8);
        if (-1 < local_14 + -1) {
          local_c = 0;
          do {
            piVar5 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_c);
            if (piVar5[0x39] == *(int *)(param_1 + 0x114)) {
              (**(code **)(*piVar5 + 0x44))(piVar5,1);
            }
            else {
              (**(code **)(*piVar5 + 0x44))(piVar5,0);
            }
            local_c = local_c + 1;
            local_14 = local_14 + -1;
          } while (local_14 != 0);
        }
        if (*(short *)(param_1 + 0x62) != 0) {
          (**(code **)(param_1 + 0x60))(*(undefined4 *)(param_1 + 100),param_1);
        }
        *(undefined1 *)(param_1 + 0xd2) = 0;
      }
    }
    if (*(short *)(param_1 + 0x3a) != 0) {
      (**(code **)(param_1 + 0x38))
                (*(undefined4 *)(param_1 + 0x3c),param_1,param_2 & 0xff,param_4,param_5,
                 param_3 & 0xff);
    }
  }
  return;
}

