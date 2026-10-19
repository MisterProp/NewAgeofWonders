
void FUN_00455dfc(undefined4 param_1,char param_2)

{
  if (param_2 == '\0') {
    Sound_PlaySample("Sfx\\Int\\Btn1.wav");
    return;
  }
  if (param_2 == '\x01') {
    Sound_PlaySample("Sfx\\Int\\Btn5.wav");
    return;
  }
  if (param_2 == '\x02') {
    Sound_PlaySample("Sfx\\Int\\Btn6.wav");
  }
  else if (param_2 == '\x03') {
    Sound_PlaySample("Sfx\\Int\\Btn3.wav");
    return;
  }
  return;
}

