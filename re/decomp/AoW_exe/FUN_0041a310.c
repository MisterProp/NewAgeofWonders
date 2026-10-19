// /AoW.exe : FUN_0041a310

void FUN_0041a310(int param_1)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int *piVar3;
  int *in_FS_OFFSET;
  int iStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_28 = &LAB_0041a9fe;
  iStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_2c;
  FUN_0041a128(param_1);
  AoWE_TAoWEngine_RegisteredTo_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x54),local_8);
  AoWE_TAoWEngine_GetVersionStr_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref,&local_14);
  local_10 = local_14;
  local_c = CONCAT31(local_c._1_3_,0xb);
  System_LoadResString_02614E1B(AoWE_VersionXRStr_04ECFCC5_exref,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,&local_18);
  AoWE_Format_D8F48273(local_18,&local_10,0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),local_8);
  uVar2 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);
  *(undefined4 *)(param_1 + 0x98) = uVar2;
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_DesignerAndDirectorRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Lennart Sas");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  System_LoadResString_02614E1B(AoWE_AdditionalDesignRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Arno van Wingerden");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_LeadProgrammerRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Arno van Wingerden");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  System_LoadResString_02614E1B(AoWE_ProgrammerRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Josh Farley");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_ArtistsRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Lennart Sas");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Thomas Cardin");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Roy Postma");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_LeadScenarioDesignerRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Menno Sas");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_ScenarioDesignersRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Doug Gibson");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Arnout Sas");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Hugo van Wingerden");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_WriterRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Raymond Bingham");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_MusicRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Michiel van den Bos");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  System_LoadResString_02614E1B(AoWE_SoundFXRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Nando Eweg");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  System_LoadResString_02614E1B(AoWE_IntroVoiceRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"John McGinn (English)");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Norbert Gastell (German)");
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_GalaxySoundSystemRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Carlo Vogelsang");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_InternetMailSuiteRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Argo Software Design");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_QACoreRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Cliff Benjamin");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Ian Calderon");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Ted Stephenson");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Hugo van Wingerden");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  AoWE_TranslateRStr_435799AA("Take 2 Europe",&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Producer");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Ken Jordan");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Manuals + Translations");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Frazer Nash");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Tarantula Studios testing suite");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Test Manager");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Mark Lloyd");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Lead Tester");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Paul Byers");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Testers");
  (**(code **)(*piVar3 + 0x34))(piVar3,0);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Jim Thompson");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Kit Brown");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Julian Turner");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Jim Cree");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Lee Johnson");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_BizzRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Jay Wilbur");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Mark Rein");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  piVar3 = (int *)System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  System_LoadResString_02614E1B(AoWE_SpecialThanksRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,local_8);
  (**(code **)(*piVar3 + 0x34))(piVar3,"Eelco van der Snoek");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Doug Gibson");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Dietmer Tan");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Arjan Brussee");
  (**(code **)(*piVar3 + 0x34))(piVar3,"Daniel Cook");
  Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x98),piVar3);
  puVar1 = puStack_28;
  *in_FS_OFFSET = (int)&local_8;
  puStack_28 = &LAB_0041aa05;
  iStack_2c = 0x41a9f5;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);
  iStack_2c = 0x41a9fd;
  System__LStrClr_40929B27(&local_8);
  return;
}

