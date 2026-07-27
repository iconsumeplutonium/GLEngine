This is a simple engine written in OpenGL and ImGUI.

* Scene editor UI, built with ImGui, to add, select, transform, and delete objects/lights at runtime
* Free-form transform controls editable live via the inspector panel
* Point lights, spotlights, and directional lights via a UBO (with settings UI)
* Material system to swap between flat-color, diffuse-textured, and fully lit materials per object, at runtime
* Model and basic primitive loading via Assimp
* Objects grouped and sorted by shader program to minimize state changes between shader program switching