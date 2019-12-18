
float=script()

float.spos=vec3()
float.Init=function()
	float.spos=ent.pos
end

float.Update=function()

	ent.pos=float.spos+vec3(0,1*math.sin(Time.time),0)
end

speechrate=8
player=script()
speeches={}
player.Init=function()
      stime=0
      cam=findEntity('camera');speech=findEntity('speech')
      ent.rigidBody.linearDamping=0.999
      by=speech.button.y
end
player.Update=function()
  dt=Time.deltaTime
    stime=stime+dt*speechrate
    local lv=ent.rigidBody.linearVelocity
    cam.pos=ent.pos+vec3(0,20,14)
    local vel=vec3();local speed=160*dt 
    if getKey(keys.w) then vel.z=vel.z-speed end
    if getKey(keys.s) then vel.z=vel.z+speed end
    if getKey(keys.a) then vel.x=vel.x-speed end
    if getKey(keys.d) then vel.x=vel.x+speed end
    if getKeyDown(keys.space) then vel.y=30+vel.y end
    if mag(lv) < 60 then ent.rigidBody.applyImpulse(vel) end
    speech.button.y=by+math.sin(Time.time*2)*60
    if #speeches[1]>1+math.floor(stime) then
      speech.button.text=string.sub( speeches[1],1,math.min(1+math.floor(stime),#speeches[1]) )
    end
    
    printf('position = '..math.floor(ent.pos.x).." , "..math.floor(ent.pos.y),0,300)
end




speeches[1]="I don't think I remember how to make this work "


--[[
tplayer=script()
tplayer.Update=function()
  print('pushing'..inp.y)
  cam= findEntity('cam')
  --local cdir=norm(cam.pos-ent.pos)
  local cdir=norm(vec3(0,3,0)+  rotate(euler(0,-getScrMousePos().x*0,0),vec3(0,0,2)) )
  pmesh= findEntity('pmesh')
  ent.rb.linearDamping=0.9999
  
  zoom=zoom-getMouseScroll()*dt*240
  zoom=math.min(50,zoom);zoom=math.max(30,zoom)
  cam.rot=lookAt(cam.pos,ent.pos)
  
  cam.pos=ent.pos+cdir*zoom
  forward=norm(-1*cdir*vec3(1,0,1));right=cross(-1*up,forward)
  imp=inp.x*right+inp.y*forward
  ent.rb.applyImpulse((imp)*dt*180)
  
  local to=norm(vec3(inp.x,0,-inp.y+0.001))--+ent.pos
  pmesh.rot=  quat(vec3(0,0,1),to)

end

tplayer.OnCollisionEnter=function(col)
  if col.other.name=="portal" then
    trans=0;tolevel='next';trandir=1
    --loadScene('next')
  end
  
end
]]