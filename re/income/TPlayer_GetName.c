

void AoWE_TPlayer_GetName_23EDC2EF(int param_1,int *param_2)



{

  int *piVar1;

  int iVar2;

  undefined1 *puVar3;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_24;

  undefined1 *puStack_20;

  undefined1 *puStack_1c;

  undefined4 local_c;

  int local_8;

  

                    /* 0x52104  2618  AoWE.TPlayer.GetName@23EDC2EF */

  puStack_1c = &stack0xfffffffc;

  local_8 = 0;

  local_c = 0;

  puStack_20 = &LAB_557521fe;

  uStack_24 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_24;

  if (*(char *)(param_1 + 0xa6) == '\0') {

    System_LoadResString_02614E1B(&AoWE_IndependentsRStr_8276FCC3,&local_8);

    AoWE_TranslateRStr_435799AA(local_8,param_2);

  }

  else {

    piVar1 = *(int **)(param_1 + 0xd4);

    if (piVar1 == (int *)0x0) {

      puStack_1c = &stack0xfffffffc;

      System__LStrLAsg_51F89FF7(param_2,*(undefined4 *)(param_1 + 0x4c));

    }

    else {

      puStack_1c = &stack0xfffffffc;

      System__LStrLAsg_51F89FF7(param_2,piVar1[0x18]);

      (**(code **)(*piVar1 + 0x1b8))(piVar1,&local_8);

      if (local_8 != 0) {

        iVar2 = **(int **)(param_1 + 0xd4);

        (**(code **)(iVar2 + 0x1b8))

                  (*(int **)(param_1 + 0xd4),&local_8,iVar2,&DAT_55752214,*param_2);

        System__LStrCatN_51F89FF7(param_2,3);

      }

    }

    if (*param_2 == 0) {

      System_LoadResString_02614E1B(&AoWE_PlayerRStr_8276FCC3,&local_c);

      AoWE_TranslateRStr_435799AA(local_c,&local_8);

      SysUtils_IntToStr_0F6FDFF6((int)*(char *)(param_1 + 0xa6),&local_c);

      System__LStrCatN_51F89FF7(param_2,3);

    }

  }

  puVar3 = puStack_1c;

  *in_FS_OFFSET = uStack_24;

  puStack_1c = &LAB_55752205;

  puStack_20 = (undefined1 *)0x557521fd;

  System__LStrArrayClr_51F89FF7(&local_c,2,puVar3);

  return;

}




