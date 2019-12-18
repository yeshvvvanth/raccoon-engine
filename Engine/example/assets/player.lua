
up=vec3(0,1,0)
inp=vec3(0,0,0);ay=0;ax=0
input=script();menu=script()
dt=0.02;zoom=40
euler= Quat.fromEuler
rotate=Quat.rotate

trans=-1;tolevel="next"
trandir=1
transition=function()
  if trans<=1 and trans >=0 then
    trans=trans+trandir*dt*3
    findEntity('black').button.alpha=trans
  elseif trans>1 then
     trans=1;loadScene(tolevel);
  end
end

-- Common Funtion for all scenes !!!
input.Init=function() trandir=-1; end
input.Update=function()
  inp=vec3()
  if getKey(keys.w) then inp.y=1 end
  if getKey(keys.s) then inp.y=-1 end
  if getKey(keys.d) then inp.x=1 end
  if getKey(keys.a) then inp.x=-1 end
  
  ay=math.min(ay,89);ay=math.max(ay,-60)
  ax=ax-getMouseDelta().x
  ay=ay-getMouseDelta().y
  
  if getKeyDown(keys.escape) then trans=0;tolevel='menu';trandir=1 end
  
  dt=Time.deltaTime
  transition()
end
menu.Update=function()
  if findEntity("play").button.justDown then
     loadScene('next')
  end
end
function resize(w,h)
  print("resize the screen to "..w.."  "..h) 
  local buttons=findEntity('ui').children
  for k,v in pairs(buttons) do
      if v.button then
        v.button.x=-w/2+120-v.button.width/2
      end
  end
end

fplayer=script()
fplayer.Update=function()
  cam=findEntity('cam')
  ent.rb.linearDamping=0.99999
  
  forward=Quat.rotate(cam.rot,vec3(0,0,-1))
  forward=norm(forward*vec3(1,0,1))
  right=cross(-1*up,forward)
  
  cam.rot= Quat.fromEuler(0,ax,0)*  Quat.fromEuler(ay,0,0)

  imp=inp.x*right+inp.y*forward
  if getKeyDown(keys.space) then ent.rb.applyImpulse((up)*6) end
  ent.rb.applyImpulse((imp)*dt*180)
  
  setAmbientLight(vec3(0.6))
  
end






