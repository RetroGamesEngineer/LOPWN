# Pwn Fly

Pwn Adventure 3 Fly Solution! :)

C++ version and Cheat Engine ASM version in repo and below...

After going into game in offline mode: Enable the script and press middle mouse button to use fly! Press F1 to temporarily deactivate/reactivate ability to fly.

Pwn Fly v2.1 (Copyable directly to Cheat Engine's cheat list view)
```
<?xml version="1.0" encoding="utf-8"?>
<CheatTable>
  <CheatEntries>
    <CheatEntry>
      <ID>20</ID>
      <Description>"Pwn Fly v2.1"</Description>
      <LastState/>
      <VariableType>Auto Assembler Script</VariableType>
      <AssemblerScript>{Pwn Adventure 3 Fly Cheat v2.1
RetroGamesEngineer on github (C) 2019}
[enable]
alloc(FlyThread,$1000)
registersymbol(FlyThread)
createthread(FlyThreadLoop)
define(VK_F1,70) //Toggle on/off (F1)
define(VK_MBUTTON,4) //Trigger Flying (Middle Mouse Button)

struct world_transformation_vectors
inverse_sin_yaw: dd ?
cos_yaw: dd ?
sin_pitch: dd ?
zero1: dd ?
cos_pitch1: dd ?
cos_pitch2: dd ?
pitch_steepness_factor: dd ?
zero2: dd ?
fly_speed1: dd ?
fly_speed2: dd ?
fly_speed3: dd ?
zero3: dd ?
endstruct

FlyThread:
WorldTransformationVectors:
dd 0 0 0 0 0 0 (float)1.0 0 0 0 0 0
PitchYawPointer:
dd 0
PlayerPositionPointer:
dd 0
PI_divided_by_180:
dd (float)0.0174532
FlySpeed:
dd (float)33.33
FlyState:
dd 1
Ninety:
dd (float)90.0
dq 9090909090909090

FlyThreadLoop: //+0x50
push 1
call Sleep
cmp [FlyState],2
je FlyThreadExit
push VK_F1
call NtUserGetKeyState
and eax,80
je @f
xor [FlyState],1 //Toggle fly active/inactive
push #200
call Sleep
@@:
cmp [FlyState],1 //Fly only if activated
jne FlyThreadLoop
push VK_MBUTTON  //And Middle Mouse Button is pressed
call NtUserGetKeyState
and eax,80
je FlyThreadLoop
mov eax,[GameLogic.Game] //Same as --&gt; mov eax,["GameLogic.dll"+97d80] &lt;--
test eax,eax
je FlyThreadLoop
mov eax,[eax+58]
test eax,eax
je FlyThreadLoop
mov eax,[eax+1cc]
test eax,eax
je FlyThreadLoop
mov eax,[eax+294]
test eax,eax
je FlyThreadLoop
mov [PitchYawPointer],eax
mov ecx,[GameLogic.GameWorld] //Same as --&gt; mov ecx,["GameLogic.dll"+97d7c] &lt;--
test ecx,ecx
je FlyThreadLoop
mov ecx,[ecx+1c]
test ecx,ecx
je FlyThreadLoop
mov ecx,[ecx+4]
test ecx,ecx
je FlyThreadLoop
mov ecx,[ecx+114]
test ecx,ecx
je FlyThreadLoop
mov [PlayerPositionPointer],ecx
//WorldTransformationVectors layout: -sin(yaw-90.0f), cos(yaw-90.0f), sin(pitch), 0.0f
//WorldTransformationVectors+10 layout: cos(pitch), cos(pitch), 1.0f, 0.0f
//WorldTransformationVectors+20 layout: FlySpeed, FlySpeed, FlySpeed, 0.0f
mov edx,WorldTransformationVectors
fld dword [eax+2a0] //load pitch
fmul dword [PI_divided_by_180] //Degrees to radians
fsincos
fst dword [edx+cos_pitch1]
fstp dword [edx+cos_pitch2]
fstp dword [edx+sin_pitch]
fld dword [eax+2a4] //load yaw
fsub dword [Ninety] //yaw - 90.0f
fmul dword [PI_divided_by_180] //Degrees to radians
fsincos
fstp dword [edx+cos_yaw]
fstp dword [edx+inverse_sin_yaw]
xor byte [edx+inverse_sin_yaw+3],80
mov eax,[FlySpeed]
mov [edx+fly_speed1],eax
mov [edx+fly_speed2],eax
mov [edx+fly_speed3],eax
//Vectorized calculate new x,y,z coordinates simultaneously
//Multiply the three set up vectors and then add current player coordinates
//Then directly write new position back to player coordinates
movaps xmm1,[edx]    //xmm1 == -sin(yaw-90.0f), cos(yaw-90.0f), sin(pitch), 0.0f
mulps xmm1,[edx+10]  //xmm1 *= cos(pitch), cos(pitch), 1.0f, 0.0f
mulps xmm1,[edx+20]  //xmm1 *= FlySpeed, FlySpeed, FlySpeed, 0.0f
addps xmm1,[ecx+90]  //xmm1 += Player.x, Player.y, Player.z, 0.0f
movaps [ecx+90],xmm1 //PlayerXYZ == xmm1
jmp FlyThreadLoop
FlyThreadExit:
mov [FlyState],ffffffff
ret
[disable]
assert(FlyThread+40,ffffffff)
dealloc(FlyThread)
unregistersymbol(FlyThread)
</AssemblerScript>
      <CheatEntries>
        <CheatEntry>
          <ID>21</ID>
          <Description>"FlyState"</Description>
          <ShowAsHex>1</ShowAsHex>
          <VariableType>4 Bytes</VariableType>
          <Address>FlyThread+40</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>23</ID>
          <Description>"FlySpeed"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+3c</Address>
        </CheatEntry>
        <CheatEntry>
          <ID>30</ID>
          <Description>"Camera.Pitch"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+30</Address>
          <Offsets>
            <Offset>2a0</Offset>
          </Offsets>
        </CheatEntry>
        <CheatEntry>
          <ID>31</ID>
          <Description>"Camera.Yaw"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+30</Address>
          <Offsets>
            <Offset>2a4</Offset>
          </Offsets>
        </CheatEntry>
        <CheatEntry>
          <ID>32</ID>
          <Description>"Player.X"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+34</Address>
          <Offsets>
            <Offset>90</Offset>
          </Offsets>
        </CheatEntry>
        <CheatEntry>
          <ID>34</ID>
          <Description>"Player.Y"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+34</Address>
          <Offsets>
            <Offset>94</Offset>
          </Offsets>
        </CheatEntry>
        <CheatEntry>
          <ID>35</ID>
          <Description>"Player.Z"</Description>
          <VariableType>Float</VariableType>
          <Address>FlyThread+34</Address>
          <Offsets>
            <Offset>98</Offset>
          </Offsets>
        </CheatEntry>
      </CheatEntries>
    </CheatEntry>
  </CheatEntries>
</CheatTable>
```
