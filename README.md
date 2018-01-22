# Overview
This purpose of this project is purely educational.
## IDs:
- All of the objects that are instanced or referenced are given a unique ID to index them in-game:
  - IDs are prefixed by the addonID(a unique id given to index the addons), a typeID, and the actual itemID.
  - This method results in a ID scheme whith lots of unused IDs, but also makes it easier to search for specific objects by different properties.
  - Since the addon id is set by the addon creator and there is no guarantee that the addons won't have conflicting IDs, there will have to be a system in place to resolve conflicting IDs in a way that is reproducable, possible by storing the addon name and the newly assigned ID.
 
## Classes:
### Renderer:
- Holds the Vulkan context as well as all global vulkan objects
  - Device
  - Window
  - Swapchain
  - Imageviews
  - Render Pass
  - Command Pool
  - Semaphores
  - Descriptor Pool
  - Framebuffer
  - Surface
### RenderObject:
- Is purely virtual, each child of RenderObject is required to implement a pipeline for itself.
- Stores the area of memory each part of it is loaded into memory in the scene, with an offset and a size
- Methods
  - getData() for returning the info needed for rendering, tightly packed in a specific order
    - First byte, flags for the data, describing what data is going to be returned
    - 2 byte segments giving the amount of data in each buffer(eg. the number of vertices in the buffer)
      - This allows for each function to return a max of 65,536 vertices as each vertex is 3*sizeof(float) large
    - Next segments are the actual data being passed
  - recordDraw() for recording the required draw commands to the currently recording renderpass.
  - assignDescriptorSet() returns a descriptorset to be used with the renderObjects pipeline
  - unassingDescriptorSet() frees the Vulkan descriptor set
  - updateDescriptorSet() takes in 
  - getPipeline() as an access method
### Object:
  - Since renderobjects can be used for multiple physical objects, Object stores the positional and physical aspects of the object and represents the instance of an object.
  - Contains a pointer to it's renderObject, or nullptr if it is not a renderable object.
  - Contains it's instanced object ID.
  - Only method so far is a constructor to create the item given an objectID to be looked up in the addon data
  - Contains a DescripterObject which contains all the information needed to upadte and use the uniform buffer object associated with the object.
### Addons:
  - Addons are the meat and potatoes of the rendering. All pimitives, objects, renderobjects, textures, maps, and anything inbetween is stored in addon files.
  - The first couple ids are reserved for core addons(primitives, core objects, etc.), while the rest can be used however.
  - On startup the engine will load the headers for the addons, resolve any conflicts, and store some basic info about the addons.
  - The addons class is the main interface between the engine and actual data for the application(meshes and such)
### Scene:
  - Scenes are what stores all the textures, meshes, objects, and such that is currently loaded in the game.
  - At some point there will need to be a method to save and load scenes into files so that there is some form of persistence.
  - Scenes are made up of a list of objects and the buffers that store all the render data for those objects renderobjects.
  - Scenes are where the main update function will eventually be.
  - Stores the command buffer for drawing the scene, needs to be updated whenever objects are added or removed from the scene.ffer 
  - Stores the VkDeviceMemory that points to the buffers as well.
  - Since renderobjects can be loaded and unloaded from the buffers whenevere, there is a function to optimize the buffer usage, repacking the buffers as tight as can be so that there isn't big chunks of unused data in use by the gpu.
  
Well, that's all for now, as said previously the purpose of this engine is purely educational, so any flaws/strange decisions are either for the sake of learning or lack of knowledge.
