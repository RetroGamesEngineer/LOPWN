# Pwn Fly

Pwn Adventure 3 Fly Solution! :)

C++ version and Cheat Engine ASM version in repo and below...

After going into game in offline mode: Enable the script, press F1 to toggle fly active, then middle mouse button to use it!

Improved Pwn Fly v2 ASM Copy Pastable Directly To Cheat Engine:
```
<?xml version="1.0" encoding="utf-8"?>
<CheatTable>
  <CheatEntries>
    <CheatEntry>
      <ID>20</ID>
      <Description>"Pwn Fly v2"</Description>
      <LastState Activated="1"/>
      <VariableType>Auto Assembler Script</VariableType>
      <AssemblerScript>[enable]
//RetroGamesEngineer on github (C) 2019
alloc(FlyThread,$1000)
registersymbol(FlyThread)
createthread(FlyThreadStart)

define(VK_F1,70) //Toggle on/off
define(VK_MBUTTON,4) //Trigger Flying

struct pitch_yaw_custom_vectors
negative_sin_yaw: dd ?
cos_yaw: dd ?
sin_pitch: dd ?
zero1: dd ?
cos_pitch: dd ?
cos_pitch2: dd ?
one: dd ?
zero2: dd ?
fly_speed1: dd ?
fly_speed2: dd ?
fly_speed3: dd ?
zero3: dd ?
endstruct

struct vec3
x: dd ?
y: dd ?
z: dd ?
endstruct

FlyThread:
label(PitchYawVectors)
PitchYawVectors:
dd 0 0 0 0 0 0 (float)1.0 0 0 0 0 0
label(PitchYawPointer)
PitchYawPointer:
dd 0
label(PlayerPositionPointer)
PlayerPositionPointer:
dd 0
label(FlyEnabled)
FlyEnabled:
dd 1
label(FlyActive)
FlyActive:
dd 0
label(FlySpeed)
FlySpeed:
dd (float)33.33
label(PI_divided_by_180)
PI_divided_by_180:
dd (float)0.0174532
label(Ninety)
Ninety:
dd (float)90.0

label(FlyThreadStart)
FlyThreadStart:
push 1
call Sleep
//Get PitchYawPointer on initially starting thread before proceeding to main loop
mov eax,[GameLogic.Game] //Same as --&gt; mov eax,["GameLogic.dll"+97d80] &lt;--
test eax,eax
je @b
mov eax,[eax+58]
test eax,eax
je @b
mov eax,[eax+1cc]
test eax,eax
je @b
mov eax,[eax+294]
test eax,eax
je @b
lea eax,[eax+2a0]
mov [PitchYawPointer],eax

label(FlyThreadLoop)
FlyThreadLoop:
push 1
call Sleep
cmp [FlyEnabled],0 //Exit thread upon disabling
je FlyThreadExit
push VK_F1
call GetAsyncKeyState
and ax,8000
je @f
xor [FlyActive],1 //Toggle Fly on/off
push #337
call Sleep
@@:
cmp [FlyActive],1 //Only Fly if activated
jne FlyThreadLoop
push VK_MBUTTON  //And Middle Mouse Button is pressed
call GetAsyncKeyState
and ax,8000
je FlyThreadLoop
mov eax,[GameLogic.GameWorld] //Same as --&gt; mov eax,["GameLogic.dll"+97d7c] &lt;--
test eax,eax
je FlyThreadLoop
mov eax,[eax+1c]
test eax,eax
je FlyThreadLoop
mov eax,[eax+4]
test eax,eax
je FlyThreadLoop
mov eax,[eax+114]
test eax,eax
je FlyThreadLoop
lea eax,[eax+90]
mov [PlayerPositionPointer],eax

//Setup custom vectors
//PitchYawVectors layout: -sin(yaw-90.0f), cos(yaw-90.0f), sin(pitch), 0.0f
//PitchYawVectors+10 layout: cos(pitch), cos(pitch), 1.0f, 0.0f
//PitchYawVectors+20 layout: FlySpeed, FlySpeed, FlySpeed, 0.0f
mov eax,[PitchYawPointer]
mov ecx,[PlayerPositionPointer]
mov edx,PitchYawVectors
//Get pitch sin &amp; cos
fld dword [eax] //load pitch
fmul dword [PI_divided_by_180] //Degrees to radians
fsincos
fst dword [edx+cos_pitch]
fstp dword [edx+cos_pitch2]
fstp dword [edx+sin_pitch]
//Get yaw sin &amp; cos
fld dword [eax+4] //load yaw
fsub dword [Ninety] //yaw - 90.0f
fmul dword [PI_divided_by_180] //Degrees to radians
fsincos
fstp dword [edx+cos_yaw]
fstp dword [edx+negative_sin_yaw]
fldz //load zero
fsub dword [edx+negative_sin_yaw] //0.0f - sin(yaw-90.0f) == -(sin(yaw-90.0f))
fstp dword [edx+negative_sin_yaw]
mov eax,[FlySpeed]
mov [edx+fly_speed1],eax
mov [edx+fly_speed2],eax
mov [edx+fly_speed3],eax

//Vectorized calculate new x,y,z coordinates simultaneously
//Multiply the three set up vectors and then add current player coordinates
//Then directly write new position back to player coordinates
movaps xmm1,[edx]  //xmm1 == -sin(yaw-90.0f), cos(yaw-90.0f), sin(pitch), 0.0f
mulps xmm1,[edx+10]//xmm1 *= cos(pitch), cos(pitch), 1.0f, 0.0f
mulps xmm1,[edx+20]//xmm1 *= FlySpeed, FlySpeed, FlySpeed, 0.0f
addps xmm1,[ecx]   //xmm1 += Player.x, Player.y, Player.z, 0.0f
movaps [ecx],xmm1  //PlayerXYZ == xmm1
jmp FlyThreadLoop

label(FlyThreadExit)
FlyThreadExit:
ret

[disable]

FlyThread+38: //Causes FlyThread to exit
dd 0

unregistersymbol(FlyThread)
</AssemblerScript>
      <CheatEntries>
        <CheatEntry>
          <ID>21</ID>
          <Description>"FlyEnabled"</Description>
          <VariableType>4 Bytes</VariableType>
          <Address>FlyThread+38</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>22</ID>
          <Description>"FlyActive"</Description>
          <VariableType>4 Bytes</VariableType>
          <Address>FlyThread+3c</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>23</ID>
          <Description>"FlySpeed"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+40</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>24</ID>
          <Description>"PitchYaw Pointer"</Description>
          <ShowAsHex>1</ShowAsHex>
          <VariableType>4 Bytes</VariableType>
          <Address>FlyThread+30</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>25</ID>
          <Description>"PlayerPosition Pointer"</Description>
          <ShowAsHex>1</ShowAsHex>
          <VariableType>4 Bytes</VariableType>
          <Address>FlyThread+34</Address>
        </CheatEntry>
      </CheatEntries>
    </CheatEntry>
  </CheatEntries>
</CheatTable>
```
