//
// Created by auser on 6/12/25.
//

#include "ModelMaterial.h"

aiTextureType ModelTexture::toAssimpType(Type type) {
    return toAssimpTypeMap[type];
}