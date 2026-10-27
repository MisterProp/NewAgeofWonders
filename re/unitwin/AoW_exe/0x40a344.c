// /AoW.exe : 0x40a344

void FUN_0040a344(int param_1,int *param_2)

{
  int iVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a420 + 0xc0);
  if (iVar1 != 0) {
    AoWE_TPlayerList_GetPlayers_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
               (int)*(char *)(iVar1 + 0x24));
    cVar2 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if ((cVar2 == '\0') && ((char)param_2[0x2c] != '\0')) {
      *(undefined4 *)(param_1 + 0x230) = 0;
      if (param_2 == DAT_0045b074) {
        (**(code **)(*DAT_0045b074 + 0xac))(DAT_0045b074,0);
        DAT_0045b074 = (int *)0x0;
        AoWWinGeneric_TAOWGenericWindow_SetCursor_23EDC2EF(*(undefined4 *)(param_1 + 0x44),0);
        *(undefined1 *)(param_1 + 0x220) = 6;
        (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))(*(int **)(param_1 + 0x164),0);
        *(undefined1 *)(param_1 + 0x228) = 0;
      }
      else if (DAT_0045b074 == (int *)0x0) {
        if (param_2[3] == -1) {
          (**(code **)(*param_2 + 0xac))(param_2,0);
        }
        else {
          DAT_0045b074 = param_2;
          *(undefined4 *)(param_1 + 0x21c) = 0x32;
          *(undefined4 *)(param_1 + 0x224) = 10;
          iVar3 = AoWE_TItemList_FindItem_23EDC2EF(*(undefined4 *)(iVar1 + 0x74),DAT_0045b074[3]);
          if (iVar3 == 0) {
            iVar3 = AoWE_TItemList_FindItem_23EDC2EF(*(undefined4 *)(iVar1 + 0x70),DAT_0045b074[3]);
          }
          if (iVar3 == 0) {
            iVar4 = AoWE_THero_GetItemsOnGround_23EDC2EF(iVar1);
            if (iVar4 != 0) {
              uVar5 = AoWE_THero_GetItemsOnGround_23EDC2EF(iVar1);
              iVar3 = AoWE_TItemList_FindItem_23EDC2EF(uVar5,DAT_0045b074[3]);
            }
          }
          if (iVar3 == 0) {
            *(undefined1 *)(param_1 + 0x220) = 6;
            (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))(*(int **)(param_1 + 0x164),0);
          }
          else {
            uVar5 = AoWE_TItem_GetImage_23EDC2EF(iVar3);
            *(undefined4 *)(param_1 + 0x230) = uVar5;
            *(undefined1 *)(param_1 + 0x220) = *(undefined1 *)(iVar3 + 0x34);
            if (*(char *)(iVar3 + 0x34) == '\x05') {
              (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))(*(int **)(param_1 + 0x164),1);
            }
            else {
              (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))(*(int **)(param_1 + 0x164),0);
            }
          }
          *(undefined4 *)(param_1 + 0x22c) = 2;
          *(undefined1 *)(param_1 + 0x228) = 1;
        }
      }
      else {
        Sound_PlaySample("SFX\\Events\\NewEvent.Wav");
        if (param_2[3] == -1) {
          if (param_2 == *(int **)(param_1 + 0x128)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],1);
          }
          else if (param_2 == *(int **)(param_1 + 300)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],4);
          }
          else if (param_2 == *(int **)(param_1 + 0x130)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],0);
          }
          else if (param_2 == *(int **)(param_1 + 0x134)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],2);
          }
          else if (param_2 == *(int **)(param_1 + 0x13c)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],5);
          }
          else if (param_2 == *(int **)(param_1 + 0x138)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],3);
          }
          else if (param_2 == *(int **)(param_1 + 0xfc)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],6);
          }
          else if (param_2 == *(int **)(param_1 + 0x104)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],7);
          }
          else if (param_2 == *(int **)(param_1 + 0x108)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],8);
          }
          else if (param_2 == *(int **)(param_1 + 0x10c)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],9);
          }
          else if (param_2 == *(int **)(param_1 + 0x110)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],10);
          }
          else if (param_2 == *(int **)(param_1 + 0x114)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],0xb);
          }
          else if (param_2 == *(int **)(param_1 + 0x118)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],0xc);
          }
          else if (param_2 == *(int **)(param_1 + 0x11c)) {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],0xd);
          }
          else {
            AoWE_THero_PlaceItem_23EDC2EF(iVar1,DAT_0045b074[3],0xe);
          }
        }
        else {
          AoWE_THero_SwapItem_23EDC2EF(iVar1,DAT_0045b074[3],param_2[3]);
        }
        (**(code **)(*param_2 + 0xac))(param_2,0);
        (**(code **)(*DAT_0045b074 + 0xac))(DAT_0045b074,0);
        DAT_0045b074 = (int *)0x0;
        AoWWinGeneric_TAOWGenericWindow_SetCursor_23EDC2EF(*(undefined4 *)(param_1 + 0x44),0);
        *(undefined1 *)(param_1 + 0x220) = 6;
        (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))(*(int **)(param_1 + 0x164),0);
        *(undefined1 *)(param_1 + 0x228) = 0;
      }
      *(undefined4 *)(*(int *)PTR_DAT_0045a168 + 0x1f8) = *(undefined4 *)(param_1 + 0x230);
    }
  }
  return;
}

