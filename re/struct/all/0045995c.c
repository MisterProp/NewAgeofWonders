
void entry(void)

{
  code *pcVar1;
  undefined1 *puVar2;
  char cVar3;
  HANDLE hObject;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14 [4];
  
  local_14[0] = 0;
  local_18 = 0;
  puStack_24 = (undefined1 *)0x459976;
  FUN_004011fc(&LAB_00458f1c);
  pcVar1 = Forms_Application_E55EEEF8_exref;
  puStack_28 = &LAB_00459fd9;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  puStack_24 = &stack0xfffffffc;
  hObject = CreateFileMappingA((HANDLE)0xffffffff,(LPSECURITY_ATTRIBUTES)0x0,2,0,0x20,"AOW");
  cVar3 = FUN_00458e5c();
  if (cVar3 != '\0') {
    cVar3 = FUN_00458dcc();
    if (cVar3 != '\0') {
      System_LoadResString_02614E1B(AoWE_AgeOfWondersRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,local_14);
      Forms_TApplication_SetTitle_23EDC2EF(*(undefined4 *)pcVar1,local_14[0]);
      Forms_TApplication_Initialize_23EDC2EF(*(undefined4 *)pcVar1);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004039e8,PTR_DAT_0045a4a0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040380c,PTR_DAT_0045a210);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00453fc4,PTR_DAT_0045a420);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004445d8,PTR_DAT_0045a44c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0041375c,PTR_DAT_0045a454);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044e554,PTR_DAT_0045a130);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00458004,PTR_DAT_0045a398);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00429864,PTR_DAT_0045a28c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00427450,PTR_DAT_0045a3fc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00456cac,PTR_DAT_0045a0cc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004519a0,PTR_DAT_0045a1c0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00450c38,PTR_DAT_0045a5bc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00441930,PTR_DAT_0045a5c4);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00450334,PTR_DAT_0045a5fc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00404910,PTR_DAT_0045a084);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00403d84,PTR_DAT_0045a1d0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040710c,PTR_DAT_0045a53c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00412d5c,PTR_DAT_0045a490);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0042a6c4,PTR_DAT_0045a598);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0041eb60,PTR_DAT_0045a4c8);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00422ec8,PTR_DAT_0045a4fc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004131a0,PTR_DAT_0045a24c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00420e44,PTR_DAT_0045a158);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040add4,PTR_DAT_0045a478);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00434cd4,PTR_DAT_0045a4f0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004290b0,PTR_DAT_0045a2f0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004391b0,PTR_DAT_0045a4ac);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004365e4,PTR_DAT_0045a090);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004371b4,PTR_DAT_0045a5c0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00406950,PTR_DAT_0045a498);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00436e98,PTR_DAT_0045a4e0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00418940,PTR_DAT_0045a230);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00438ec4,PTR_DAT_0045a058);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00447cec,PTR_DAT_0045a150);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00437734,PTR_DAT_0045a0e4);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004475a0,PTR_DAT_0045a59c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0041b5b8,PTR_DAT_0045a174);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00433494,PTR_DAT_0045a358);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00450958,PTR_DAT_0045a098);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040fbf8,PTR_DAT_0045a30c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00439834,PTR_DAT_0045a428);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00423988,PTR_DAT_0045a4b8);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004449d4,PTR_DAT_0045a164);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044bda8,PTR_DAT_0045a2f8);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00431368,PTR_DAT_0045a388);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0042c190,PTR_DAT_0045a394);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044b6ec,PTR_DAT_0045a050);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0042b9b0,PTR_DAT_0045a440);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0042dda8,PTR_DAT_0045a3e8);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00414be0,PTR_DAT_0045a3f0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00445340,PTR_DAT_0045a34c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004458c8,PTR_DAT_0045a0f0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040efac,PTR_DAT_0045a144);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00431acc,PTR_DAT_0045a254);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0043285c,PTR_DAT_0045a2dc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004382e8,PTR_DAT_0045a0a0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004279a8,PTR_DAT_0045a20c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00413334,PTR_DAT_0045a550);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00439f68,PTR_DAT_0045a52c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0043e654,PTR_DAT_0045a4c4);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004484f0,PTR_DAT_0045a0b0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040c058,PTR_DAT_0045a0e0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004192f4,PTR_DAT_0045a434);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00451f68,PTR_DAT_0045a370);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044d054,PTR_DAT_0045a214);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040ebe8,PTR_DAT_0045a468);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00429ecc,PTR_DAT_0045a4dc);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044d630,PTR_DAT_0045a0c8);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_004043f0,PTR_DAT_0045a330);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00418020,PTR_DAT_0045a100);
      Forms_TApplication_CreateForm_23EDC2EF(*(undefined4 *)pcVar1,&LAB_0044c8d0,PTR_DAT_0045a574);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0042200c,PTR_DAT_0045a19c);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044dea4,PTR_DAT_0045a458);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_00406f44,PTR_DAT_0045a3d4);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0040e4e8,PTR_DAT_0045a1a0);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0044e1c0,PTR_DAT_0045a5ac);
      Forms_TApplication_CreateForm_23EDC2EF
                (*(undefined4 *)pcVar1,&PTR_LAB_0041b1b0,PTR_DAT_0045a5f0);
      Forms_TApplication_CreateForm_23EDC2EF(*(undefined4 *)pcVar1,&LAB_004526c0,PTR_DAT_0045a168);
      Forms_TApplication_Run_23EDC2EF(*(undefined4 *)pcVar1);
    }
  }
  if (hObject != (HANDLE)0x0) {
    CloseHandle(hObject);
  }
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &DAT_00459fe0;
  puStack_28 = (undefined1 *)0x459fd8;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar2);
  return;
}

