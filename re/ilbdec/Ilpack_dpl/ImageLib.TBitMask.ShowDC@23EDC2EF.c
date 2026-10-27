// /Ilpack.dpl : ImageLib.TBitMask.ShowDC@23EDC2EF

void ImageLib_TBitMask_ShowDC_23EDC2EF
               (int *param_1,int *param_2,tagRECT *param_3,undefined4 param_4,HDC param_5)

{
  undefined1 *puVar1;
  HWND hWnd;
  int *in_FS_OFFSET;
  tagRECT *ptVar2;
  undefined1 *puStack_7c;
  undefined1 *puStack_78;
  undefined1 *puStack_74;
  tagRECT local_68;
  int local_58 [3];
  int local_4c;
  undefined1 local_48 [60];
  int *local_c;
  tagRECT *local_8;
  
                    /* 0x1266c  519  ImageLib.TBitMask.ShowDC@23EDC2EF */
  local_8 = param_3;
  if (param_2 == (int *)0x0) {
    local_58[0] = 0;
    local_58[1] = 0;
    local_58[2] = param_1[8];
    local_4c = param_1[9];
    param_2 = local_58;
  }
  puStack_74 = (undefined1 *)(param_2[3] - param_2[1]);
  puStack_78 = (undefined1 *)0x56509310;
  puStack_7c = (undefined1 *)0x552126b5;
  local_c = (int *)GFXE_TDIB16Surface_Create_23EDC2EF
                             (GFXE__TDIB16Surface_816E1192_exref,1,param_2[2] - *param_2);
  puStack_78 = &LAB_55212743;
  puStack_7c = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = (int)&puStack_7c;
  puStack_74 = &stack0xfffffffc;
  if (local_8 == (tagRECT *)0x0) {
    puStack_74 = &stack0xfffffffc;
    hWnd = WindowFromDC(param_5);
    GetClientRect(hWnd,&local_68);
    local_8 = &local_68;
  }
  (**(code **)(*local_c + 0x44))(local_c,0x808080);
  (**(code **)(*local_c + 0x2c))(local_c,local_48);
  (**(code **)(*param_1 + 0x84))(param_1,-*param_2,-param_2[1],local_48);
  ptVar2 = local_8;
  (**(code **)(*local_c + 0x24))(local_c,param_5,0);
  (**(code **)(*local_c + 0x30))();
  puVar1 = puStack_7c;
  *in_FS_OFFSET = (int)ptVar2;
  puStack_7c = &LAB_5521274a;
  System_TObject_Free_23EDC2EF(local_c,ptVar2,puVar1);
  return;
}

