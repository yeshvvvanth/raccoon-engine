import bpy
import struct
import bmesh
import time
import mathutils
import math

def get_keyframes(obj):
    keyframes = []
    if obj is not None:
        anim = obj.animation_data
        if anim is not None and anim.action is not None:
            for fcu in anim.action.fcurves:
                for keyframe in fcu.keyframe_points:
                    x, y = keyframe.co
                    if x not in keyframes:
                        keyframes.append((math.ceil(x)))
    return keyframes

def saveMeshes(selff):
    global_matrix=mathutils.Matrix()
    global_matrix=mathutils.Matrix.Rotation(-math.pi / 2.0, 4, 'X')
    stime=(time.time())

    def write(*args):
        if(len(args)==1):
            file.write(args[0].encode())
        else:
            file.write(struct.pack(*args))
    print("exporting.........")
    file = open(selff.filepath, 'bw')
    fpos=file.tell()
    write('H',1)                     #<=================    #write number of meshes being saved
    T_BONE='B'
    N_VERT=6
    num_meshes=0   #number of meshes
    numanuma=0;puma=0;mama=0
    #/////////
    #obj=bpy.context.scene.objects.active
    #if obj!=None:
    for obj in (bpy.context.selected_objects):
        if(obj.type=='MESH'):
            B_TEXT=2;B_RIGGED=4;B_ANIM=8;
            M_TYPE=1;
            
            num_meshes+=1;

            bpy.context.scene.objects.active=obj;#making current object as selected
            mesh=obj.data
            ## specifying mesh type
            if (mesh.uv_layers.active!=None):M_TYPE+=B_TEXT
            if(obj.parent!=None and obj.parent.type=='ARMATURE'):
                M_TYPE+=B_RIGGED
                if(obj.parent.animation_data is not None):
                    M_TYPE+=B_ANIM
                bpy.ops.object.vertex_group_limit_total();
                print("vertex groups limited.. bitches!!!!")
            ##
            bpy.ops.object.transform_apply( scale = True )
            obj.rotation_euler=(-math.pi/2,0,0)
            bpy.ops.object.transform_apply(rotation = True)

            mama+=1;
            ## triangulating mesh
            bm=bmesh.new();bm.from_mesh(mesh)
            bmesh.ops.triangulate(bm,faces=bm.faces);print("mesh triangulated bitches!!!!")
            bm.to_mesh(mesh);bm.free()
            #end 
            v_list=[];vertices=[];elements=[];vvl=[]# vbl vertices ,mesh.vertices map
            uvs=[];ccs=[]
            numvert=len(mesh.vertices)
            for m in range(numvert):
                tist=[0]
                v_list.append(tist)

            flat=selff.shade_flat
            if (flat):
                if (M_TYPE&B_TEXT): # if mesh is textured
                    uv_layer =mesh.uv_layers.active.data
                    print("saving uv mapped model bitches!!!!")
                    for pol in mesh.polygons:
                        norm=pol.normal
                        for i,loop_index in enumerate(pol.loop_indices):
                            puma+=1  #for testing
                            vertID=pol.vertices[i]
                            v=mesh.vertices[vertID]
                            uv=uv_layer[loop_index].uv
                            numanuma+=1  #for testing #
                            new_vert=int(len(vertices)/N_VERT)
                            elements.append(new_vert)

                            vertices.extend([v.co.x,v.co.y,v.co.z,norm.x,norm.y,norm.z]);
                            uvs.extend((uv[0],1-uv[1]))
                            vvl.append(vertID)
                            
                else:
                    print("saving NON-UV mapped model bitches!!!!")
                    for pol in mesh.polygons:
                        norm=pol.normal
                        for i,loop_index in enumerate(pol.loop_indices):
                            puma+=1  #for testing
                            vertID=pol.vertices[i]
                            v=mesh.vertices[vertID]
                            mat_ind=pol.material_index
                            
                            numanuma+=1  #for testing #
                            new_vert=int(len(vertices)/N_VERT)
                            elements.append(new_vert)
                            if (not mesh.materials) or (mesh.materials[mat_ind] is None):
                                col=mathutils.Color((0.5,0.5,0.5))
                            else:
                                col=mesh.materials[mat_ind].diffuse_color

                            vertices.extend([v.co.x,v.co.y,v.co.z,norm.x,norm.y,norm.z]);
                            ccs.extend((col[0],col[1],col[2]))
                            vvl.append(vertID)
            
            else:
                if (M_TYPE&B_TEXT): # if mesh is textured
                    uv_layer =mesh.uv_layers.active.data
                    print("saving uv mapped model bitches!!!!")
                    for pol in mesh.polygons:
                        for i,loop_index in enumerate(pol.loop_indices):
                            puma+=1  #for testing
                            vertID=pol.vertices[i]
                            v=mesh.vertices[vertID]
                            uv=uv_layer[loop_index].uv
                            num=v_list[vertID][0]
                            exists=False;vert_num=0
                            for n in range(num-1):
                                if(v_list[vertID][1+(3*n)]==uv[0] and v_list[vertID][1+(3*n+1)]==1-uv[1]):
                                    vert_num=v_list[vertID][1+(3*n+2)]
                                    exists=True
                                    break
                            if(exists):
                                elements.append(vert_num)
                            else:
                                numanuma+=1  #for testing #
                                new_vert=int(len(vertices)/N_VERT)
                                elements.append(new_vert)

                                vertices.extend([v.co.x,v.co.y,v.co.z,v.normal.x,v.normal.y,v.normal.z]);
                                uvs.extend((uv[0],1-uv[1]))
                                vvl.append(vertID)

                                v_list[vertID][0]+=1
                                v_list[vertID].append(uv[0])
                                v_list[vertID].append(1-uv[1])
                                v_list[vertID].append(new_vert)
                else:
                    print("saving NON-UV mapped model bitches!!!!")
                    for pol in mesh.polygons:
                        for i,loop_index in enumerate(pol.loop_indices):
                            puma+=1  #for testing
                            vertID=pol.vertices[i]
                            v=mesh.vertices[vertID]
                            mat_ind=pol.material_index
                            num=v_list[vertID][0]
                            exists=False;vert_num=0
                            for n in range(num):
                                if(v_list[vertID][1+(2*n)]==mat_ind):
                                    vert_num=v_list[vertID][1+(2*n+1)]
                                    exists=True
                                    break
                            if(exists):
                                elements.append(vert_num)
                            else:
                                numanuma+=1  #for testing #
                                new_vert=int(len(vertices)/N_VERT)
                                elements.append(new_vert)
                                if (not mesh.materials) or (mesh.materials[mat_ind] is None):
                                    col=mathutils.Color((0.5,0.5,0.5))
                                else:
                                    col=mesh.materials[mat_ind].diffuse_color


                                vertices.extend([v.co.x,v.co.y,v.co.z,v.normal.x,v.normal.y,v.normal.z]);
                                ccs.extend((col[0],col[1],col[2]))
                                vvl.append(vertID)

                                v_list[vertID][0]+=1
                                v_list[vertID].append(mat_ind)
                                v_list[vertID].append(new_vert)
            
                        print("no uv mapping bitches!!")

            # writing len of vertices & elements #
            vs=int(len(vertices)/N_VERT)
            write('B',M_TYPE);print("type = %d\n\n"%M_TYPE)
            write('B',len(obj.name))
            write(obj.name);                         print("name written :^__^:")
            write('I',len(elements));
            write('%dI'%len(elements),*elements)
            write('I',vs);
            write('%df'%len(vertices),*vertices)
            if (M_TYPE&B_TEXT):
                write('%df'%len(uvs),*uvs)
            else:
                write('%df'%len(ccs),*ccs)
            # Bounding Box    
            for i in range(3):
                write('f',obj.bound_box[0][i])
            for i in range(3):
                write('f',obj.bound_box[6][i])
                
            print("len of vertices =%d"%len(vertices))
            print("len of uvs =%d"%len(uvs))
            #IF MESH HAS ARMATURE#
            if(obj.parent!=None and obj.parent.type=='ARMATURE'):
                obones=obj.parent.data.bones
                # maps old bone index => new (heirarchical)bone id #
                bone_map=[255]*len(obones)
                # maps vertex group => old bone index=> heirarchical bone id #
                groupToBone=[]
                bnew=[]
                # maps new bone ids => original bone ids # opposite of bone_map  #
                hb=[]
                # ordering bones in heirarchical order and storing them in hb[] #
                for bon in (obones):
                    if bon.use_deform or (not bon.parent):
                        dep=0;tb=bon;hen=len(hb)
                        while(tb.parent):
                            dep+=1
                            tb=tb.parent
                        if(hen<dep+1):
                            for g in range(dep+1-hen):
                                hb.append([])
                        hb[dep].append(bon.name)
                        #adding parents#
                        tb=bon
                        while(dep>0):
                            if not(tb.parent.name in hb[dep-1]):
                                hb[dep-1].append(tb.parent.name)
                            tb=tb.parent
                            dep-=1
                for hobo in hb:
                    for bnam in hobo:
                        ob_id=obones.find(bnam)
                        bone_map[ob_id]=len(bnew)
                        bnew.append(ob_id)
                # don't reverse the order of the above two lines #
                print('number of bones=%d'%len(bnew))
                print('new bone to old bone map :')
                print(bnew)
                print('old bone to new bone map :')
                print(bone_map)
                # saving bones #
                nbones=[]
                for I,bID in enumerate(bnew):
                    bone=obones[bID];cbn=[];childlist=[]
                    trans=global_matrix*bone.matrix_local;tramlist=[]
                    off=(global_matrix*bone.matrix_local).inverted();offmlist=[]
                    # (opt)don't forget to transform mesh space => armature space

                    off=off*obj.matrix_local # check this one !!!!!!!!!!!!!!

                    # storing bones #
                    cbn.append(I)
                    cbn.append(bone.use_deform)
                    cbn.append(len(bone.name))
                    cbn.append(bone.name);print("%d %s"%(I,bone.name))
                    if bone.parent:
                        # add parent id
                        cbn.append(bone_map[obones.find(bone.parent.name)])
                        # calc transform relative to parent #
                        trans=(global_matrix*bone.parent.matrix_local).inverted()*trans
                    else:
                        #add 255 if no parent
                        cbn.append(255)
                        print("%s has no parent"%bone.name)
                    for c in bone.children:                    # saving children
                        ti=obones.find(c.name)
                        if ti in bnew:                        # if child bone is either deform or parent of one
                            childlist.append(bone_map[ti])
                    cbn.append(childlist)
                    for i in range(4):
                        for j in range(4):
                            tramlist.append(trans[j][i])
                    cbn.append(tramlist)
                    for i in range(4):
                        for j in range(4):
                            offmlist.append(off[j][i])
                    cbn.append(offmlist)
                    nbones.append(cbn)
                # end of storing bones.....phew!  #

                # map groups to bone and store it in 'groupToBone' list#
                for vg in obj.vertex_groups:
                    obid=obones.find(vg.name)             # find old bone index of vertex group name
                    if obid!=-1:
                        groupToBone.append(bone_map[obid])    # map new bone id via bone_map
                    else:
                        groupToBone.append(-1)
                #print((bone_map))
                # writing vertex data #
                ####write('I',(len(vertices)));
                for nu in range(vs):
                    #####write('%df'%N_VERT,*vtx)
                    bids=[0,0,0,0];weights=[0,0,0,0]       #write bone ids and weights here
                    o_vert=mesh.vertices[vvl[nu]]          # vvl= original vertex id
                    for n,gp in enumerate(o_vert.groups):
                        bi=groupToBone[gp.group]
                        if(bi!=-1):
                            bids[n]=bi
                            weights[n]=gp.weight
                    write('4'+T_BONE,*bids)
                    write('4f',*weights)
                print('Armature loaded bitches!!!!')
                # writing bones #
                print("number of bones overall=%d"%len(bnew))
                write(T_BONE,len(bnew))
                check=0
                for boan in nbones:
                    write(T_BONE,boan[0])                           # bone id
                    write('B',boan[1])                               #booleaen id use_deform
                    write('B',boan[2])                               # bone name len
                    write(boan[3])                                   #name is a string so...
                    write(T_BONE,boan[4])                             # parent bone id
                    #print("%d par= %d"%(check,boan[4]))
                    children=boan[5];cs=len(children)
                    write(T_BONE,cs)                                 # number of children
                    if cs>0:write(('%d'+T_BONE)%cs,*children) # write child ids if there are children
                    write('16f',*boan[6])                           # transform matrix
                    if boan[1]:write('16f',*boan[7])                   #offset matrix
                    check+=1
                # EXPORT ANIMATIONS

                abones=[];acc=0;bcc=0
                actions=bpy.data.actions
                sc=bpy.context.scene


                if M_TYPE&B_ANIM:
                    write('B',len(actions))# number of actions (animations)
                    print("number of actions =%d"%len(actions))
                    for aID,act in enumerate(actions):
                        start=(int)(act.frame_range[0]);end=(int)(act.frame_range[1]);
                        obj.parent.animation_data.action=act
                        #obj.animation_data.action=act
                        #write
                        write('B',aID)              # id of action (animation)
                        write('B',len(act.name))   # n_len of action name
                        write(act.name)          # action name
                        write('B',sc.render.fps)
                        write('H',end-start) # number of frames  (to calc duration and time)
                        pose=obj.parent.pose
                        #write(T_BONE,len(pose.bones)) # number of aBones
                        #loop through Posebones
                        pbones=[];f_nums=[];pIDs=[]
                        for obo in (bnew):
                            pid=pose.bones.find(obones[obo].name)
                            pIDs.append(pid)
                            pbones.append([])
                            f_nums.append([])
                            #write
                            #write(T_BONE,nbid) # id of Bone linked to it
                            # loop through key frames after seeking to beginning
                            #sc.frame_set(start)

                        print('start=%d,end=%d'%(start,end))
                        acc+=1

                        #pp={'FINISHED'};ep={'FINISHED'}
                        bpy.context.scene.frame_set(start)

                        keyframes=get_keyframes(obj.parent)

                        print(obj.parent.animation_data.action.name)
                        for kf in keyframes:
                            sc.frame_set(kf)
                            print(sc.frame_current)
                            for Is,pb in enumerate(pbones):
                                B=pIDs[Is]

                                f_nums[Is].append(sc.frame_current)
                                matrix=global_matrix*pose.bones[B].matrix
                                #matrix=pose.bones[B].matrix_channel
                                if pose.bones[B].parent:
                                    matrix=(global_matrix*pose.bones[B].parent.matrix).inverted()*matrix
                                    #matrix=pose.bones[B].parent.matrix_channel.inverted()*matrix
                                (position,rotation,scale)=matrix.decompose()
                                #print(rotation)
                                #save pose matrix
                                for l in range(3):
                                    pb.append(position[l])

                                for l in range(4):
                                    pb.append(rotation[l])

                                for l in range(3):
                                    pb.append(scale[l])

                                bcc+=1 #testing

                            #pp=bpy.ops.screen.keyframe_jump(next=True)# to next keyframe
                            #sc.frame_set(sc.frame_current+1)
                        num_keys=int(len(pbones[0])/10)
                        #print('number of keyframes=%d'%num_keys)       ###
                        #print('number of frames=%d'%(end-start))
                        write('H',num_keys)

                        print("number of pbones=%d ,%d"%(len(pbones),len(f_nums)))

                        for Is,p in enumerate(pbones):
                            write(('%d'+T_BONE)%num_keys,*f_nums[Is])
                            write('%df'%len(p),*p)  # yoooylooooooooo !!!!!!!!!!!!!!!

                    print('acc=%d,bcc=%d'%(acc,bcc))
            else:
                print("mesh does't have armature bitches!!!! ")
            
        
        obj.rotation_euler=(math.pi/2.0,0,0)
        bpy.ops.object.transform_apply( rotation = True )

    file.seek(fpos,0)
    write('H',num_meshes)
    file.close()
    print('number of meshes= %d'%mama)
    print('number of elements= %d'%puma)
    print('number of vertices= %d'%numanuma)
    etime=(time.time())
    print(etime-stime)
