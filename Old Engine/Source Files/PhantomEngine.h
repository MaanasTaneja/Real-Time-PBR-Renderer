#define STB_IMAGE_IMPLEMENTATION
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
#define MSAA_SAMPLE_LEVEL 12
#define PE_CYCLES 0
#define PE_EEVE 1
#define PE_WORKBENCH 2


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

#include "khrplatform.h"
#include "glad/glad.h"
#include "glad/glad.c"
#include "glfw3.h"
#include <windows.h>
#include <iostream>
#include "ShaderHandler.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/easing.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/random.hpp"
#include <stb_image.h>
#include "Bullet/src/btBulletDynamicsCommon.h"
#include "Bullet/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#include "Bullet/src/BulletCollision/CollisionShapes/btShapeHull.h"
#include "Bullet/src/BulletSoftBody/btSoftBodyHelpers.h"
#include "Bullet/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "Bullet/src/BulletSoftBody/btDefaultSoftBodySolver.h"
#include "Bullet/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "openAL/al.h"
#include "openAL/alc.h"
#include "openAL/efx.h";


//CORE ENGINE CODE
#include "SoundEngine.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "TextureHandler.h"
#include "TexturedModel.h"

#include "ShaderHandler.h"
#include "Entity.h"
#include "Camera.h"
#include "EntityRenderer.h"
#include "VertexObjects.h"
#include "FrameBufferObjects.h"
#include "Skybox.h"
#include "SkyboxRenderer.h"
#include "HeightsGenerator.h"
#include "PrimitiveMaker.h"
#include "LiquidBody.h"
#include "LiquidBodyRenderer.h"
#include "Light.h"
#include "LightRenderer.h"
#include "Particle.h"
#include "ParticlePhysics.h"
#include "ParticleRenderer.h"
#include "ParticleSystem.h"
#include "ParticleMaster.h"

//#include "AreaLights.h"
//#include "AreaLightsRenderer.h"

//ENGINE EXTENSIONS

#include "PostProcesser.h"

#include "PhysicsDebugDrawer.h"
#include "Physics.h"

//GAME SPECIFIC MECHANICS

#include "Gun.h"
#include "GunContainer.h"
#include "Player.h"
#include "PlayerRenderer.h"
#include "LineRenderer.h"
#include "Grapple.h"
#include "SoftBodyRenderer.h"
#include "Lava.h"
#include "LavaRenderer.h"
#include "RadiostyUtils.h"
#include "RadiostyLightmapper.h"

//MASTER RENDERER

#include "Loader.h"
#include "Terrain.h"
#include "TerrainRenderer.h"
#include "Precomputer.h"
#include "MasterRenderer.h"

#include "LevelBuilder.h"





