#ifndef UTILS_H_
#define UTILS_H_

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Text.h>
#include <iostream>
#include <QListView>
#include <QStringListModel>

#include "NGLScene.h"
#include "NGLSceneTreeView.h"
#include "SceneObject.h"
#include "SceneMesh.h"

class Utils
{
  public:
    //get transpose of a matrix as a matrix form;
    static ngl::Mat4 getTranspose( const ngl::Mat4 & _m);
    static ngl::Mat4 getScaleRotate( const ngl::Mat4 & _m);
    const static void printMatrix( const ngl::Mat4 & _m);
};

#endif