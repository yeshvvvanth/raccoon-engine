--main.lua
getKeys=function()
  local s=''
  for k, v in pairs(_G) do
    if type(v)=='table' and v['type']==108 and k~='Script' then
       s = s..k..','
    end
  end
  return s
end

keys={}
keys.unknown            =31
keys.space              =32
keys.apostrophe         =39
keys.comma              =44
keys.minus              =45
keys.fullstop           =46
keys.slash              =47
keys.num_0              =48
keys.num_1              =49
keys.num_2              =50
keys.num_3              =51
keys.num_4              =52
keys.num_5              =53
keys.num_6              =54
keys.num_7              =55
keys.num_8              =56
keys.num_9              =57
keys.semicolon          =59
keys.equal              =61
keys.a                  =65
keys.b                  =66
keys.c                  =67
keys.d                  =68
keys.e                  =69
keys.f                  =70
keys.g                  =71
keys.h                  =72
keys.i                  =73
keys.j                  =74
keys.k                  =75
keys.l                  =76
keys.m                  =77
keys.n                  =78
keys.o                  =79
keys.p                  =80
keys.q                  =81
keys.r                  =82
keys.s                  =83
keys.t                  =84
keys.u                  =85
keys.v                  =86
keys.w                  =87
keys.x                  =88
keys.y                  =89
keys.z                  =90
keys.left_bracket       =91
keys.backslash          =92
keys.right_bracket      =93
keys.grave_accent       =96
keys.world_1            =161
keys.world_2            =162
keys.escape             =256
keys.enter              =257
keys.tab                =258
keys.backspace          =259
keys.insert             =260
keys.delete             =261
keys.right              =262
keys.left               =263
keys.down               =264
keys.up                 =265
keys.page_up            =266
keys.page_down          =267
keys.home               =268
keys.end_key            =269
keys.caps_lock          =280
keys.scroll_lock        =281
keys.num_lock           =282
keys.print_screen       =283
keys.pause              =284
keys.f1                 =290
keys.f2                 =291
keys.f3                 =292
keys.f4                 =293
keys.f5                 =294
keys.f6                 =295
keys.f7                 =296
keys.f8                 =297
keys.f9                 =298
keys.f10                =299
keys.f11                =300
keys.f12                =301
keys.f13                =302
keys.f14                =303
keys.f15                =304
keys.f16                =305
keys.f17                =306
keys.f18                =307
keys.f19                =308
keys.f20                =309
keys.f21                =310
keys.f22                =311
keys.f23                =312
keys.f24                =313
keys.f25                =314
keys.numpad_0           =320
keys.numpad_1           =321
keys.numpad_2           =322
keys.numpad_3           =323
keys.numpad_4           =324
keys.numpad_5           =325
keys.numpad_6           =326
keys.numpad_7           =327
keys.numpad_8           =328
keys.numpad_9           =329
keys.numpad_decimal     =330
keys.numpad_divide      =331
keys.numpad_multiply    =332
keys.numpad_subtract    =333
keys.numpad_add         =334
keys.numpad_enter       =335
keys.numpad_equal       =336
keys.lshift             =340
keys.lctrl              =341
keys.lalt               =342
keys.lsuper             =343
keys.rshift             =344
keys.rctrl              =345
keys.ralt               =346
keys.rsuper             =347
keys.menu               =348
keys.last               =keys.menu

PI= 3.14159265358979323846

Vec2={}
Vec3={}
Vec4={}
Quat={}
Mat4={}
Script={}
Entity={}

Sound={}
Light={}
RigidBody={}
collisionShape={}
Camera={}
Button={}

Script.type=108
Vec2.type=109
Vec3.type=110
Vec4.type=111
Quat.type=112
Mat4.type=113
Entity.type=114
RigidBody.type=115
collisionShape.type=116
--------proto-----------
Script.proto={type=108}
Vec2.proto={x=0,y=0,type=109}
Vec3.proto={x=0,y=0,z=0,type=110}
Vec4.proto={x=0,y=0,z=0,w=0,type=111}
Quat.proto={x=0,y=0,z=0,w=0,type=112}
Mat4.proto={elements={},type=113}
for i=1,16 do Mat4.proto.elements[i]=0 end
-------  -------------------------------
Script.mt={};Entity.mt={};Sound.mt={};Light.mt={};Camera.mt={};Button.mt={}
Vec2.mt={};Vec3.mt={};Vec4.mt={};Quat.mt={};Mat4.mt={};RigidBody.mt={}
Light.data={};Sound.data={};Camera.data={}

Vec3.dot=function(v1,v2)
  return (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z)
end
Vec3.cross=function(v1,v2)
  return Vec3.new(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x)
end
log=function(str,v)
  if type(v)=='table' and v.type==110 then
    print(str.." vec3("..v.x..","..v.y..","..v.z..")")
  elseif  type(v)=='table' and v.type==112 then
    print(str.." quat("..v.x..","..v.y..","..v.z..","..v.w..")")
  else
      print(str.." ="..v)
  end
end

Script.mt.__index=Script.proto
Vec2.mt.__index=Vec2.proto
Vec3.mt.__index=Vec3.proto
Vec4.mt.__index=Vec4.proto
Quat.mt.__index=Quat.proto
Mat4.mt.__index=Mat4.proto
----------- Vec3 ----------------------
Vec3.new=function(...)
  local args={...}
  local tv={x=0,y=0,z=0}
  if #args>0 and type(args[1])=='table' then
    local ov=args[1]
    tv.x=ov.x; tv.y=ov.y ; tv.z=ov.z
  elseif #args>=3 then
      --print('args='..args[1]..args[2]..args[3])
      if type(args[1])=='number' then tv.x=args[1] end
      if type(args[2])=='number' then tv.y=args[2] end
      if type(args[3])=='number' then tv.z=args[3] end
  elseif #args==1 then
      tv.x=args[1];tv.y=args[1];tv.z=args[1]
  end
  --print("Vec3 = "..tv.x..tv.y..tv.z)
  setmetatable(tv,Vec3.mt)
  return tv
end
Vec3.sqMagnitude=function(v) return v.x*v.x+v.y*v.y+v.z*v.z end
Vec3.Magnitude=function(v) return math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z) end
Vec3.Normalize=function(v)
  if(v.type==110) then
    local mag=Vec3.Magnitude(v);
    if mag~=0 then return v*(1/mag) else return v end
  else
    local sqm=(v.x*v.x+v.y*v.y+v.z*v.z+v.w*v.w)
    return v*(1/sqm)
  end
end
---------- operators ------------------
Vec3.mt.__add=function(l,r)
  return Vec3.new(l.x+r.x,l.y+r.y,l.z+r.z)
end
Vec3.mt.__mul=function(l,r)
  local v
  if (type(l)=='table' and l.type==110 )and (type(r)=='table' and r.type==110 ) then
    v=Vec3.new(l.x*r.x,l.y*r.y,l.z*r.z)
  elseif (type(l)=='table' and l.type==110 ) and type(r)=='number' then
    v=Vec3.new(l.x*r,l.y*r,l.z*r)
  elseif (type(r)=='table' and r.type==110 ) and type(l)=='number' then
    v=Vec3.new(l*r.x,l*r.y,l*r.z)
  end
  return v
end
Vec3.mt.__sub=function(l,r)
  return Vec3.new(l.x-r.x,l.y-r.y,l.z-r.z)
end

----------- Vec4 ------------------------
Vec4.new=function(...)
  local args={...}
  local tv={x=0,y=0,z=0,w=0}
  if #args>0 and type(args[1])=='table' then
    local ov=args[1]
    tv.x=ov.x; tv.y=ov.y ; tv.z=ov.z ; tv.w=ov.w
  elseif #args==4 then
      if type(args[1])=='number' then tv.x=args[1] end
      if type(args[2])=='number' then tv.y=args[2] end
      if type(args[3])=='number' then tv.z=args[3] end
      if type(args[4])=='number' then tv.w=args[4] end
  elseif #args==1 then
      tv.x=args[1];tv.y=args[1];tv.z=args[1];tv.w=args[1]
  end
  setmetatable(tv,Vec4.mt)
  return tv
end
---------- operators ------------------
Vec4.mt.__add=function(l,r)
  return Vec4.new(l.x+r.x,l.y+r.y,l.z+r.z,l.w+r.w)
end
Vec4.mt.__mul=function(l,r)
  local v
  if (type(l)=='table' and l.type==111 )and (type(r)=='table' and r.type==111 ) then
    v=Vec4.new(l.x*r.x,l.y*r.y,l.z*r.z,l.w*r.w)
  elseif (type(l)=='table' and l.type==111 ) and type(r)=='number' then
    v=Vec4.new(l.x*r,l.y*r,l.z*r,l.w*r)
  elseif (type(r)=='table' and r.type==111 ) and type(l)=='number' then
    v=Vec4.new(l*r.x,l*r.y,l*r.z,l*r.w)
  end
  return v
end
Vec4.mt.__sub=function(l,r)
  return Vec4.new(l.x-r.x,l.y-r.y,l.z-r.z,l.w-r.w)
end

----------- Quat ------------------------
Quat.new=function(...)
  local args={...}
  local tq={x=0,y=0,z=0,w=0}
  if #args==1 and type(args[1])=='table' then
    local oq=args[1]
    tq.x=oq.x; tq.y=oq.y ; tq.z=oq.z ; tq.w=oq.w
  elseif #args==4 then

      if type(args[1])=='number' then tq.x=args[1] end
      if type(args[2])=='number' then tq.y=args[2] end
      if type(args[3])=='number' then tq.z=args[3] end
      if type(args[4])=='number' then tq.w=args[4] end
  elseif #args==2 and type(args[1])=='table' then
      return Quat.fromEuler(args[1],args[2])
  end
  setmetatable(tq,Quat.mt)
  return tq
end
---------- operators ------------------
Quat.mt.__add=function(l,r)
  return Quat.new(l.x+r.x,l.y+r.y,l.z+r.z,l.w+r.w)
end
Quat.mt.__mul=function(l,r)
  local v
  if (type(l)=='table' and l.type==112 )and (type(r)=='table' and r.type==112 ) then
    --v=Quat.new(l.x*r.x,l.y*r.y,l.z*r.z,l.w*r.w);
    local x=l.x;local y=l.y;local z=l.z;local w=l.w;local quaternion=r;
    v=Quat.new(
              (((w * quaternion.x) + (x * quaternion.w)) + (y * quaternion.z)) - (z * quaternion.y),
              (((w * quaternion.y) + (y * quaternion.w)) + (z * quaternion.x)) - (x * quaternion.z),
              (((w * quaternion.z) + (z * quaternion.w)) + (x * quaternion.y)) - (y * quaternion.x),
              (((w * quaternion.w) - (x * quaternion.x)) - (y * quaternion.y)) - (z * quaternion.z)
              )
    v=v*(1/math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z+v.w*v.w))
  elseif (type(l)=='table' and l.type==112 ) and type(r)=='number' then
    v=Quat.new(l.x*r,l.y*r,l.z*r,l.w*r)
  elseif (type(r)=='table' and r.type==112 ) and type(l)=='number' then
    v=Quat.new(l*r.x,l*r.y,l*r.z,l*r.w)
  end
  return v
end
Quat.mt.__sub=function(l,r)
  return Quat.new(l.x-r.x,l.y-r.y,l.z-r.z,l.w-r.w)
end
----------- Mat4 ----------------------
Mat4.new=function(...)
  local args={...}
  local tm={}
  for i=1,16 do tm[i]=0 end
  tm[1]=1;tm[6]=1;tm[11]=1;tm[16]=1;

  if     #args==1 and type(args[1])=='table'  then
    for i=1,16 do tm[i]=args[1][i] end
  elseif #args==1 and type(args[1])=='number' then
    tm[1]= args[1] ;tm[6]= args[1]
    tm[11]=args[1] ;tm[16]=args[1]
  end
  setmetatable(tm,Mat4.mt)
  return tm
end
Mat4.mt.__mul=function (l,r)
  local tm=multiplyMat4(l,r)
  return tm
end
----------- Aux Meta Tables ------------

---------- Script ----------------
Script.new=function()
  local scr={}
  setmetatable(scr,Script.mt)
  scr.type=108
  return scr
end
-------- RigidBody -----------
RigidBody.new=function(udata,type,mass,frict,pos,rot,ld,ad,linVel,angVel,grav,astate)
  local rb={}
  rb.udata=udata;rb.type=type;rb.mass=mass;rb.pos=pos;
  rb.linearDamping=ld;rb.angularDamping=ad;rb.friction=frict
  rb.rot=rot;rb.linearVelocity=linVel;rb.angularVelocity=angVel;
  rb.gravity=grav;rb.actstate=astate;

  rb.applyImpulse=function(vec,ce)
    if ce==nil then
      applyRBody(0,vec)
    else
      applyRBody(1,vec,ce)
    end
  end
  rb.applyForce=  function(vec,ce)
     if ce==nil then
       applyRBody(2,vec)
     else
       applyRBody(3,vec,ce)
     end
  end
  rb.applyTorqueImpulse=function(vec) applyRBody(4,vec) end
  rb.applyTorque=function(vec) applyRBody(5,vec) end
  rb.applyDamping=function(vec) applyRBody(6,vec) end

  return rb
end
-----------------------------
RigidBody.mt.__index=function(t,k)
  return t.data[k]
end
RigidBody.mt.__newindex=function(t,k,v)
  t.data[k]=v
  setRigidBody(t)
end
------------ Entity
Entity.new=function(...)
  local args={...}
  local tent={type=114}
  tent.instantiate=function(arg) return do_entity(tent.udata,1,arg) end
  tent.rename=function(tname)  do_entity(tent.udata,2,tname);tent.name=tname end
  tent.setAnimation=function(arg) setAnimation(0,arg) end
  tent.setAnimSpeed=function(arg) setAnimation(1,arg) end
  tent.setAnimTransSpeed=function(arg) setAnimation(2,arg) end
  if #args==0 then
    tent.udata=0;tent.name="ent"
  elseif #args==2 then
    tent.udata=args[1];tent.name=args[2]
  end
  setmetatable(tent,Entity.mt)
  return tent
end

entity=Entity.new()
C_ascript=false
Time={deltaTime=0,time=0,dt=0,timescale=1}
setAmbientLight=function  (arg)
  setLight(true,arg)
end
-----------------------------
Entity.mt.__index=function(t,k)
  local exists=setEntity(t.udata)
  if not exists then return nil end
    if k=='pos' then
      return getTrans(0)
    elseif k=='rot' then
      return getTrans(1)
    elseif k=='scale' then
      return getTrans(2)
    elseif k=='parent' then
      return parchild(true)
    elseif k=='children' then
      return parchild(false)
    elseif k=='rigidBody' or k=='rb' or k=='body' then
      local trb={};trb.data=getRigidBody();setmetatable(trb,RigidBody.mt)
      return trb
    elseif k=='button' then
      local tb={};tb.data=getButton();setmetatable(tb,Button.mt)
      return tb
    elseif k=='light' then
      local tl={};tl.data=getLight();setmetatable(tl,Light.mt)
      return tl
    elseif k=="Remove" then
      return Remove
    elseif k=='sound' then
      local ts={}
      ts.data=getSound()
      ts.play =function() setSound(true,false)  end
      ts.pause=function() setSound(false,false) end
      ts.stop =function() setSound(false,true)  end
      setmetatable(ts,Sound.mt)
      return ts
    elseif k=='modelMat' then
      return  getTrans(13)
    else
      local tprop=_G[C_ascript][k]
      if tprop~=nil then return tprop else print(k.."_ is not a member of entity") end
    end
  setEntity(entity.udata)
end

Entity.mt.__newindex=function(t,k,v)
  local exists=setEntity(t.udata)
  if not exists then return nil end
    if k=='pos' then
       setTrans(0,v)
    elseif k=='rot' then
      setTrans(1,v)
    elseif k=='scale' then
      setTrans(2,v)
    elseif k=='rigidBody' then
      setRigidBody(v)
    elseif k=='light' then
      setLight(v)
    elseif k=='enabled' then
      do_entity(t.udata,3,v)
    elseif k=='sound' then
      setSound(v)
    else
      local tprop=_G[C_ascript][k]
      if tprop~=nil then _G[C_ascript][k]=v else print(k.."   is not a member of entity") end
    end
  setEntity(entity.udata)
end
-------------------------------------------------------

Sound.mt.__newindex=function(t,k,v)
  t.data[k]=v
  setSound(t.data)
end
Sound.mt.__index=function(t,k)
  return t.data[k]
end
Light.mt.__newindex=function(t,k,v)
  t.data[k]=v;
  setLight(t.data)
end
Light.mt.__index=function(t,k)
  return t.data[k]
end
Button.mt.__newindex=function(t,k,v)
  t.data[k]=v
  if k=='text' then setButton(5,v)
  elseif k=='width' then setButton(1,v)
  elseif k=='height' then setButton(2,v)
  elseif k=='x' then setButton(3,v)
  elseif k=='y' then setButton(4,v)
  elseif k=='colour' then setButton(6,v)
  elseif k=='alpha' then setButton(7,v)
  end

end
Button.mt.__index=function(t,k)
  return t.data[k]
end

gmt={}
setmetatable(_G,gmt)
gmt.__index=function (t,k)
  if k=='ent' then
    return _G['entity']
  elseif k=='entities' then
    return getEntities()
  elseif k=='scale' then
      print('set scale')
  end
end

loadScene=function(arg)
  local boo=false
  --args: type , index , reload , name
  if(type(arg)=='number') then boo=setActiveScene(3,arg,false,"")
  elseif(type(arg)=='string' and arg=='next') then boo=setActiveScene(1,0,false,"")
  elseif(type(arg)=='boolean' and arg=='previous') then boo=setActiveScene(2,0,false,"")
  elseif(type(arg)=='string') then boo=setActiveScene(4,0,false,arg)
  end
end
reloadScene=function(arg)
  if arg~=nil then local boo=false;
    if(type(arg)=='number') then boo=setActiveScene(3,arg,true,"")
    elseif(type(arg)=='string' and arg=='next') then boo=setActiveScene(1,0,true,"")
    elseif(type(arg)=='boolean' and arg=='previous') then boo=setActiveScene(2,0,true,"")
    elseif(type(arg)=='string') then boo=setActiveScene(4,0,true,arg)
    end
  else
    setActiveScene(0,0,true,"")
  end
end

getKey=function(arg) return get_Key(0,arg) end
getKeyDown=function(arg) return get_Key(1,arg) end
getMouseButton=function(arg)  return getMouse(5,arg) end
getMouseButtonDown=function(arg)  return getMouse(6,arg) end
getMouseScroll=function()  return getMouse(9) end

getCamRay=function()  return getMouse(1) end
getScreenRay=function(ax,ay) return getMouse(4,vec3(ax,ay,0)) end

getMouseDelta=function()  return getMouse(0) end
getCamPos=function()  return getMouse(7) end
getScrMousePos=function()  return getMouse(2) end
getScreenResolution=function()  return getMouse(3) end
getMousePos=function()  return getMouse(4) end

script=Script.new
vec3=Vec3.new;quat=Quat.new;Mat4=Mat4.new;vec4=Vec4.new
cross=Vec3.cross;dot=Vec3.dot;norm=Vec3.Normalize;
mag=Vec3.Magnitude;smag=Vec3.sqMagnitude;
euler= Quat.fromEuler
rotate=Quat.rotate

lerp=function(v1,v2,fac)
  return v1*(1-fac)+v2*fac
end

--   (1= raycast) (2= draw line) (4= closest) (8= boolean)
RayCastBool=function(v1,v2,draw,colour)
  local f=1|4|8;if draw==true then f=f|2 end
  return C_castRay(v1,v2,f,colour)
end
RayCast=function(v1,v2,draw,colour)
  local f=1|4;if draw==true then f=f|2 end
    return C_castRay(v1,v2,f,colour)
end
RayCastAll=function(v1,v2,draw,colour)
  local f=1;if draw==true then f=f|2 end
    return C_castRay(v1,v2,f,colour)
end
DrawLine=function(v1,v2)C_castRay(v1,v2,2)end

RenderLine=function(p1,p2,t1,t2,c1,c2)
  C_drawPrim(-1,p1,p2,t1,t2,vec3(c1.x,c1.y,c1.z),vec3(c2.x,c2.y,c2.z),c1.w,c2.w)
end
renderCube=function(pos,size,c1)
  C_drawPrim(0,pos,size,vec3(c1.x,c1.y,c1.z),c1.w)
end
renderSphere=function(pos,size,c1)
  C_drawPrim(1,pos,size,vec3(c1.x,c1.y,c1.z),c1.w)
end
renderCylinder=function(pos,size,c1)
  C_drawPrim(2,pos,size,vec3(c1.x,c1.y,c1.z),c1.w)
end

Destroy=function(tent)
  do_entity(tent.udata,0)
end

Remove=function(comp)
  if type(comp)=='string' then
      if comp=="physics" then
        C_removeComp(1)
      elseif comp=='mesh' then
        C_removeComp(0)
      elseif comp=='light' then
        C_removeComp(2)
      elseif comp=='sound' then
        C_removeComp(3)
      end
  end
end
print("Everything is fine............................... \n\n\n")
lookAt=function(sp,ep)
  local cd=norm(ep-sp)
  if cd.x==0 and cd.z==0 then  cd1=vec3(0,-1,0)else cd1=norm(cd*vec3(1,0,1))end
  return quat(cd1,cd)*quat(vec3(0,0,-1),cd1)
end
