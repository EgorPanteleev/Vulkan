//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_MODELMATERIAL_H
#define VULKAN_MODELMATERIAL_H
/*TODO
1) create texture class, which handles vulkan textures
2) load textures as i know
3) change graphics pipeline to create descriptor set per mesh
*/
class ModelMaterial {
public:
    enum TEXTURE_TYPE {
        TEX_TYPE_DIFFUSE = 0,
        TEX_TYPE_NUM = 1
    };
private:
   // Texture* mTextures[TEX_TYPE_NUM];
};


#endif //VULKAN_MODELMATERIAL_H
