# Custom Game Engine

Custom map creator/Game engine for my multiplayer shooter. As development has progressed this map editor
has turned into more of a custom game engine.

# Development phases:
- Modernise old Vulkan rendering code 🟢
  - Update descriptor set code
  - Implement VMA
- Asset and object management 🟢
  - Texture and Mesh loading
  - Vulkan buffer updates
- Implement ImGUI <- Currently here 
  - Adding buttons for creating new objects
  - Fields for editing an objects mesh/texture/material
  - etc..
-  Implement map loading and saving

# Libraries
- stb_image
- tiny_obj_loader
- VMA
- ImGUI

Developed and tested on Linux.

Shoutout to these sites for providing tutorials, explanations, and code:
- https://vulkan-tutorial.com/
- https://vkguide.dev/
- https://lesleylai.info/en/vk-khr-dynamic-rendering/
