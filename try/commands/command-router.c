#include <stdio.h>
#include "../../src/clips/clips.h"

// Router system
int queryRouter(char *name)
{
//  printf("Router considering '%s'\n", name);

  if(strcmp(name, "stdout") == 0) return TRUE;
  if(strcmp(name, "wprompt") == 0) return TRUE;
  if(strcmp(name, "wdialog") == 0) return TRUE;
  if(strcmp(name, "wdisplay") == 0) return TRUE;
  if(strcmp(name, "werror") == 0) return TRUE;
  if(strcmp(name, "wwarning") == 0) return TRUE;
  if(strcmp(name, "wtrace") == 0) return TRUE;
  if(strcmp(name, "wclips") == 0) return TRUE;
//  if(strcmp(name, "command") == 0) return TRUE;

  return FALSE;
}
int printRouter(char *name, char*str)
{
//  if(strcmp(name, "wprompt") == 0) return TRUE;

  printf("%s", str);
//  printf("%s: '%s'\n", name, str);
  return TRUE;
}
int getcRouter(char *name)
{
  return 'a';
}
int ungetcRouter(int ch, char *name)
{
  return ch;
}
int exitRouter(int code)
{
  return code;
}

int ExecuteIfCommandComplete_jarcec(void *theEnv)
{
   if ((CompleteCommand(CommandLineData(theEnv)->CommandString) == 0) || 
       (RouterData(theEnv)->CommandBufferInputCount <= 0))
    { return FALSE; }
      
  FlushPPBuffer(theEnv);
  SetPPBufferStatus(theEnv,OFF);
  RouterData(theEnv)->CommandBufferInputCount = -1;
  RouteCommand(theEnv,CommandLineData(theEnv)->CommandString,TRUE);
  FlushPPBuffer(theEnv);
  SetHaltExecution(theEnv,FALSE);
  SetEvaluationError(theEnv,FALSE);
  FlushCommandString(theEnv);
  FlushBindList(theEnv);
  PeriodicCleanup(theEnv,TRUE,FALSE);
  PrintPrompt(theEnv);
         
  return TRUE;
}

// Tries
int main(int argc, char **argv)
{
  // Initialization
//  InitializeEnvironment();
//  void *env = GetCurrentEnvironment();
  void *env = CreateEnvironment();
  int ret;

  // Adding my new routers
  AddRouter("stdout", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wprompt", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wdialog", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wdisplay", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("werror", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wwarning", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wtrace", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("wclips", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);
  AddRouter("command", 20, queryRouter, printRouter, getcRouter, ungetcRouter, exitRouter);

  FlushCommandString(env);

  // Some coding (tries)
  AppendCommandString(env, "(assert (ahoj ahoj ahoj))\n(assert (zvire jarcec))\n");
  ret = ExecuteIfCommandComplete_jarcec(env);
  printf("Debug: %s\n", GetCommandString(env));
  ret = ExecuteIfCommandComplete_jarcec(env);
  printf("exec: %d\n", ret);

  AppendCommandString(env, "(assert (ahoj ahoj ahoj nazdar))\n");
  ret = ExecuteIfCommandComplete_jarcec(env);
  printf("exec: %d\n", ret);

  AppendCommandString(env, "(facts)\n");
  ret = ExecuteIfCommandComplete_jarcec(env);
  printf("exec: %d\n", ret);
}
