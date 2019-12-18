#include"game.h"
#include"../utils/g_dir.h"
#include<iostream>
MyDebugDraw *ddraw;
Label* Game::fpsmeter=0;
float Label::dfsize=16.0;

#define max(a,b) a<b?b:a
#define min(a,b) a<b?a:b

#define norm(a) a<0?0:(a>1?1:a)
Game::Game():running(true)
{
   data=0;
   Rpath=path::GetBasePath();
   Gdir="";   
   printf("Rpath =%s \n",Rpath.c_str());
   #ifndef EMSCRIPTEN
    Gdir=Rpath;
   #endif
}
Skybox* skb1=0;
void Game::InitFBOs(){

    glActiveTexture(GL_TEXTURE0);
    glGenFramebuffers(1, &Light::ShadowBuffer);  
    glBindFramebuffer(GL_FRAMEBUFFER, Light::ShadowBuffer);
    int res=1024*2;Shader::sh_res.x=res;Shader::sh_res.y=res;
    
    glGenTextures(1, &Light::depthMap);
    glBindTexture(GL_TEXTURE_2D, Light::depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,res,res,0,GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    
//     float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Light::depthMap, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        printf("*******Shadow frame buffer complete !!!\n");
    else
        printf("*******Shadow frame buffer INCOMPLETE !!!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    ///
    
    
    /*
    const GLuint SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;
    glGenFramebuffers(1, &Light::shadowCubeFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, Light::shadowCubeFbo);
    // Create depth cubemap texture
    glGenTextures(1, &Light::m_shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Light::m_shadowMap);
    
//     for (GLuint i = 0; i < 6; ++i)
//         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // Attach cubemap as depth map FBO's color buffer
//     glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Light::m_shadowMap, 0);
    for(int i=0;i<6;i++)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, Light::m_shadowMap, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    else
        std::cout << "Framebuffer COMPLEEEEEETEEEEE!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //return GLCheckError();
    */
    
}
void Game::initPhysics(){
    // Build the broadphase
    broadphase = new btDbvtBroadphase();
    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;
    // The world.
    Entity::dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    btDiscreteDynamicsWorld* dynamicsWorld =Entity::dynamicsWorld;
    
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
    
    ddraw   =   new MyDebugDraw;
    ddraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
    Entity::dynamicsWorld->setDebugDrawer(ddraw);
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
    vec2 pres=Shader::getResolution();
//     glViewport(0,0,width,height);
//     glViewport(0,0,width,height);
    Game::resize_window(width,height);
}
void Game::resize_window(int  w,int h){
    glViewport(0,0,w,h);
    float rx=Shader::getResolution().x;
    float ry=Shader::getResolution().y;
    rx/=2.0;ry/=2.0;
//     glViewport(0,0,Shader::sc_res.x,Shader::sc_res.y);
    mat4 vv=mat4::Orthographic(-rx,rx,-ry,ry,0.0f,100.0f);
    Shader* sh=Shader::render2D;
    sh->use();
    glUniformMatrix4fv(glGetUniformLocation(sh->PID,"ortho"), 1, GL_FALSE, vv.elements);
    Game::fpsmeter->setPos(rx-120.0,ry-22.0);
//     rx*=2.0;ry*=2.0;
    
    lua_State* L=Script::L;
    lua_getglobal(L,"resize");
    if( lua_isfunction(L,-1)){
        lua_pushnumber(L,rx*2);
        lua_pushnumber(L,ry*2);
        lua_pcall(L,2,0,0);
    }else{
        lua_pop(L,1);
    }
}
void Game::Start()
{
#ifndef EMSCRIPTEN
    window=new Window("OpenGL",0,0,1280,720,false);
    glfwSetWindowSizeCallback(window->GL_window,window_size_callback);
#else
    window=new Window("OpenGL",0,0,960,540,false);
#endif
    Input::setWindow(window->GL_window);
   
//     glEnable(GL_MULTISAMPLE);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Font::start();
    Audio::Init();
    initPhysics();
    /// LOAD DATA ///////////////////////////////////////////////////////////////////////////////
    camera=Camera::default_camera=new Camera;
    data=new Data(Gdir);
    Shader::Init();
    InitFBOs();
    
    vec3 tint(0.0);
    Shader::pervert->use();
    glUniform3fv(glGetUniformLocation(Shader::pervert->PID,"glow"),1,tint.elements);
    
    printf("number of scenes =%d \n",data->scenes.size());
    Scene* scn=0;
    if(Scene::scenes.empty()){
      scn=new Scene();
      Scene::scenes.push_back(scn);
      Scene::ascenes.push_back(scn);
      Scene::activeScene=scn;
    }
    else{
        scn=(Scene::ascenes.size()>0)?Scene::ascenes[0]:Scene::scenes[0];
    }
    Scene::activeScene=scn;
    
    //////////////////////////////////////////////////////////////////////////////////////////
    
    /// temp
    
    counter=new FpsCounter();
    fpsmeter=new Label("This is a sample text",0,0,vec3(0.6,0.9,0.1),24.0);
//     Font* font=new Font("vera",22);//84 is limit to 512 * 512

     vec2 scres=Shader::getResolution();
    float rx=scres.x/2.0f;float ry=scres.y/2.0f;

    mat4 vv=mat4::Orthographic(-rx,rx,-ry,ry,0.0f,100.0f);
    Shader::render2D->use();
    glUniformMatrix4fv(glGetUniformLocation(Shader::render2D->PID,"ortho"), 1, GL_FALSE, vv.elements);
    
    /// *****
     Init();
    //////////
    for(int i=0;i<data->shaders.size();i++){
      Light::setAmbient(data->shaders[i],vec3(0.09));
      //glUniform3f(data->shaders[i]->uniAmbient,0.5f,0.5f,0.5f);
    }
    skb1=new Skybox();
    if(CubeMap::cubemaps.size())skb1->cm=CubeMap::cubemaps[0];
}
void Game::Init(){
  glfwSwapInterval(0); 
  Scene::activeScene->Play();
  Scene::play=true;
//   glfwSetInputMode(window->GL_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   vec2 scres(960.0,540.0);
    float rx=scres.x/2.0f;
    float ry=scres.y/2.0f;

    mat4 vv=mat4::Orthographic(-rx,rx,-ry,ry,0.0f,100.0f);
    Shader::render2D->use();
    //glUniformMatrix4fv(glGetUniformLocation(Shader::render2D->PID,"ortho"), 1, GL_FALSE, vv.elements);
}
float thetay1=0,thetax1=0;
void editor_camera()
{
    /////////////
    Camera* camera=Camera::default_camera;
    mat4 &view=camera->view;
    vec3 &forward=camera->forward,&strafe=camera->strafe,&pos=camera->pos;
    
    view=mat4(1.0);
    
   float df=0,ds=0;
   float camspeed=40.0f*Input::deltaTime;
     
   if(Input::getKey(KEY_LSHIFT)){camspeed*=4;}
   if(Input::getKey(KEY_LCTRL)){camspeed/=4;}
   
   if(Input::getKey(KEY_W)){df=camspeed;}
   if(Input::getKey(KEY_A)){ds=-camspeed;}
   if(Input::getKey(KEY_S)){df=-camspeed;}
   if(Input::getKey(KEY_D)){ds=camspeed;}

   if(Input::getKey(KEY_Q)){pos.y+=-camspeed;}
   if(Input::getKey(KEY_E)){pos.y+=camspeed;}
    
  view[12]=-pos.x;
  view[13]=-pos.y;
  view[14]=-pos.z;
    
//   if(Input::getMouseButton(1))
  {
   thetay1= Input::getMousedelta().x *0.005;
   thetax1= Input::getMousedelta().y *0.005;
   camera->rot= quat::RotationX(thetax1)*camera->rot*quat::RotationY(thetay1);
  }
  camera->dir=quat::Rotate(camera->rot.inverse(),-vec3::Zaxis());
  view=mat4::Rotate(camera->rot)*view;
  
   forward= vec3(-view.elements[2],-view.elements[6],-view.elements[10]);
   strafe=  vec3(view.elements[0],view.elements[4],view.elements[8]);
   
  pos+=forward* (df);
  pos+=strafe* (ds);
  
}
bool debdraw=false;
void Game::Render()
{ 
    ///////////////////////////
  glfwPollEvents();
    float camspeed=0.4f;
    editor_camera();

   running=!glfwWindowShouldClose(window->GL_window);
   if(input.getKeyDown(LCTRL,KEY_Q)){running=false;}
    
    glfwMakeContextCurrent(window->GL_window);

    vec3 col(0.088,0.084,0.08);
    glClearColor(col.x,col.y,col.z,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(Camera::active_camera==Camera::default_camera)
	{Camera::active_camera->update(0);};
    
//     skb1->tick();
    // step physics

    Entity::dynamicsWorld->stepSimulation(Input::deltaTime,4);
    Scene::activeScene->tick();    
    if(Input::getKeyDown(KEY_KP_SUBTRACT))debdraw=!debdraw;
#ifndef EMSCRIPTEN
    if(debdraw)
    Entity::dynamicsWorld->debugDrawWorld();
#endif  
     Input::reset();
    char fbuff[50];
    snprintf(fbuff, sizeof(fbuff), "FPS=%.2f",counter->fps);
    fpsmeter->setText(fbuff);
    if(counter->fps >55){
            fpsmeter->color= vec3(0.6,0.9,0.1);
    }
    else if(counter->fps>40){
         fpsmeter->color=vec3(0.9,0.9,0.1);
    }
    else if(counter->fps>30){
         fpsmeter->color=vec3(0.9,0.5,0.1);
    }
    else{
         fpsmeter->color=vec3(0.7,0.1,0.1);
    }
    vec2 scress=Shader::getResolution();
    snprintf(fbuff, sizeof(fbuff),"FPS=%.1f",counter->fps);
    fpsmeter->setText(fbuff);

//     printf("screen res= %f x %f \n",scress.x,scress.y);

    fpsmeter->tick();
    counter->update();
    Button::b2=Button::b1;
    glfwSwapBuffers(window->GL_window);
}

Game::~Game()
{
    delete Entity::dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
    delete ddraw;
    delete counter;
    
   Cleanup();
//    Script::end(); //done in ~data()
   Font::cleanUp();
   Audio::Terminate();
   
}
void Game::Cleanup()
{
  delete Camera::default_camera;
  delete data;
  delete window;
}


